#include "rvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>

struct undo_record {
    size_t offset;
    size_t size;
    void *old_data;
    struct undo_record *next;
};

struct trans {
    trans_t tid;
    bool active;
    struct undo_record *undos;
};

struct rvm_t {
    char *log_file;
    char *backing_file;
    size_t size;
    int log_fd;
    void *seg_base;
    
    struct trans current_tx;
    trans_t next_tid;
};

typedef struct {
    size_t offset;
    size_t size;
} log_record_header_t;

#define TX_COMMIT_MAGIC ((size_t)-1)

static void apply_log(rvm_t *rvm) {
    int fd = open(rvm->log_file, O_RDONLY);
    if (fd < 0) return; // No log file yet

    struct stat st;
    fstat(fd, &st);
    if (st.st_size == 0) {
        close(fd);
        return;
    }

    // Read log and apply to backing file
    int back_fd = open(rvm->backing_file, O_RDWR | O_CREAT, 0644);
    if (back_fd < 0) {
        perror("Failed to open backing file for recovery");
        close(fd);
        return;
    }

    // We must only apply committed transactions.
    // We will parse the log, keep a list of pending writes, and apply them on commit.
    // For simplicity in this lab, we can read the whole log into memory.
    void *log_data = malloc(st.st_size);
    ssize_t read_bytes = read(fd, log_data, st.st_size);
    if (read_bytes == st.st_size) {
        size_t pos = 0;
        size_t tx_start_pos = 0;
        
        while (pos + sizeof(log_record_header_t) <= (size_t)st.st_size) {
            log_record_header_t *hdr = (log_record_header_t *)((char*)log_data + pos);
            if (hdr->offset == TX_COMMIT_MAGIC && hdr->size == TX_COMMIT_MAGIC) {
                // Commit! Apply all changes from tx_start_pos to pos
                size_t p = tx_start_pos;
                while (p < pos) {
                    log_record_header_t *rec = (log_record_header_t *)((char*)log_data + p);
                    p += sizeof(log_record_header_t);
                    ssize_t pw_res = pwrite(back_fd, (char*)log_data + p, rec->size, rec->offset);
                    (void)pw_res;
                    p += rec->size;
                }
                tx_start_pos = pos + sizeof(log_record_header_t);
                pos = tx_start_pos;
            } else {
                if (pos + sizeof(log_record_header_t) + hdr->size > (size_t)st.st_size) {
                    break; // Incomplete record
                }
                pos += sizeof(log_record_header_t) + hdr->size;
            }
        }
    }
    free(log_data);
    fsync(back_fd);
    close(back_fd);
    close(fd);
    
    // We've recovered into backing_file. Truncate log to avoid re-applying.
    int tr_res = truncate(rvm->log_file, 0);
    (void)tr_res;
}

rvm_t* rvm_init(const char *log_file, const char *backing_file, size_t size) {
    rvm_t *rvm = calloc(1, sizeof(rvm_t));
    rvm->log_file = strdup(log_file);
    rvm->backing_file = strdup(backing_file);
    rvm->size = size;
    rvm->next_tid = 1;

    // Apply any existing log to the backing file
    apply_log(rvm);

    // Open log file for appending
    rvm->log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    // Ensure backing file exists and is of correct size
    int back_fd = open(backing_file, O_RDWR | O_CREAT, 0644);
    int ft_res = ftruncate(back_fd, size);
    (void)ft_res;
    
    // Map backing file into memory privately
    // We use MAP_PRIVATE so our modifications aren't flushed directly to disk
    // behind the scenes by the OS page cache.
    rvm->seg_base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, back_fd, 0);
    close(back_fd);

    return rvm;
}

trans_t rvm_begin_transaction(rvm_t *rvm) {
    if (rvm->current_tx.active) {
        return -1; // Only one active tx in this simple implementation
    }
    rvm->current_tx.active = true;
    rvm->current_tx.tid = rvm->next_tid++;
    rvm->current_tx.undos = NULL;
    return rvm->current_tx.tid;
}

void rvm_set_range(rvm_t *rvm, trans_t tid, void *addr, size_t size) {
    if (!rvm->current_tx.active || rvm->current_tx.tid != tid) return;
    
    struct undo_record *undo = malloc(sizeof(struct undo_record));
    undo->offset = (char*)addr - (char*)rvm->seg_base;
    undo->size = size;
    undo->old_data = malloc(size);
    memcpy(undo->old_data, addr, size);
    
    undo->next = rvm->current_tx.undos;
    rvm->current_tx.undos = undo;
}

void rvm_end_transaction(rvm_t *rvm, trans_t tid, bool no_flush) {
    if (!rvm->current_tx.active || rvm->current_tx.tid != tid) return;

    // Write redo records to log
    struct undo_record *curr = rvm->current_tx.undos;
    while (curr) {
        log_record_header_t hdr;
        hdr.offset = curr->offset;
        hdr.size = curr->size;
        ssize_t w1 = write(rvm->log_fd, &hdr, sizeof(hdr));
        ssize_t w2 = write(rvm->log_fd, (char*)rvm->seg_base + curr->offset, curr->size);
        (void)w1; (void)w2;
        curr = curr->next;
    }

    // Write commit marker
    log_record_header_t commit_hdr = {TX_COMMIT_MAGIC, TX_COMMIT_MAGIC};
    ssize_t w3 = write(rvm->log_fd, &commit_hdr, sizeof(commit_hdr));
    (void)w3;

    if (!no_flush) {
        fsync(rvm->log_fd);
    }

    // Free undos
    curr = rvm->current_tx.undos;
    while (curr) {
        struct undo_record *next = curr->next;
        free(curr->old_data);
        free(curr);
        curr = next;
    }
    rvm->current_tx.active = false;
}

void rvm_abort_transaction(rvm_t *rvm, trans_t tid) {
    if (!rvm->current_tx.active || rvm->current_tx.tid != tid) return;

    // Restore undos
    struct undo_record *curr = rvm->current_tx.undos;
    while (curr) {
        memcpy((char*)rvm->seg_base + curr->offset, curr->old_data, curr->size);
        struct undo_record *next = curr->next;
        free(curr->old_data);
        free(curr);
        curr = next;
    }
    rvm->current_tx.active = false;
}

void rvm_truncate(rvm_t *rvm) {
    // Truncation: apply log to backing file and clear log.
    // Since our mmap is MAP_PRIVATE, we must write our in-memory changes to the backing file directly,
    // or we can sync the log file to backing file.
    // It's easier to just call apply_log, which reads the log and writes to backing file.
    // But apply_log will open the file again. Let's just flush log, close it, apply it, and reopen.
    fsync(rvm->log_fd);
    close(rvm->log_fd);
    apply_log(rvm);
    rvm->log_fd = open(rvm->log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
}

void rvm_destroy(rvm_t *rvm) {
    if (!rvm) return;
    munmap(rvm->seg_base, rvm->size);
    close(rvm->log_fd);
    free(rvm->log_file);
    free(rvm->backing_file);
    free(rvm);
}

void* rvm_get_segment(rvm_t *rvm) {
    return rvm->seg_base;
}
