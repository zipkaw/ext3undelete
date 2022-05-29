cmake_minimum_required(VERSION 3.16)
project(ext3undelete)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Wall")

add_library(dirparser STATIC ./src/dir_parser.c)
add_library(extsource STATIC ./src/ext_source.c)
add_library(journalrec STATIC ./src/journal_recovery.c ./src/convert_to_LE.c)
add_library(readiblock STATIC ./src/read_iblock.c)

add_executable(ext3undelete ext3undelete.c)


target_link_libraries(${PROJECT_NAME}   -lext2fs
                                        dirparser
                                        extsource
                                        journalrec
                                        readiblock)