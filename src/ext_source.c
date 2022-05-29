#include "../header/ext_source.h"

int open_device(char *name)
{
    int fd = 0;
    if ((fd = open(name, O_RDONLY)) < 0)
    {
        perror("cant open device file!");
        exit(-1);
    }
    return fd;
}

void close_device(int fd)
{
    if (close(fd) < 0)
    {
        perror("close");
        exit(-1);
    }
}

unsigned int get_block_size(char *name)
{
    int fd = open_device(name);
    int block_size = 0;
    struct ext2_super_block *super = (struct ext2_super_block *)malloc(sizeof(struct ext2_super_block));
    if (lseek(fd, BASE_OFFSET, SEEK_SET) < 0)
    {
        perror("seek");
        exit(-1);
    }
    if (read(fd, super, sizeof(*super)) < 0)
    {
        perror("read");
        exit(-1);
    }
    if (super->s_magic != EXT2_SUPER_MAGIC)
    {
        fprintf(stderr, "Not a ExtX filesystem\n");
        exit(1);
    }
    block_size = 1024 << super->s_log_block_size;
    close_device(fd);
    free(super);
    return block_size;
}

int extXdetector(struct ext2_super_block* super)
{
    if (EXT2_SUPER_MAGIC != super->s_magic)
    {
        fprintf(stderr, "This not extfs");
        exit(-1);
    }
    int flag = 0;
    if ((ext2fs_has_feature_xattr(super) != 0) && (ext2fs_has_feature_sparse_super(super)) != 0)
    {
        flag = 2;
    }
    if ((ext2fs_has_feature_journal(super) != 0) && (ext2fs_has_feature_dir_index(super) != 0))
    {
        flag = 3;
    }
    if ((ext2fs_has_feature_64bit(super) != 0) && (ext2fs_has_feature_extents(super) != 0) &&
        (ext2fs_has_feature_extra_isize(super) != 0) && (ext2fs_has_feature_huge_file(super) != 0))
    {
        flag = 4;
    }
    return flag;
}

struct ext2_group_desc read_gd(struct struct_ext2_filsys filsys, int fd, int group)
{
    unsigned char buff_grp[filsys.blocksize];
    if (lseek(fd, (filsys.super->s_first_data_block + 1) * filsys.blocksize, 0) < 0)
    {
        perror("lseek");
        exit(-1);
    }

    if (read(fd, buff_grp, filsys.blocksize) < 0)
    {
        perror("read");
        exit(-1);
    }
    struct ext2_group_desc gd;

    memset((void *)&gd, 0, sizeof(gd));
    memcpy((void *)&gd, buff_grp + (group * (sizeof(gd))), sizeof(gd));

    return gd;
}