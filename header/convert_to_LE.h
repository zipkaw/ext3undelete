#ifndef _CONVERT_
#define _CONVERT_

#include <ext2fs/ext2_types.h>

typedef union u32Format_big_endian
{
    unsigned char buff[4];
    __u32 d_word;
}u32Format_big_endian_t;

__u32 convert_from_num(__u32 number);


#endif //_CONVERT_