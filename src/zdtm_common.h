
#ifndef ZDTM_COMMON_H
#define ZDTM_COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "config.h"

uint16_t zdtm_liltobigs(uint16_t lilshort);
uint32_t zdtm_liltobigl(uint32_t lillong);
uint16_t zdtm_bigtolils(uint16_t bigshort);
uint32_t zdtm_bigtolill(uint32_t biglong);

#endif
