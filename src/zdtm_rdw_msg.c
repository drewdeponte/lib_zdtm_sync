#include "zdtm_rdw_msg.h"

const char *RDW_MSG_TYPE = "RDW";

int zdtm_rdw_length(struct zdtm_rdw_msg_content *rdw){
    int size = 0;

    /* This one is tricky. */
    size += sizeof(rdw->sync_type);
    size += sizeof(rdw->num_sync_ids);
    size += sizeof(rdw->sync_id);

    switch(rdw->variation){
        case 1:
            size += sizeof(rdw->vars.one.padding);

            switch(rdw->sync_type){
                //case SYNC_TYPE_CALENDAR:
                case SYNC_TYPE_TODO:
                    size += zdtm_todo_length(&rdw->cont.todo);
                    break;
                //case SYNC_TYPE_ADDRESS:
                default:
                    return RET_UNK_TYPE;
                    break;
            }

            break;
            
        case 2:
            size += sizeof(uint32_t) + sizeof(rdw->vars.two.attribute);
            break;

        case 3:
            size += 
                sizeof(uint32_t) +
                sizeof(rdw->vars.three.attribute) +
                sizeof(uint32_t) +
                sizeof(rdw->vars.three.card_created_date_time) +
                sizeof(uint32_t) +
                sizeof(rdw->vars.three.card_mod_date_time) +
                sizeof(uint32_t) +
                sizeof(rdw->vars.three.sync_id);


            switch(rdw->sync_type){
                //case SYNC_TYPE_CALENDAR:
                case SYNC_TYPE_TODO:
                    size += zdtm_todo_length(&rdw->cont.todo);
                    break;
                //case SYNC_TYPE_ADDRESS:
                default:
                    return RET_UNK_TYPE;
                    break;
            }
            break;
        default:
            return RET_UNK_VAR;
            break;
    }

    return size;
}

void *zdtm_rdw_write(void *buf, struct zdtm_rdw_msg_content *rdw){
    *((unsigned char*)buf++) = rdw->sync_type;
    
#ifdef WORDS_BIGENDIAN
    *((uint16_t*)buf) = zdtm_liltobigs(rdw->num_sync_ids);
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = zdtm_liltobigs(rdw->sync_id);
    buf += sizeof(uint32_t);

#else
    *((uint16_t*)buf) = rdw->num_sync_ids;
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = rdw->sync_id;
    buf += sizeof(uint32_t);

#endif

    switch(rdw->variation){
        case 1:
            memcpy(buf, rdw->vars.one.padding, 
                   sizeof(rdw->vars.one.padding));
            buf += sizeof(rdw->vars.one.padding);

            switch(rdw->sync_type){
                //case SYNC_TYPE_CALENDAR:
                case SYNC_TYPE_TODO:
                    buf = zdtm_todo_write(buf, &rdw->cont.todo);
                    break;
                //case SYNC_TYPE_ADDRESS:
                default:
                    return NULL;
                    break;
            }
            break;

        case 2:
#ifdef WORDS_BIGENDIAN
            *((uint32_t*)buf) = zdtm_liltobigl(sizeof(rdw->vars.two.attribute));
#else
            *((uint32_t*)buf) = sizeof(rdw->vars.two.attribute);
#endif
            buf += sizeof(uint32_t);

            memcpy(buf, &rdw->vars.two.attribute,
                   sizeof(rdw->vars.two.attribute));

            buf += sizeof(rdw->vars.two.attribute);
            break;

        case 3:
#ifdef WORDS_BIGENDIAN
            *((uint32_t*)buf) = zdtm_liltobigl(
                                    sizeof(rdw->vars.three.attribute));
#else
            *((uint32_t*)buf) = sizeof(rdw->vars.three.attribute);
#endif
            buf += sizeof(uint32_t);

            memcpy(buf, &rdw->vars.three.attribute,
                   sizeof(rdw->vars.three.attribute));

            buf += sizeof(rdw->vars.three.attribute);

#ifdef WORDS_BIGENDIAN
            *((uint32_t*)buf) = zdtm_liltobigl(
                                sizeof(rdw->vars.three.card_created_date_time));
#else
            *((uint32_t*)buf) = sizeof(rdw->vars.three.card_created_date_time);
#endif
            buf += sizeof(uint32_t);

            memcpy(buf, rdw->vars.three.card_created_date_time, 
                    sizeof(rdw->vars.three.card_created_date_time));

            buf += sizeof(rdw->vars.three.card_created_date_time);

#ifdef WORDS_BIGENDIAN
            *((uint32_t*)buf) = zdtm_liltobigl(
                                    sizeof(rdw->vars.three.card_mod_date_time));
#else
            *((uint32_t*)buf) = sizeof(rdw->vars.three.card_mod_date_time);
#endif
            buf += sizeof(uint32_t);

            memcpy(buf, rdw->vars.three.card_mod_date_time, 
                    sizeof(rdw->vars.three.card_created_date_time));
            buf += sizeof(rdw->vars.three.card_mod_date_time);

#ifdef WORDS_BIGENDIAN
            *((uint32_t*)buf) = zdtm_liltobigl(sizeof(rdw->vars.three.sync_id));
#else
            *((uint32_t*)buf) = sizeof(rdw->vars.three.sync_id);
#endif
            buf += sizeof(uint32_t);

            memcpy(buf, &rdw->vars.three.sync_id, 
                        sizeof(rdw->vars.three.sync_id));
            buf += sizeof(rdw->vars.three.sync_id);

            switch(rdw->sync_type){
                //case SYNC_TYPE_CALENDAR:
                case SYNC_TYPE_TODO:
                    buf = zdtm_todo_write(buf, &rdw->cont.todo);
                    break;
                //case SYNC_TYPE_ADDRESS:
                default:
                    return NULL;
                    break;
            }
            break;

        default:
            break;
    }

    return buf;
}
