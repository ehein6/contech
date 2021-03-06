#ifndef CT_EVENT_ST_H
#define CT_EVENT_ST_H

#include <stdio.h>
#include <stdbool.h>

//#if 0
//#ifndef __cplusplus
//typedef char bool;
//#define false 0
//#define true 1
//#endif
//#endif

#define CONTECH_EVENT_VERSION 8

typedef unsigned long long ct_tsc_t;
typedef unsigned long long ct_addr_t;

typedef struct _ct_memory_op {
  union {
    struct {
        // NB. Addr must come first so that the storing to data32[0] is completely stored into
        //   the address field.
        unsigned long long addr : 50;
        unsigned long long rank : 8;
        unsigned long long is_write : 1;
        unsigned long long pow_size : 3; // the size of the op is 2^pow_size
    };
    unsigned long long data;
    unsigned int data32[2];
  };
} ct_memory_op, *pct_memory_op;

// Event IDs used primarily in serialize / deserialize
enum _ct_event_id { ct_event_basic_block = 0, 
    ct_event_basic_block_info = 128, 
    ct_event_memory, 
    ct_event_sync, 
    ct_event_barrier, 
    ct_event_task_create, 
    ct_event_task_join, 
    ct_event_buffer,  // INTERNAL USE
    ct_event_bulk_memory_op,
    ct_event_version, // INTERNAL USE
    ct_event_delay,
    ct_event_rank,
    ct_event_mpi_transfer,
    ct_event_mpi_wait,
    ct_event_roi,
    ct_event_unknown};
typedef enum _ct_event_id ct_event_id;

enum _ct_sync_type {
    ct_sync_release = 0,
    ct_sync_acquire = 1,
    ct_cond_wait,
    ct_cond_sig,
    ct_sync_atomic,
    ct_task_depend,
    ct_sync_unknown};
typedef enum _ct_sync_type ct_sync_type;

#define BBI_FLAG_CONTAIN_CALL 0x1

#define BBI_FLAG_MEM_DUP 0x2

//
// Maps basic_block_ids to string identifiers
//
typedef struct _ct_basic_block_info
{
    unsigned int basic_block_id, fun_name_len, file_name_len, callFun_name_len, num_mem_ops, line_num, num_ops, crit_path_len;
    unsigned int flags;
    char* file_name;
    char* fun_name;
    char* callFun_name;
} ct_basic_block_info, *pct_basic_block_info;

typedef struct _ct_basic_block
{
    unsigned int basic_block_id, len;
    pct_memory_op mem_op_array;
} ct_basic_block, *pct_basic_block;

typedef struct _ct_memory
{
    bool isAllocate;
    unsigned long long size;  // Only has meaning if isAllocate = true
    ct_addr_t alloc_addr;
} ct_memory, *pct_memory;

typedef struct _ct_sync
{
    int sync_type;
    ct_tsc_t start_time;
    ct_tsc_t end_time;
    ct_addr_t sync_addr;
    unsigned long long ticketNum;
} ct_sync, *pct_sync;

typedef struct _ct_barrier
{
    bool onEnter;
    ct_tsc_t start_time;
    ct_tsc_t end_time;
    ct_addr_t sync_addr;
    unsigned long long barrierNum;
} ct_barrier, *pct_barrier;

typedef struct _ct_task_create
{
    unsigned int other_id; // child if id is creator, parent if id is created
    ct_tsc_t start_time;
    ct_tsc_t end_time;
    long long approx_skew; // 0 if parent, value if child, middle layer uses this as a flag
} ct_task_create, *pct_task_create;

typedef struct _ct_task_join
{
    bool isExit;
    ct_tsc_t start_time;
    ct_tsc_t end_time;
    unsigned int other_id; // self if calling exit, child if calling join
} ct_task_join, *pct_task_join;

typedef struct _ct_buffer_info
{
    unsigned int pos;
} ct_buffer_info, *pct_buffer_info;

typedef struct _ct_bulk_memory
{
    unsigned long long size;
    ct_addr_t dst_addr;
    ct_addr_t src_addr;
} ct_bulk_memory, *pct_bulk_memory;

typedef struct _ct_delay
{
    ct_tsc_t start_time;
    ct_tsc_t end_time;
} ct_delay, *pct_delay;

typedef struct _ct_rank
{
    unsigned int rank;
} ct_rank, *pct_rank;

typedef struct _ct_mpi_transfer
{
    bool isSend, isBlocking;
    int comm_rank;
    int tag;
    ct_addr_t buf_ptr;
    size_t buf_size;
    ct_tsc_t start_time;
    ct_tsc_t end_time;
    ct_addr_t req_ptr; // Used when the request is non-blocking
} ct_mpi_transfer, *pct_mpi_transfer;

typedef struct _ct_mpi_wait
{
    ct_addr_t req_ptr;
    ct_tsc_t start_time;
    ct_tsc_t end_time;
} ct_mpi_wait, *pct_mpi_wait;

typedef struct _ct_roi_event
{
    ct_tsc_t start_time;
} ct_roi_event, *pct_roi_event;

//
// There are two ways to combine objects with common fields.
//   1) Common fields in a single type that is the first field
//       in the other structs
//   2) Union block of the other structs
//
//  Should a basic block contain an event header or should events be basic blocks?
//
typedef struct _ct_event {
    unsigned int contech_id;
    ct_event_id event_type;
    
    union {
        ct_basic_block      bb;
        ct_basic_block_info bbi;
        ct_barrier          bar;
        ct_sync             sy;
        ct_task_create      tc;
        ct_task_join        tj;
        ct_memory           mem;
        ct_buffer_info      buf;
        ct_bulk_memory      bm;
        ct_delay            dly;
        ct_rank             rank;
        ct_mpi_transfer     mpixf;
        ct_mpi_wait         mpiw;
        ct_roi_event        roi;
    };
} ct_event, *pct_event;

#endif
