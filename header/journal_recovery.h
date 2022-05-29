#ifndef __JPARSER__
#define __JPARSER__


#include "ext_source.h"

#include "read_iblock.h"
#include "journal.h"
/*struct for recovery*/
typedef struct deleted_file_s
{
    char file_name[EXT2_NAME_LEN];
    __u32 seq;
    __u32 inode_num;
} deleted_file_t;


int check_latest_deleted(deleted_file_t *deleted,
                         deleted_file_t deleted_to_check,
                         __u32 deleted_file_c);

deleted_file_t* scan_dir(char *dir,
             struct struct_ext2_filsys filsys,
             deleted_file_t *deleted,
             __u32 *deleted_file_c,
             __u32 curr_seq);

void inode_reader(char *buff, 
                __u32 index_in_table, 
                deleted_file_t deleted, 
                int fd,
                struct struct_ext2_filsys filsys
                );

void inode_recover(int fd,
                   struct struct_ext2_filsys filsys,
                   deleted_file_t *deleted_files,
                   __u32 deleted_files_c);

void journal_parser(int fd,
                    struct struct_ext2_filsys filsys,
                    __u32 dirnum);

#endif