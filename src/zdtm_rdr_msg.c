#include "zdtm_rdr_msg.h"
#include "zdtm_common.h"

const char *RDR_MSG_TYPE = "RDR";

int zdtm_rdr_length(struct zdtm_rdr_msg_content *rdr){
    /* Because there is a char early in the struct, it is being 
     * null by the compiler to get the other data types memory
     * aligned, therefore, we are going to size the individual 
     * components, rather than using gcc-isms to get the data
     * structure packed.
     * The alternative is to add a __attribute__((__packed__))
     * between the closing brace and the semicolon on the 
     * structure declaration.
     * TODO Do we need to do this on the other structures
     *      for cross-platform issues?
     */
    
    return sizeof(rdr->sync_type) +
           sizeof(rdr->num_sync_ids) +
           sizeof(rdr->sync_id);
}

void *zdtm_rdr_write(void *buf, struct zdtm_rdr_msg_content *rdr){
    *((unsigned char*)buf++) = rdr->sync_type;

#ifdef WORDS_BIGENDIAN
    *((uint16_t*)buf) = zdtm_liltobigs(rdr->num_sync_ids);
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = zdtm_liltobigs(rdr->sync_id);
    buf += sizeof(uint32_t);

#else
    *((uint16_t*)buf) = rdr->num_sync_ids;
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = rdr->sync_id;
    buf += sizeof(uint32_t);

#endif
    return buf;
}

