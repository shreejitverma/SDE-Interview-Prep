#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PATH 1024

static char backing_file[MAX_PATH];
static char cache_buf[4096];
static size_t cache_size = 0;
static int dirty = 0;

static int wb_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (strcmp(path, "/test.txt") == 0) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = cache_size;
        return 0;
    }

    return -ENOENT;
}

static int wb_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    (void) offset; (void) fi; (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, "test.txt", NULL, 0, 0);

    return 0;
}

static int wb_open(const char *path, struct fuse_file_info *fi) {
    (void) fi;
    if (strcmp(path, "/test.txt") != 0) return -ENOENT;

    if (cache_size == 0 && !dirty) {
        FILE *f = fopen(backing_file, "rb");
        if (f) {
            cache_size = fread(cache_buf, 1, sizeof(cache_buf), f);
            fclose(f);
        }
    }
    return 0;
}

static int wb_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    if (strcmp(path, "/test.txt") != 0) return -ENOENT;

    if ((size_t)offset >= cache_size) return 0;
    if (offset + size > cache_size) size = cache_size - offset;

    memcpy(buf, cache_buf + offset, size);
    return size;
}

static int wb_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    if (strcmp(path, "/test.txt") != 0) return -ENOENT;

    if (offset + size > sizeof(cache_buf)) size = sizeof(cache_buf) - offset;

    memcpy(cache_buf + offset, buf, size);
    if (offset + size > cache_size) cache_size = offset + size;

    dirty = 1;
    printf("[CACHE] Buffered %zu bytes\n", size);
    fflush(stdout);
    return size;
}

static int wb_flush(const char *path, struct fuse_file_info *fi) {
    (void) fi;
    if (strcmp(path, "/test.txt") != 0) return 0;
    
    if (dirty) {
        FILE *f = fopen(backing_file, "wb");
        if (f) {
            fwrite(cache_buf, 1, cache_size, f);
            fclose(f);
            printf("[CACHE] Flushed %zu bytes to backing store\n", cache_size);
            fflush(stdout);
            dirty = 0;
        }
    }
    return 0;
}

static int wb_release(const char *path, struct fuse_file_info *fi) {
    wb_flush(path, fi);
    return 0;
}

static int wb_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
    (void) fi;
    if (strcmp(path, "/test.txt") != 0) return -ENOENT;
    if ((size_t)size <= sizeof(cache_buf)) {
        cache_size = size;
        dirty = 1;
    }
    return 0;
}

static struct fuse_operations wb_oper = {
    .getattr    = wb_getattr,
    .readdir    = wb_readdir,
    .open       = wb_open,
    .read       = wb_read,
    .write      = wb_write,
    .flush      = wb_flush,
    .release    = wb_release,
    .truncate   = wb_truncate,
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <backing_file> <mount_point>\n", argv[0]);
        return 1;
    }
    
    if (argv[1][0] != '/') {
        char cwd[MAX_PATH / 2];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            snprintf(backing_file, MAX_PATH, "%s/%s", cwd, argv[1]);
        } else {
            snprintf(backing_file, MAX_PATH, "%s", argv[1]);
        }
    } else {
        snprintf(backing_file, MAX_PATH, "%s", argv[1]);
    }
    
    char *fuse_argv[4];
    fuse_argv[0] = argv[0];
    fuse_argv[1] = argv[2]; // mount point
    fuse_argv[2] = "-f"; // foreground
    fuse_argv[3] = NULL;
    
    printf("Mounting Write-Back Cache on %s backed by %s\n", argv[2], argv[1]);
    fflush(stdout);
    return fuse_main(3, fuse_argv, &wb_oper, NULL);
}
