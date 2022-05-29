#ifndef __PARSER__
#define __PARSER__

#include "ext_source.h"


void dir_checker(char *argv);

__u32 dir_parser(char *dir, struct struct_ext2_filsys filsys, int fd);

#endif //__PARSER__