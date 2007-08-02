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

#include "zdtm_common.h"

/**
 * Convert little-endian short to a big-endian short.
 *
 * The zdtm_liltobigs function converts a little-endian uint16_t
 * to a big-endian uint16_t.
 * @param lilshort The lil-end uint16_t to convert to big-end uint16_t.
 * @return The big-endian version of given little-endian uint16_t.
 */
uint16_t zdtm_liltobigs(uint16_t lilshort) {
    int size, i;
    unsigned char buff[sizeof(uint16_t)];

    size = sizeof(uint16_t);

    for (i = 0; i < size; i++) {
        buff[i] = ((unsigned char *)&lilshort)[(size - i - 1)];
    }

    return *(uint16_t *)buff;
}

/**
 * Convert little-endian long to a big-endian long.
 *
 * The zdtm_liltobigl function converts a little-endian uint32_t
 * to a big-endian uint32_t.
 * @param lillong The lil-end uint32_t to convert to big-end uint32_t.
 * @return The big-endian version of given little-endian uint32_t.
 */
uint32_t zdtm_liltobigl(uint32_t lillong) {
    int size, i;
    unsigned char buff[sizeof(uint32_t)];

    size = sizeof(uint32_t);
    
    for (i = 0; i < size; i++) {
        buff[i] = ((unsigned char *)&lillong)[(size - i - 1)];
    }

    return *(uint32_t *)buff;
}

/** 
 * Convert big-endian short to a little-endian short.
 *
 * The zdtm_bigtolils function converts a big-endian uint16_t to a
 * little-endian uint16_t.
 * @param bigshort The big-end uint16_t to convert to lil-end uint16_t.
 * @return The little-endian version of given big-endian uint16_t.
 */ 
uint16_t zdtm_bigtolils(uint16_t bigshort) {
    return zdtm_liltobigs(bigshort);
}

/**
 * Convert big-endian long to a little-endian long.
 *
 * The zdtm_bigtolils function converts a big-endian uint32_t to a
 * little-endian uint32_t.
 * @param biglong The big-end uint32_t to convert to lil-end uint32_t.
 * @return The little-endian version of given big-endian uint32_t.
 */
uint32_t zdtm_bigtolill(uint32_t biglong) {
    return zdtm_liltobigl(biglong);
}

/**
 * Calculate the length of a todo entry in the packet.
 *
 * @param todo a filled in todo struct
 * @return the number of bytes necessary in the packed packet.
 */
int zdtm_todo_length(struct zdtm_todo_item *todo){
    return sizeof(uint32_t) + todo->category_len +
           sizeof(uint32_t) + sizeof(todo->start_date) +
           sizeof(uint32_t) + sizeof(todo->due_date) +
           sizeof(uint32_t) + sizeof(todo->completed_date) +
           sizeof(uint32_t) + sizeof(todo->progress) +
           sizeof(uint32_t) + sizeof(todo->priority) +
           sizeof(uint32_t) + todo->description_len +
           sizeof(uint32_t) + todo->notes_len;
}

/**
 * Copies the contents of a zdtm_todo struct into a packet buffer
 * and returns a pointer just past the end of the data.
 */
void* zdtm_todo_write(void *buf, struct zdtm_todo_item *todo){
#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(todo->category_len);            
#else
    *((uint32_t*)buf) = sizeof(todo->category_len);
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, todo->category, todo->category_len);
    buf += todo->category_len;

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(sizeof(todo->start_date));            
#else
    *((uint32_t*)buf) = sizeof(todo->start_date); 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, todo->start_date, sizeof(todo->start_date));
    buf += sizeof(todo->start_date);

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(sizeof(todo->due_date));            
#else
    *((uint32_t*)buf) = sizeof(todo->due_date); 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, todo->due_date, sizeof(todo->due_date));
    buf += sizeof(todo->due_date);

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(sizeof(todo->completed_date));
#else
    *((uint32_t*)buf) = sizeof(todo->completed_date); 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, todo->completed_date, sizeof(todo->completed_date));
    buf += sizeof(todo->completed_date);

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(sizeof(todo->progress));
#else
    *((uint32_t*)buf) = sizeof(todo->progress); 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, &todo->progress, sizeof(todo->progress));
    buf += sizeof(todo->progress);

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(sizeof(todo->priority));
#else
    *((uint32_t*)buf) = sizeof(todo->priority); 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, &todo->priority, sizeof(todo->priority));
    buf += sizeof(todo->priority);

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(todo->description_len);
#else
    *((uint32_t*)buf) = todo->description_len; 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, todo->description, sizeof(todo->description_len));
    buf += sizeof(todo->description_len);

#ifdef WORDS_BIGENDIAN
    *((uint32_t*)buf) = zdtm_liltobigl(todo->notes_len);
#else
    *((uint32_t*)buf) = todo->notes_len; 
#endif
    buf += sizeof(uint32_t);

    memcpy(buf, todo->notes, sizeof(todo->notes_len));
    buf += sizeof(todo->notes_len);

    return buf;
}

int zdtm_calendar_length(struct zdtm_calendar_item *calendar) {
    return sizeof(uint32_t) + calendar->category_len +
           sizeof(uint32_t) + calendar->description_len +
           sizeof(uint32_t) + calendar->location_len +
           sizeof(uint32_t) + calendar->notes_len +
           sizeof(uint32_t) + sizeof(calendar->start_time) +
           sizeof(uint32_t) + sizeof(calendar->end_time) +
           sizeof(uint32_t) + sizeof(calendar->schedule_type) +
           sizeof(uint32_t) + sizeof(calendar->alarm) +
           sizeof(uint32_t) + sizeof(calendar->alarm_setting) +
           sizeof(uint32_t) + sizeof(calendar->repeat_type) +
           sizeof(uint32_t) + sizeof(calendar->repeat_period) +
           sizeof(uint32_t) + sizeof(calendar->repeat_position) +
           sizeof(uint32_t) + sizeof(calendar->repeat_date) +
           sizeof(uint32_t) + sizeof(calendar->repeat_end_date_setting) +
           sizeof(uint32_t) + sizeof(calendar->repeat_end_date) +
           sizeof(uint32_t) + sizeof(calendar->all_day_start_date) +
           sizeof(uint32_t) + sizeof(calendar->all_day_end_date) +
           sizeof(uint32_t) + sizeof(calendar->multiple_days_flag);
}

int zdtm_address_length(struct zdtm_address_item *address) {
    return sizeof(uint32_t) + address->category_len +
           sizeof(uint32_t) + address->full_name_len +
           sizeof(uint32_t) + address->full_name_pronun_len +
           sizeof(uint32_t) + address->title_len +
           sizeof(uint32_t) + address->last_name_len +
           sizeof(uint32_t) + address->first_name_len +
           sizeof(uint32_t) + address->middle_name_len +
           sizeof(uint32_t) + address->suffix_len +
           sizeof(uint32_t) + address->alternative_name_len +
           sizeof(uint32_t) + address->last_name_pronun_len +
           sizeof(uint32_t) + address->first_name_pronun_len +
           sizeof(uint32_t) + address->company_len +
           sizeof(uint32_t) + address->department_len +
           sizeof(uint32_t) + address->job_title_len +
           sizeof(uint32_t) + address->work_phone_len +
           sizeof(uint32_t) + address->work_fax_len +
           sizeof(uint32_t) + address->work_mobile_len +
           sizeof(uint32_t) + address->work_state_len +
           sizeof(uint32_t) + address->work_city_len +
           sizeof(uint32_t) + address->work_street_len +
           sizeof(uint32_t) + address->work_zip_len +
           sizeof(uint32_t) + address->work_country_len +
           sizeof(uint32_t) + address->work_web_page_len +
           sizeof(uint32_t) + address->office_len +
           sizeof(uint32_t) + address->profession_len +
           sizeof(uint32_t) + address->assistant_len +
           sizeof(uint32_t) + address->manager_len +
           sizeof(uint32_t) + address->pager_len +
           sizeof(uint32_t) + address->cellular_len +
           sizeof(uint32_t) + address->home_phone_len +
           sizeof(uint32_t) + address->home_fax_len +
           sizeof(uint32_t) + address->home_state_len +
           sizeof(uint32_t) + address->home_city_len +
           sizeof(uint32_t) + address->home_street_len +
           sizeof(uint32_t) + address->home_zip_len +
           sizeof(uint32_t) + address->home_country_len +
           sizeof(uint32_t) + address->home_web_page_len +
           sizeof(uint32_t) + address->default_email_len +
           sizeof(uint32_t) + address->emails_len +
           sizeof(uint32_t) + address->spouse_len +
           sizeof(uint32_t) + address->gender_len +
           sizeof(uint32_t) + address->birthday_len +
           sizeof(uint32_t) + address->anniversary_len +
           sizeof(uint32_t) + address->nickname_len +
           sizeof(uint32_t) + address->children_len +
           sizeof(uint32_t) + address->memo_len +
           sizeof(uint32_t) + address->group_len;

}
