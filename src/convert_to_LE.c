#include "../header/convert_to_LE.h"

__u32 convert_from_num(__u32 number){

    u32Format_big_endian_t num;
    num.d_word= number; 
    __u32 ret = 0; 

    return ret = (__u32)num.buff[3] | (__u32)num.buff[2] << 8 |  (__u32)num.buff[1] << 16 | (__u32)num.buff[0] << 24 ;
}
