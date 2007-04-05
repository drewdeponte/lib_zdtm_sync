/*
 * Copyright 2005-2007 Andrew De Ponte
 * 
 * This file is part of lib_zdtm_sync.
 * 
 * lib_zdtm_sync is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or any
 * later version.
 * 
 * lib_zdtm_sync is distributed in the hopes that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with lib_zdtm_sync; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

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

// Data types
#define DATA_ID_TIME 0x04
#define DATA_ID_BIT 0x06
#define DATA_ID_WORD 0x08
#define DATA_ID_UCHAR 0x0b
#define DATA_ID_BARRAY 0x0c
#define DATA_ID_UTF8 0x11
#define DATA_ID_ULONG 0x12

uint16_t zdtm_liltobigs(uint16_t lilshort);
uint32_t zdtm_liltobigl(uint32_t lillong);
uint16_t zdtm_bigtolils(uint16_t bigshort);
uint32_t zdtm_bigtolill(uint32_t biglong);

/**
 * zdtm_todo contains the information for a todo message. 
 */ 
struct zdtm_todo_item {
    unsigned char attribute;
    char creation_date[5];
    char modification_date[5];
    uint32_t sync_id;

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

struct zdtm_calendar_item {
    unsigned char attribute;
    char creation_date[5];
    char modification_date[5];
    uint32_t sync_id;

    uint32_t category_len;
    char *category;
    uint32_t description_len;
    char *description;
    uint32_t location_len;
    char *location;
    uint32_t notes_len;
    char *notes;
    
    char start_time[5];
    char end_time[5];
    unsigned char schedule_type;
    unsigned char alarm;
    unsigned char alarm_setting;
    uint16_t alarm_time;
    unsigned char repeat_type;
    uint16_t repeat_period;
    uint16_t repeat_position;
    unsigned char repeat_date;
    unsigned char repeat_end_date_setting;
    char repeat_end_date[5];
    char all_day_start_date[5];
    char all_day_end_date[5];
    unsigned char multiple_days_flag;
};

struct zdtm_address_item {
    unsigned char attribute;
    char creation_date[5];
    char modification_date[5];
    uint32_t sync_id;
    
    uint32_t category_len;
    char *category;
    uint32_t full_name_len;
    char *full_name;
    uint32_t full_name_pronun_len;
    char *full_name_pronun;
    uint32_t title_len;
    char *title;
    uint32_t last_name_len;
    char *last_name;
    uint32_t first_name_len;
    char *first_name;
    uint32_t middle_name_len;
    char *middle_name;
    uint32_t suffix_len;
    char *suffix;
    uint32_t alternative_name_len;
    char *alternative_name;
    uint32_t last_name_pronun_len;
    char *last_name_pronun;
    uint32_t first_name_pronun_len;
    char *first_name_pronun;
    uint32_t company_len;
    char *company;
    uint32_t company_pronun_len;
    char *company_pronun;
    uint32_t department_len;
    char *department;
    uint32_t job_title_len;
    char *job_title;
    uint32_t work_phone_len;
    char *work_phone;
    uint32_t work_fax_len;
    char *work_fax;
    uint32_t work_mobile_len;
    char *work_mobile;
    uint32_t work_state_len;
    char *work_state;
    uint32_t work_city_len;
    char *work_city;
    uint32_t work_street_len;
    char *work_street;
    uint32_t work_zip_len;
    char *work_zip;
    uint32_t work_country_len;
    char *work_country;
    uint32_t work_web_page_len;
    char *work_web_page;
    uint32_t office_len;
    char *office;
    uint32_t profession_len;
    char *profession;
    uint32_t assistant_len;
    char *assistant;
    uint32_t manager_len;
    char *manager;
    uint32_t pager_len;
    char *pager;
    uint32_t cellular_len;
    char *cellular;
    uint32_t home_phone_len;
    char *home_phone;
    uint32_t home_fax_len;
    char *home_fax;
    uint32_t home_state_len;
    char *home_state;
    uint32_t home_city_len;
    char *home_city;
    uint32_t home_street_len;
    char *home_street;
    uint32_t home_zip_len;
    char *home_zip;
    uint32_t home_country_len;
    char *home_country;
    uint32_t home_web_page_len;
    char *home_web_page;
    uint32_t default_email_len;
    char *default_email;
    uint32_t emails_len;
    char *emails;
    uint32_t spouse_len;
    char *spouse;
    uint32_t gender_len;
    char *gender;
    uint32_t birthday_len;
    char *birthday;
    uint32_t anniversary_len;
    char *anniversary;
    uint32_t nickname_len;
    char *nickname;
    uint32_t children_len;
    char *children;
    uint32_t memo_len;
    char *memo;
    uint32_t group_len;
    char *group;
};

inline int zdtm_todo_length(struct zdtm_todo_item * todo);
inline void *zdtm_todo_write(void *buf, struct zdtm_todo_item *todo);


#endif
