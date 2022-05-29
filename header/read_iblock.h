#ifndef _READING_BLOCKS_
#define _READING_BLOCKS_

//#include "ext2_recovery.h"
#include "ext_source.h"
#include "convert_to_LE.h"
#include "journal.h"

typedef unsigned int it; /*for inode type 32 bits num*/
#define IBLOCK_SIZE 1024


char *read_inode_iblock(struct ext2_inode inode,
                        int fd,
                        long int i_block_num,
                        struct ext2_super_block super,
                        __u32 block_size);

__u32 *read_inode_iblock_u32(struct ext2_inode inode,
                             int fd,
                             long int i_block_num,
                             struct ext2_super_block super,
                             __u32 block_size);

int read_inode_blocks(struct ext2_inode inode,
                      int fd,
                      struct struct_ext2_filsys filsys,
                      char file_name[]);

#endif //_READING_BLOCKS_