#include "../header/read_iblock.h"

char *read_inode_iblock(struct ext2_inode inode,
                        int fd,
                        long int i_block_num,
                        struct ext2_super_block super,
                        __u32 block_size)
{
    /*
    EXT2_NDIR_BLOCKS 0..11
    EXT2_IND_BLOCK   12
    EXT2_DIND_BLOCK  13
    EXT2_TIND_BLOCK  14
    */
    /*
     offset to info
     */
    /*
     size of adress block
     */
    it count_offset_blks = block_size / 4;

    char *inf_buff = (char *)malloc(sizeof(char) * block_size);
    __u32 *adr_buff = (__u32 *)malloc(sizeof(__u32) * count_offset_blks);
    /*
        0..11
        DIRECT BLOCK
    */
    it first_lvl = EXT2_NDIR_BLOCKS - 1;
    /*
        12..(block_size/4)+11
        INDIRECT BLOCK
    */
    it second_lvl = count_offset_blks + first_lvl;
    /*
        (block_size/4)+12)..((block_size/4)^2) + (block_size/4)+11)
        DOUBLE INDIRECT BLOCK
    */
    it third_lvl = count_offset_blks * count_offset_blks + second_lvl;

    /*
        (((block_size/4)^2) + (block_size/4)+12))..(block_size/4)^3) + (block_size/4)^2 + (block_size/4)+11
        TRIPLE INDIRECT BLOCK
    */

    it fourth_lvl = (count_offset_blks * count_offset_blks * count_offset_blks) + (count_offset_blks * count_offset_blks) + second_lvl;
    long int first = i_block_num - first_lvl;
    long int second = i_block_num - second_lvl;
    long int third = i_block_num - third_lvl;
    long int fourth = i_block_num - fourth_lvl;
    if (first <= 0)
    {
        it pos = inode.i_block[i_block_num] * block_size;
        pread(fd, inf_buff, block_size, pos);
    }
    else if (second <= 0)
    {
        long int id = i_block_num - first_lvl - 1;

        while (id - count_offset_blks > 0)
        {
            id -= count_offset_blks;
        }

        it dir_pos = inode.i_block[EXT2_IND_BLOCK] * block_size;
        pread(fd, adr_buff, count_offset_blks, dir_pos);

        it pos = adr_buff[id] * block_size;
        pread(fd, inf_buff, count_offset_blks, pos);
    }
    else if (third <= 0)
    {
        long int id = i_block_num - second_lvl - 1;

        it ddir_pos = inode.i_block[EXT2_DIND_BLOCK] * block_size;
        pread(fd, adr_buff, count_offset_blks, ddir_pos);

        for (it i = 0; i < count_offset_blks; i++)
        {
            if (id < count_offset_blks)
            {
                it dir_pos = adr_buff[i] * block_size;
                pread(fd, adr_buff, block_size, dir_pos);

                it pos = adr_buff[id] * block_size;
                pread(fd, inf_buff, count_offset_blks, pos);

                break;
            }
            else
            {
                id -= count_offset_blks;
                continue;
            }
        }
    }
    else if (fourth < 0)
    {
        long int id = i_block_num - third_lvl - 1;

        it count_d_offset_blocks = count_offset_blks * count_offset_blks;
        it tdir_pos = inode.i_block[EXT2_TIND_BLOCK] * block_size;
        pread(fd, adr_buff, count_offset_blks, tdir_pos);

        // it d = 0;
        // it i = 0;
        for (it d = 0; d < count_offset_blks; d++)
        {
            if (id < count_d_offset_blocks)
            {
                it ddir_pos = adr_buff[d] * block_size;
                pread(fd, adr_buff, count_offset_blks, ddir_pos);

                for (it i = 0; i < count_offset_blks; i++)
                {
                    if (id < count_offset_blks)
                    {
                        it dir_pos = adr_buff[i] * block_size;
                        pread(fd, adr_buff, block_size, dir_pos);
                        it pos = adr_buff[id] * block_size;
                        pread(fd, inf_buff, count_offset_blks, pos);
                        break;
                    }
                    else
                    {
                        id -= count_offset_blks;
                        continue;
                    }
                }
            }
            else
            {
                id -= count_d_offset_blocks;
            }
        }
    }
    else
    {
        fprintf(stderr, "BLOCK DOES NOT EXIST!");
        exit(-1);
    }
    free(adr_buff);
    return inf_buff;
}

__u32 *read_inode_iblock_u32(struct ext2_inode inode,
                             int fd,
                             long int i_block_num,
                             struct ext2_super_block super,
                             __u32 block_size)
{
    // if (i_block_num > (inode.i_size / block_size))
    // {
    //     return 0;
    // }
    /*
    EXT2_NDIR_BLOCKS 0..11
    EXT2_IND_BLOCK   12
    EXT2_DIND_BLOCK  13
    EXT2_TIND_BLOCK  14
    */
    /*
     offset to info
     */
    /*
     size of adress block
     */
    it count_offset_blks = block_size / 4;

    __u32 *inf_buff = (__u32 *)malloc(sizeof(__u32) * count_offset_blks);
    __u32 *adr_buff = (__u32 *)malloc(sizeof(__u32) * count_offset_blks);
    /*
        0..11
        DIRECT BLOCK
    */
    it first_lvl = EXT2_NDIR_BLOCKS - 1;
    /*
        12..(block_size/4)+11
        INDIRECT BLOCK
    */
    it second_lvl = count_offset_blks + first_lvl;
    /*
        (block_size/4)+12)..((block_size/4)^2) + (block_size/4)+11)
        DOUBLE INDIRECT BLOCK
    */
    it third_lvl = count_offset_blks * count_offset_blks + second_lvl;

    /*
        (((block_size/4)^2) + (block_size/4)+12))..(block_size/4)^3) + (block_size/4)^2 + (block_size/4)+11
        TRIPLE INDIRECT BLOCK
    */

    it fourth_lvl = (count_offset_blks * count_offset_blks * count_offset_blks) + (count_offset_blks * count_offset_blks) + second_lvl;
    long int first = i_block_num - first_lvl;
    long int second = i_block_num - second_lvl;
    long int third = i_block_num - third_lvl;
    long int fourth = i_block_num - fourth_lvl;
    if (first <= 0)
    {
        it pos = inode.i_block[i_block_num] * block_size;
        pread(fd, inf_buff, block_size, pos);
    }
    else if (second <= 0)
    {
        long int id = i_block_num - first_lvl - 1;

        while (id - count_offset_blks > 0)
        {
            id -= count_offset_blks;
        }

        it dir_pos = inode.i_block[EXT2_IND_BLOCK] * block_size;
        pread(fd, adr_buff, count_offset_blks, dir_pos);

        it pos = adr_buff[id] * block_size;
        pread(fd, inf_buff, block_size, pos);
    }
    else if (third <= 0)
    {
        long int id = i_block_num - second_lvl - 1;

        it ddir_pos = inode.i_block[EXT2_DIND_BLOCK] * block_size;
        pread(fd, adr_buff, count_offset_blks, ddir_pos);

        for (it i = 0; i < count_offset_blks; i++)
        {
            if (id < count_offset_blks)
            {
                it dir_pos = adr_buff[i] * block_size;
                pread(fd, adr_buff, block_size, dir_pos);

                it pos = adr_buff[id] * block_size;
                pread(fd, inf_buff, block_size, pos);

                break;
            }
            else
            {
                id -= count_offset_blks;
                continue;
            }
        }
    }
    else if (fourth < 0)
    {
        long int id = i_block_num - third_lvl - 1;

        it count_d_offset_blocks = count_offset_blks * count_offset_blks;
        it tdir_pos = inode.i_block[EXT2_TIND_BLOCK] * block_size;
        pread(fd, adr_buff, count_offset_blks, tdir_pos);

        // it d = 0;
        // it i = 0;
        for (it d = 0; d < count_offset_blks; d++)
        {
            if (id < count_d_offset_blocks)
            {
                it ddir_pos = adr_buff[d] * block_size;
                pread(fd, adr_buff, count_offset_blks, ddir_pos);

                for (it i = 0; i < count_offset_blks; i++)
                {
                    if (id < count_offset_blks)
                    {
                        it dir_pos = adr_buff[i] * block_size;
                        pread(fd, adr_buff, block_size, dir_pos);
                        it pos = adr_buff[id] * block_size;
                        pread(fd, inf_buff, block_size, pos);
                        break;
                    }
                    else
                    {
                        id -= count_offset_blks;
                        continue;
                    }
                }
            }
            else
            {
                id -= count_d_offset_blocks;
            }
        }
    }
    else
    {
        fprintf(stderr, "BLOCK DOES NOT EXIST!");
        exit(-1);
    }
    free(adr_buff);
    return inf_buff;
}

int read_inode_blocks(struct ext2_inode inode,
                      int fd,
                      struct struct_ext2_filsys filsys,
                      char file_name[])
{
    size_t size = inode.i_size;
    size_t count_blk = size / filsys.blocksize;
    size_t last_blk = size % filsys.blocksize;
    if ((count_blk == 0 && size))
    {
        count_blk += 1;
    }
    int rec_file_fd;
    if ((rec_file_fd = creat(file_name, O_WRONLY | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
    {
        fprintf(stderr, "Create file error\n");
    }
    char *buff;
    for (int i = 0; i <= count_blk; i++)
    {
        buff = (char*)read_inode_iblock_u32(inode, fd, i, *filsys.super, filsys.blocksize);
        lseek(rec_file_fd, 0, SEEK_END);
        //size_t str_len = strlen(buff);
        if ((i == (count_blk)))
        {
            write(rec_file_fd, buff, last_blk);
        }
        else
        {
            write(rec_file_fd, buff, filsys.blocksize);
        }
        free(buff);
    }
    it writed_size = lseek(rec_file_fd, 0, SEEK_END);
    if (writed_size == size)
    {
        printf("File:%s write success!\nBytes writed: %u\n\n", file_name, writed_size);
        close(rec_file_fd);
        return 0;
    }
    else
    {
        close(rec_file_fd);
        return 1;
    }
}