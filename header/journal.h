#ifndef __JOURNAL__
#define __JOURNAL__

/*
|SBlk |descritorBlk|                      |
|_____|____________|.....transactions.....|
*/

/*
Journal With two Transactions___________________________________________________
|JSBlk |DesSq|fileSysMDBlk|commit|DesSq|FilSysMD|commit|
|______|Blk__|_____|______|Blk___|_____|________|______|
*/

#include <ext2fs/ext2_types.h>

typedef enum journal_header_fields
{
    descriptor_block = 1,
    commit_block,
    superblock_v1,
    superblock_v2,
    revoke_block
} journal_header_fields_t;

#define JOURNAL_MAGIC_NUMBER 0xC03B3998
#define JOURNAL_UUID_SIZE 16  
/* 
**uuid size is 16 this unused 
**variale in our rec file method
*/
typedef struct journal_header_s
{
    __be32 h_magic;
    __be32 h_blocktype;
    __be32 h_sequence;
} journal_header_t;

/*journal tag structs for ext3 filsys*/

// typedef struct journal_block_tag_s
// {
//     __be32 t_blocknr;  /* The on-disk block number */
//     __be32 t_flags;    /* See below */
//     __u16 uuid[8];
// } journal_block_tag_t;

/*if t_flag != 0 use this*/
typedef struct journal_block_tag_s
{
    __be32 t_blocknr;  /* The on-disk block number */
    __be32 t_flags;    /* See below */
} journal_block_tag_t;

typedef struct journal_descriptor_block
{
    journal_header_t d_header;  /*0x0000 - 0x000C*/
    journal_block_tag_t d_tag;
} journal_descriptor_block_t;

/* Definitions for the journal tag flags word: */
#define JFS_FLAG_UUID_EXIST 0 /*uuid exist in next 16 bytes*/
#define JFS_FLAG_ESCAPE 1    /* on-disk block is escaped */
#define JFS_FLAG_SAME_UUID 2 /* block has same uuid as previous */
#define JFS_FLAG_DELETED 4   /* block deleted by this transaction */
#define JFS_FLAG_LAST_TAG 8  /* last tag in this descriptor block */


typedef struct journal_superblock_s
{
    /* 0x0000 */
    journal_header_t s_header;

    /* 0x000C */
    /* Static information describing the journal */
    __be32 s_blocksize; /* journal device blocksize */
    __be32 s_maxlen;    /* total blocks in journal file */
    __be32 s_first;     /* first block of log information */

    /* 0x0018 */
    /* Dynamic information describing the current state of the log */
    __be32 s_sequence; /* first commit ID expected in log */
    __be32 s_start;    /* blocknr of start of log */

    /* 0x0020 */
    /* Error value, as set by journal_abort(). */
    __be32 s_errno;

    /* 0x0024 */
    /* Remaining fields are only valid in a version-2 superblock */
    __be32 s_feature_compat;    /* compatible feature set */
    __be32 s_feature_incompat;  /* incompatible feature set */
    __be32 s_feature_ro_compat; /* readonly-compatible feature set */
                                /* 0x0030 */
    __u8 s_uuid[16];            /* 128-bit uuid for journal */

    /* 0x0040 */
    __be32 s_nr_users; /* Nr of filesystems sharing log */

    __be32 s_dynsuper; /* Blocknr of dynamic superblock copy*/

    /* 0x0048 */
    __be32 s_max_transaction; /* Limit of journal blocks per trans.*/
    __be32 s_max_trans_data;  /* Limit of data blocks per trans. */

    /* 0x0050 */
    __u32 s_padding[44];

    /* 0x0100 */
    __u8 s_users[16 * 48]; /* ids of all fs'es sharing the log */
                           /* 0x0400 */
} journal_superblock_t;

#endif //__JOURNAL__