#include <stdint.h>
#include <stdbool.h>

#include "print.h"
#include "thread.h"
#include "drivers/ahci.h"
#include "drivers/ext2.h"
#include "cmd.h"
#include "mklib.h"
#include "inlines.c"
#include "pci.h"
#include "page.h"
#include "virt.h"
#include "slab.h"

void mk_ext2_init() {
  struct ext2_sb_t* sb = mkmalloc(sizeof(struct ext2_sb_t));
  struct ahci_rw_port_t* port = mk_g_ahci_head();

  assert(!mk_ahci_read(port, 2, 2, sb), "[!] ext2.c: ext2 superblock read fail.");
  assert(sb->magic != EXT2_MAGIC, "[!] ext2.c: ext2 superblock read fail.");

  print_str("here!\n");
}


