#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>



#define REQ_EXPORT_TYPE  0x00
#define REQ_1_TYPE       0x01
#define REQ_2_TYPE       0x02
#define REQ_3_TYPE       0x03
#define REQ_UNKNOWN_TYPE 0xff

typedef struct {
    uint8_t reqest_type;
} header_t;



typedef struct {
    uint64_t start_value;
    uint64_t step;
} body_t;



typedef struct {
    header_t header;
    body_t body;
} reqest_t;



typedef struct {
    uint8_t is_end;
    uint64_t number;
} answer_t;

#endif // __PROTOCOL_H
