#ifndef ZDTM_COMMON_H
#define ZDTM_COMMON_H

#include "zdtm_gentypes.h"
#include "zdtm_config.h"

// Return values
#define RET_NNULL_RAW     -7
#define RET_SIZE_MISMATCH -8
#define RET_MEM_CONTENT   -9
#define RET_UNK_TYPE      -10
#define RET_BAD_SIZE      -11
#define RET_UNK_VAR       -12
#define RET_MALLOC_FAIL   -13
#define RET_PARSE_RAW_FAIL -14

// Sync Types
#define SYNC_TYPE_CALENDAR  0x01
#define SYNC_TYPE_TODO      0x06
#define SYNC_TYPE_ADDRESS   0x07

uint16_t zdtm_liltobigs(uint16_t lilshort);
uint32_t zdtm_liltobigl(uint32_t lillong);
uint16_t zdtm_bigtolils(uint16_t bigshort);
uint32_t zdtm_bigtolill(uint32_t biglong);

/**
 * zdtm_todo contains the information for a todo message. 
 */ 
struct zdtm_todo{
    uint32_t category_len;
    char *category;
    char start_date[5];
    char due_date[5];
    char completed_date[5];
    char progress;
    char priority;
    uint32_t description_len;
    char *description;
    uint32_t notes_len;
    char *notes;
};

inline int zdtm_todo_length(struct zdtm_todo * todo);
inline void *zdtm_todo_write(void *buf, struct zdtm_todo *todo);


#endif
