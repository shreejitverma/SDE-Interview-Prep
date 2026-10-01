#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct rvm_t rvm_t;
typedef int trans_t;

// Initialize RVM segment. Recovers from log if it exists.
rvm_t* rvm_init(const char *log_file, const char *backing_file, size_t size);

// Begin a new transaction
trans_t rvm_begin_transaction(rvm_t *rvm);

// Mark a range of memory to be modified, creating an undo record
void rvm_set_range(rvm_t *rvm, trans_t tid, void *addr, size_t size);

// End transaction. If no_flush is false, syncs the redo log to disk
void rvm_end_transaction(rvm_t *rvm, trans_t tid, bool no_flush);

// Abort transaction, restoring undo records
void rvm_abort_transaction(rvm_t *rvm, trans_t tid);

// Truncate the log by applying its contents to the backing file
void rvm_truncate(rvm_t *rvm);

// Clean up and free resources
void rvm_destroy(rvm_t *rvm);

// Get the pointer to the base of the recoverable segment
void* rvm_get_segment(rvm_t *rvm);
