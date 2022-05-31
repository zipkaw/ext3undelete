#ifndef __EXT_SOURCE__
#define __EXT_SOURCE__
#define _LARGEFILE64_SOURCE
#include <ext2fs/ext2fs.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
// offset to superblock
#define BASE_OFFSET 1024


// calculating group block
#define GROUP_INDEX (inode_num, inodes_per_group)(return ((inode_num - 1) / inodes_per_group))

// calculating inode index from INODE TABLES
#define INODE_INDEX (inode_num, inodes_per_group)(return ((inode_num - 1) % inodes_per_group))


//#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)

typedef struct inode_d_time
{
    unsigned int inode;
    unsigned int d_time;
} inode_d_time_t;


int open_device(char *name);

void close_device(int fd);

unsigned int get_block_size(char *name);

int extXdetector(struct ext2_super_block* super);

struct ext2_group_desc read_gd(struct struct_ext2_filsys filsys, int fd, __u32 group);

#endif //__EXT_SOURCE__