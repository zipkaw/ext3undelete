#include "../header/dir_parser.h"

void dir_checker(char *argv)
{
    /*
    example:

    *+ /dir1/dir2/dir3
    *+ /
    *+ /dir
    *+ /dir/

    *- //dir
    *- /dir//dir/
    *- d/ir/

    */

    int i = 0;
    unsigned short int symbol_flag = 0;
    unsigned short int slash_flag = 0;
    while (argv[i])
    {
        if (!slash_flag && !symbol_flag && argv[i] == '/')
        {
            slash_flag = 1;
            i++;
        }
        else if (argv[i] != '/')
        {
            symbol_flag = 1;
            i++;
        }
        else if (symbol_flag && slash_flag && argv[i] == '/')
        {
            if (argv[i + 1] == '\0')
            {
                perror("incorrect path");
                exit(-1);
            }
            else if (argv[i + 1] == '/')
            {
                perror("incorrect path");
                exit(-1);
            }
            symbol_flag = 0;
            slash_flag = 0;
        }
        else
        {
            perror("incorrect path");
            exit(-1);
        }
    }
}

__u32 dir_parser(char *dir, struct struct_ext2_filsys filsys, int fd)
{
    struct ext2_inode *inode = malloc(sizeof(struct ext2_inode));
    ext2fs_read_inode(&filsys, EXT2_ROOT_INO, inode);
    if (dir[0] == '/' && dir[1] == '\0')
    {
        return inode->i_block[0];
    }
    /*
    example:
    ** /dir1/dir2/dir3
    */
    char *buff = (char *)calloc(filsys.blocksize, sizeof(char));
    char *temp = (char *)malloc(sizeof(char) * 255);
    int i = 0;
    while (dir[i])
    {
        /* skip '/' */
        i++;

        memset(temp, 0, 255);
        int j = 0;
        while (dir[i] != '/' && dir[i] != '\0')
        {
            temp[j++] = dir[i];
            i++;
        }
        unsigned long long int block = inode->i_block[0];
        unsigned long long int blocksize = filsys.blocksize;
        unsigned long long int pos = block * blocksize;
        if (pread64(fd, buff, filsys.blocksize, pos) < 0)
        {
            perror("pread  zero!");
            exit(-1);
        };

        struct ext2_dir_entry_2 dirent;
        __u16 reclen = 0;

        do
        {
            memcpy(&dirent, buff + reclen, sizeof(struct ext2_dir_entry_2));
            reclen += dirent.rec_len;

            if (dirent.rec_len == 0)
            {
                perror("directory not found");
                exit(-1);
            }

            if (!memcmp(temp, dirent.name, dirent.name_len))
            {
                ext2fs_read_inode(&filsys, dirent.inode, inode);
                break;
            }

        } while (dirent.rec_len != 0);
    }
    free(temp);
    free(buff);
    return inode->i_block[0];
}