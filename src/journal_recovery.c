#include "../header/journal_recovery.h"

int check_latest_deleted(deleted_file_t *deleted,
                         deleted_file_t deleted_to_check,
                         __u32 deleted_file_c)
{
    int i = 0;
    while (i <= deleted_file_c)
    {
        if (deleted[i].inode_num == deleted_to_check.inode_num)
        {
            if (deleted[i].seq < deleted_to_check.seq)
            {
                deleted[i] = deleted_to_check;
                return 1;
            }
        }
        i++;
    }
    return 0;
}

deleted_file_t* scan_dir(char *dir,
             struct struct_ext2_filsys filsys,
             deleted_file_t *deleted,
             __u32 *deleted_file_c,
             __u32 curr_seq)
{
    struct ext2_dir_entry_2 dirent;
    __u16 reclen = 0;
    deleted_file_t deleted_file_temp;
    do
    {
        memset(&deleted_file_temp, 0, sizeof(deleted_file_t));
        memcpy(&dirent, dir + reclen, sizeof(struct ext2_dir_entry_2));
        reclen += dirent.rec_len;
        struct ext2_inode inode;
        ext2fs_read_inode(&filsys, dirent.inode, &inode);
        if (dirent.rec_len == 0)
        {
            break;
        }
        if (inode.i_dtime > 0)
        {
            /*return 0 if success*/
            deleted_file_temp.inode_num = dirent.inode;
            deleted_file_temp.seq = curr_seq;
            memcpy(&deleted_file_temp.file_name, dirent.name, dirent.name_len);
            if (!check_latest_deleted(deleted, deleted_file_temp, *deleted_file_c))
            {
                deleted[*deleted_file_c] = deleted_file_temp;
                (*deleted_file_c)++;
                deleted_file_t *realloc_temp = deleted;

                printf("Deleted file(seq:%u)\n"
                        "Name:%s\n"
                        "Inode:%u\n\n", 
                        deleted[*deleted_file_c - 1].seq, 
                        deleted[*deleted_file_c-1].file_name, 
                        deleted[*deleted_file_c - 1].inode_num); 

                if ((realloc_temp = (deleted_file_t *)realloc(deleted, sizeof(deleted_file_t) * (*deleted_file_c + 1))) == NULL)
                {
                    fprintf(stderr, "BAD REALLOC with reallocating.\n"
                                    "Count of preparing for recovery notes: %d\n",
                            *deleted_file_c);
                }
                else
                {
                    deleted = realloc_temp;
                }
            }
        }

    } while (dirent.rec_len != 0);
    return deleted;
}

void inode_reader(char *buff, 
                __u32 index_in_table, 
                deleted_file_t deleted, 
                int fd,
                struct struct_ext2_filsys filsys
                )
{
    __u32 rec_len = filsys.super->s_inode_size*(index_in_table-1); 
    struct ext2_inode inode;
    memcpy(&inode, buff + rec_len, sizeof(struct ext2_inode)); 
    read_inode_blocks(inode, fd, filsys, deleted.file_name); 
}

void inode_recover(int fd,
                   struct struct_ext2_filsys filsys,
                   deleted_file_t *deleted_files,
                   __u32 deleted_files_c)
{
    struct ext2_inode inode;
    ext2fs_read_inode(&filsys, EXT2_JOURNAL_INO, &inode);
    for (__u32 itr = 0; itr <= deleted_files_c; itr++)
    {
        struct ext2_group_desc group_desc; 
        __u32 inodes_per_block = (filsys.blocksize / filsys.super->s_inode_size); 
        __u32 index_of_group = (deleted_files[itr].inode_num) / filsys.super->s_inodes_per_group; 
        __u32 index_of_ino_table = ((deleted_files[itr].inode_num) % filsys.super->s_inodes_per_group) / filsys.inode_blocks_per_group;  
        __u32 index_in_table = (deleted_files[itr].inode_num) % inodes_per_block;
        group_desc = read_gd(filsys, fd, index_of_group);
        /*calc inode table block num*/ 
        __u32 offset = group_desc.bg_inode_table + index_of_ino_table; // adress of group with inode table;
        /*recovering info using journal*/
        for (__u32 i = 1; i < inode.i_size / filsys.blocksize; i++)
        {
            __u32 *buff = read_inode_iblock_u32(inode, fd, i, *filsys.super, filsys.blocksize);
            //printf("i - %d\n", i);
            for (int j = 0; j < filsys.blocksize / (sizeof(__u32)); j++)
            {
                /*convert to lit endian*/
                buff[j] = convert_from_num(buff[j]);
            }
            journal_descriptor_block_t desc;
            journal_block_tag_t tag;
            __u16 rec_len = 0;
            /*read journal_desc*/
            memcpy(&desc, buff + rec_len, sizeof(journal_descriptor_block_t));
            rec_len += sizeof(journal_descriptor_block_t) / sizeof(__u32);
            /*block num in journal_descript*/
            __u32 block_num = 0;
            /*read tag*/
            tag = desc.d_tag;
            if (desc.d_header.h_magic == JOURNAL_MAGIC_NUMBER 
                && desc.d_header.h_sequence == deleted_files[itr].seq
                && desc.d_header.h_blocktype == descriptor_block
                )
            {
                /*scan until tag != 0*/
                while (tag.t_flags != 0 || tag.t_blocknr != 0)
                {
                    if (tag.t_flags == JFS_FLAG_UUID_EXIST)
                    {
                        rec_len += 16 / sizeof(__u32);
                    }

                    if (tag.t_blocknr == offset)
                    {
                        long int target_block = i + block_num + 1;
                        char *table_buff = (char*)read_inode_iblock_u32(inode, fd, target_block, *filsys.super, filsys.blocksize);
                        /*read inode table*/
                        inode_reader(table_buff, index_in_table, deleted_files[itr], fd, filsys);
                    }
                    block_num++;
                    memcpy(&tag, buff + rec_len, sizeof(journal_block_tag_t));
                    rec_len += sizeof(journal_block_tag_t) / sizeof(__u32);
                }
            }
        }
    }
}

void journal_parser(int fd,
                    struct struct_ext2_filsys filsys,
                    __u32 dirnum)
{
    /*init inode for read journal device*/
    struct ext2_inode inode;
    ext2fs_read_inode(&filsys, EXT2_JOURNAL_INO, &inode);

    /*initialize deleted_file struct*/
    deleted_file_t *deleted_file = (deleted_file_t *)malloc(sizeof(deleted_file_t));

    /*realloc size var and iterator*/
    __u32 deleted_file_c = 0;

    for (__u32 i = 1; i < inode.i_size / filsys.blocksize; i++)
    {
        __u32 *buff = read_inode_iblock_u32(inode, fd, i, *filsys.super, filsys.blocksize);
        //printf("i - %d\n", i);
        for (int j = 0; j < filsys.blocksize / (sizeof(__u32)); j++)
        {
            /*convert to lit endian*/
            buff[j] = convert_from_num(buff[j]);
        }

        journal_descriptor_block_t desc;
        journal_block_tag_t tag;
        __u16 rec_len = 0;
        /*read journal_desc*/
        memcpy(&desc, buff + rec_len, sizeof(journal_descriptor_block_t));
        rec_len += sizeof(journal_descriptor_block_t) / sizeof(__u32);
        /*block num in journal_descript*/
        __u32 block_num = 0;
        /*read tag*/
        tag = desc.d_tag;
        if (desc.d_header.h_magic == JOURNAL_MAGIC_NUMBER && desc.d_header.h_blocktype == descriptor_block)
        {
            /*scan until tag != 0*/
            while (tag.t_flags != 0 || tag.t_blocknr != 0)
            {
                if (tag.t_flags == JFS_FLAG_UUID_EXIST)
                {
                    rec_len += 16 / sizeof(__u32);
                }

                if (tag.t_blocknr == dirnum)
                {
                    long int target_block = i + block_num + 1;
                    char *dir_buff = read_inode_iblock(inode, fd, target_block, *filsys.super, filsys.blocksize);
                    deleted_file = scan_dir(dir_buff, filsys, deleted_file, &deleted_file_c, desc.d_header.h_sequence);
                }
                block_num++;
                memcpy(&tag, buff + rec_len, sizeof(journal_block_tag_t));
                rec_len += sizeof(journal_block_tag_t) / sizeof(__u32);
            }
        }
        free(buff);
    }
    /*here we start recovery file after check journal*/
    inode_recover(fd, filsys, deleted_file, deleted_file_c);
}