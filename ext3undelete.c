#include "./header/dir_parser.h"
#include "./header/journal_recovery.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Please, enter a ./ext3undelete [device] [path-to-directory]\n");
        exit(-1);
    }
    unsigned int block_size = 0;
    block_size = get_block_size(argv[1]);
    struct struct_ext2_filsys *file_system = malloc(sizeof(struct struct_ext2_filsys));
    ext2fs_open(argv[1], EXT2_FLAG_RW, 0, block_size, unix_io_manager, &file_system);
    if (extXdetector(file_system->super) != 3)
    {
        printf("This not EXT3 filesystem\n");
        exit(-1);
    }
    int fd = open_device(argv[1]);
    dir_checker(argv[2]);
    __u32 dirnum = dir_parser(argv[2], *file_system, fd);
    journal_parser(fd, *file_system, dirnum);
    close_device(fd);
    return 0;
}