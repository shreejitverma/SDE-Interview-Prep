#include "rvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

#define SEG_SIZE 4096
#define LOG_FILE "test_rvm.log"
#define BACKING_FILE "test_rvm.data"

void test_abort() {
    printf("--- Running test_abort ---\n");
    rvm_t *rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    char *seg = rvm_get_segment(rvm);
    
    strcpy(seg, "initial");
    
    trans_t tid = rvm_begin_transaction(rvm);
    rvm_set_range(rvm, tid, seg, 10);
    strcpy(seg, "aborted!!");
    rvm_abort_transaction(rvm, tid);
    
    assert(strcmp(seg, "initial") == 0);
    printf("PASS: test_abort\n");
    rvm_destroy(rvm);
}

void test_commit() {
    printf("--- Running test_commit ---\n");
    rvm_t *rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    char *seg = rvm_get_segment(rvm);
    
    trans_t tid = rvm_begin_transaction(rvm);
    rvm_set_range(rvm, tid, seg, 20);
    strcpy(seg, "committed_data");
    rvm_end_transaction(rvm, tid, false);
    
    rvm_destroy(rvm);
    
    // Now recover
    rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    seg = rvm_get_segment(rvm);
    assert(strcmp(seg, "committed_data") == 0);
    printf("PASS: test_commit\n");
    rvm_destroy(rvm);
}

void test_truncate() {
    printf("--- Running test_truncate ---\n");
    rvm_t *rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    char *seg = rvm_get_segment(rvm);
    
    trans_t tid = rvm_begin_transaction(rvm);
    rvm_set_range(rvm, tid, seg, 20);
    strcpy(seg, "truncated_data");
    rvm_end_transaction(rvm, tid, false);
    
    rvm_truncate(rvm);
    rvm_destroy(rvm);
    
    // After truncate, the log is empty but backing file has data
    rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    seg = rvm_get_segment(rvm);
    assert(strcmp(seg, "truncated_data") == 0);
    printf("PASS: test_truncate\n");
    rvm_destroy(rvm);
}

void test_crash_recovery() {
    printf("--- Running test_crash_recovery ---\n");
    // Clean up
    unlink(LOG_FILE);
    unlink(BACKING_FILE);
    
    // Write something
    rvm_t *rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    char *seg = rvm_get_segment(rvm);
    trans_t tid = rvm_begin_transaction(rvm);
    rvm_set_range(rvm, tid, seg, 20);
    strcpy(seg, "before_crash");
    rvm_end_transaction(rvm, tid, false);
    rvm_destroy(rvm);

    pid_t pid = fork();
    if (pid == 0) {
        // Child: begin a tx, write data, but crash before committing!
        rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
        seg = rvm_get_segment(rvm);
        
        trans_t tid2 = rvm_begin_transaction(rvm);
        rvm_set_range(rvm, tid2, seg, 20);
        strcpy(seg, "CRASHED_DATA!!");
        
        // Crash immediately
        kill(getpid(), SIGKILL);
        exit(1);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    // Parent: recover
    rvm = rvm_init(LOG_FILE, BACKING_FILE, SEG_SIZE);
    seg = rvm_get_segment(rvm);
    assert(strcmp(seg, "before_crash") == 0); // The uncommitted data shouldn't be there
    printf("PASS: test_crash_recovery (uncommitted tx rolled back)\n");
    rvm_destroy(rvm);
}

int main() {
    unlink(LOG_FILE);
    unlink(BACKING_FILE);
    
    test_abort();
    test_commit();
    test_truncate();
    test_crash_recovery();
    
    unlink(LOG_FILE);
    unlink(BACKING_FILE);
    return 0;
}
