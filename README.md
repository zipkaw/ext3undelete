# ext3undelete

This is recovery file program for filesystem ext3 (with journaling)

Enter ext3undelete [device] [directory-path] for start recovering
To compile need ext2 library:
* to install ext2 library input in command line: 
  sudo apt-get install -y libext2fs-dev
  cmake . && make
