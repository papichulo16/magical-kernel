#pragma once

#include <stdint.h>

#define EXT2_MAGIC 0xef53

// superblock
struct ext2_sb_t {
    uint32_t inodes_cnt;
    uint32_t blocks_cnt;
    uint32_t r_blocks_cnt;
    uint32_t free_blocks_cnt;
    uint32_t free_inodes_cnt;
    uint32_t first_data_blk;
    uint32_t log_blk_size;
    uint32_t log_frag_size;
    uint32_t blocks_per_grp;
    uint32_t frags_per_grp;
    uint32_t inodes_per_grp;
    uint32_t mtime;
    uint32_t wtime;

    uint16_t mnt_cnt;
    uint16_t max_mnt_cnt;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev;

    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;

    uint16_t def_resuid;
    uint16_t def_resgid;

    /* Dynamic rev only */
    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t blk_grp_nr;
    uint32_t feat_compat;
    uint32_t feat_incompat;
    uint32_t feat_ro_compat;

    uint8_t  uuid[16];
    char     vol_name[16];
    char     last_mount[64];

    uint32_t algo_bitmap;

    uint8_t  prealloc_blocks;
    uint8_t  prealloc_dir_blocks;
    uint16_t pad1;

    /* Journal (unused in ext2) */
    uint8_t  journal_uuid[16];
    uint32_t journal_ino;
    uint32_t journal_dev;
    uint32_t last_orphan;

    /* Dir index */
    uint32_t hash_seed[4];
    uint8_t  def_hash_ver;
    uint8_t  jnl_backup_type;
    uint16_t desc_size;

    uint32_t mount_opts;
    uint32_t first_meta_bg;
    uint32_t mkfs_time;

    uint32_t jnl_blocks[17];

    /* Extra */
    uint32_t blocks_cnt_hi;
    uint32_t r_blocks_cnt_hi;
    uint32_t free_blocks_cnt_hi;
    uint16_t min_extra_isize;
    uint16_t want_extra_isize;
    uint32_t flags;
    uint16_t raid_stride;
    uint16_t mmp_interval;
    uint64_t mmp_block;
    uint32_t raid_stripe_w;
    uint8_t  log_groups_per_flex;
    uint8_t  csum_type;
    uint16_t pad2;
    uint64_t kbytes_written;
    uint32_t snap_ino;
    uint32_t snap_id;
    uint64_t snap_r_blocks_cnt;
    uint32_t snap_list;
    uint32_t err_cnt;
    uint32_t first_err_time;
    uint32_t first_err_ino;
    uint64_t first_err_blk;
    uint8_t  first_err_func[32];
    uint32_t first_err_line;
    uint32_t last_err_time;
    uint32_t last_err_ino;
    uint32_t last_err_line;
    uint64_t last_err_blk;
    uint8_t  last_err_func[32];
    uint8_t  mount_opts_str[64];
    uint32_t usr_quota_ino;
    uint32_t grp_quota_ino;
    uint32_t overhead_blocks;
    uint8_t  backup_bgs[8];
    uint8_t  enc_algos[4];
    uint8_t  enc_pw_salt[16];
    uint32_t lpf_ino;
    uint32_t prj_quota_ino;
    uint32_t csum_seed;
    uint8_t  wtime_hi;
    uint8_t  mtime_hi;
    uint8_t  mkfs_time_hi;
    uint8_t  lastcheck_hi;
    uint8_t  first_err_time_hi;
    uint8_t  last_err_time_hi;
    uint8_t  pad3[2];
    uint32_t encoding;
    uint32_t encoding_flags;
    uint32_t reserved[95];
};

void mk_ext2_init();
