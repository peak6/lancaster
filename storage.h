/* persistent, memory-mapped record storage and locking */

#ifndef STORAGE_H
#define STORAGE_H

#include "clock.h"
#include "spin.h"
#include "status.h"
#include <limits.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct storage;
typedef struct storage* storage_handle;

struct record;
typedef struct record* record_handle;

typedef status (*storage_iterate_func)(storage_handle, record_handle, void*);

typedef long identifier;
typedef long q_index;
typedef spin_lock revision;

struct q_element { identifier id; microsec ts; };

#define NEXT_REV(v) (((v) + 1) & SPIN_MAX)

status storage_create(storage_handle* pstore, const char* mmap_file,
					  boolean persist, int open_flags, identifier base_id,
					  identifier max_id, size_t value_size, size_t property_size,
					  size_t q_capacity);
status storage_open(storage_handle* pstore, const char* mmap_file,
					int open_flags);
status storage_destroy(storage_handle* pstore);

boolean storage_is_read_only(storage_handle store);
status storage_set_persistence(storage_handle store, boolean persist);

unsigned short storage_get_file_version(storage_handle store);
unsigned short storage_get_data_version(storage_handle store);
status storage_set_data_version(storage_handle store, unsigned short data_ver);

record_handle storage_get_array(storage_handle store);
identifier storage_get_base_id(storage_handle store);
identifier storage_get_max_id(storage_handle store);
size_t storage_get_record_size(storage_handle store);
size_t storage_get_property_size(storage_handle store);
size_t storage_get_value_size(storage_handle store);
size_t storage_get_value_offset(storage_handle store);

const char* storage_get_file(storage_handle store);
const char* storage_get_description(storage_handle store);
status storage_set_description(storage_handle store, const char* desc);

status storage_get_created_time(storage_handle store, microsec* when);
status storage_get_touched_time(storage_handle store, microsec* when);
status storage_touch(storage_handle store, microsec when);

const struct q_element* storage_get_queue_base_ref(storage_handle store);
const q_index* storage_get_queue_head_ref(storage_handle store);
size_t storage_get_queue_capacity(storage_handle store);
q_index storage_get_queue_head(storage_handle store);
status storage_write_queue(storage_handle store, identifier id);
status storage_read_queue(storage_handle store, q_index idx,
						  struct q_element* pelem, boolean update_stats);

status storage_get_id(storage_handle store, record_handle rec,
					  identifier* pident);
status storage_get_record(storage_handle store, identifier id,
						  record_handle* prec);

status storage_iterate(storage_handle store, storage_iterate_func iter_fn,
					   record_handle prev, void* param);
status storage_sync(storage_handle store);
status storage_reset(storage_handle store);

status storage_grow(storage_handle store, storage_handle* pnewstore,
					const char* new_mmap_file, identifier new_base_id,
					identifier new_max_id, size_t new_value_size,
					size_t new_property_size, size_t new_q_capacity);

void* storage_get_property_ref(storage_handle store, record_handle rec);
void* record_get_value_ref(record_handle rec);

revision record_get_revision(record_handle rec);
void record_set_revision(record_handle rec, revision rev);
status record_read_lock(record_handle rec, revision* old_rev);
status record_write_lock(record_handle rec, revision* old_rev);

double storage_get_queue_min_latency(storage_handle store);
double storage_get_queue_max_latency(storage_handle store);
double storage_get_queue_mean_latency(storage_handle store);
double storage_get_queue_stddev_latency(storage_handle store);

status storage_next_stats(storage_handle store);

#ifdef __cplusplus
}
#endif

#endif
