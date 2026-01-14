#include <stdint.h>
#include <stdbool.h>

#include "print.h"
#include "thread.h"
#include "drivers/ahci.h"
#include "cmd.h"
#include "mklib.h"
#include "inlines.c"
#include "pci.h"
#include "page.h"
#include "virt.h"
#include "slab.h"

/**
 * 2.1 PCI Header
Start (hex) End (hex) Symbol Name
00 03 ID Identifiers
04 05 CMD Command Register
06 07 STS Device Status
08 08 RID Revision ID
09 0B CC Class Codes
0C 0C CLS Cache Line Size
0D 0D MLT Master Latency Timer
0E 0E HTYPE Header Type
0F 0F BIST Built In Self Test (Optional)
10 23 BARS Other Base Address Registres (Optional) <BAR0-4>
24 27 ABAR AHCI Base Address <BAR5>
2C 2F SS Subsystem Identifiers
30 33 EROM Expansion ROM Base Address (Optional)
34 34 CAP Capabilities Pointer
3C 3D INTR Interrupt Information
3E 3E MGNT Min Grant (Optional)
3F 3F MLAT Max Latency (Optional

Start End Description
00h 2Bh Generic Host Control
2Ch 5Fh Reserved
60h 9Fh Reserved for NVMHCI
A0h FFh Vendor Specific registers
100h 17Fh Port 0 port control registers
180h 1FFh Port 1 port control registers
200h FFFh (Ports 2 – port 29 control registers)
1000h 107Fh Port 30 port control registers
1080h 10FFh Port 31 port control registers

Start End Symbol Description
00h 03h PxCLB Port x Command List Base Address
04h 07h PxCLBU Port x Command List Base Address Upper 32-Bits
08h 0Bh PxFB Port x FIS Base Address
0Ch 0Fh PxFBU Port x FIS Base Address Upper 32-Bits
10h 13h PxIS Port x Interrupt Status
14h 17h PxIE Port x Interrupt Enable
18h 1Bh PxCMD Port x Command and Status
1Ch 1Fh Reserved Reserved
20h 23h PxTFD Port x Task File Data
24h 27h PxSIG Port x Signature
28h 2Bh PxSSTS Port x Serial ATA Status (SCR0: SStatus)
2Ch 2Fh PxSCTL Port x Serial ATA Control (SCR2: SControl)
30h 33h PxSERR Port x Serial ATA Error (SCR1: SError)
34h 37h PxSACT Port x Serial ATA Active (SCR3: SActive)
38h 3Bh PxCI Port x Command Issue
3Ch 3Fh PxSNTF Port x Serial ATA Notification (SCR4: SNotification)
40h 43h PxFBS Port x FIS-based Switching Control
44h 47h PxDEVSLP Port x Device Sleep
48h 6Fh Reserved Reserved
70h 7Fh PxVS Port x Vendor Specific

 *Bit Type Reset Description
31:12 RO 0 Reserved
11:08 RO 0
Interface Power Management (IPM): Indicates the current interface state:
0h Device not present or communication not established
1h Interface in active state
2h Interface in Partial power management state
6h Interface in Slumber power management state
8h Interface in DevSleep power management state
All other values reserved
07:04 RO 0
Current Interface Speed (SPD): Indicates the negotiated interface communication
speed.
0h Device not present or communication not established
1h Generation 1 communication rate negotiated
2h Generation 2 communication rate negotiated
3h Generation 3 communication rate negotiated
All other values reserved
03:00 RO 0
Device Detection (DET): Indicates the interface device detection and Phy state.1
0h No device detected and Phy communication not established
1h Device presence detected but Phy communication not established
3h Device presence detected and Phy communication established
4h Phy in offline mode as a result of the interface being disabled or running in a
BIST loopback mode
*/

struct ahci_rw_port_t* ahci_rw_head = 0;

struct ahci_rw_port_t* mk_g_ahci_head() {
  return ahci_rw_head;
}

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

// Start command engine
void start_cmd(struct ahci_port_t *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}

// Stop command engine
void stop_cmd(struct ahci_port_t *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;

	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;

	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;

		break;
	}
}

uint8_t ctb[PAGE_SIZE * 2];

struct ahci_rw_port_t* ahci_port_push(struct ahci_port_t* port) {
  struct ahci_rw_port_t* ahci_rw = mkmalloc(sizeof(struct ahci_rw_port_t));

  void* clb = mk_vmmap_l1(0);
  void* fisb = mk_vmmap_l1(0);

  ahci_rw->clb = clb;
  ahci_rw->fisb = fisb;

  // i need a buddy allocator to map 2 pages lol, im lazy
  //void* page = 0;
  void* page = ctb;

  for (int i = 0; i < 32; i++) {
    int off = (i * 0x80) % PAGE_SIZE;

    // i need a buddy allocator to map 2 pages lol, im lazy
    //if (off == 0)
      //page = mk_vmmap_l1(0);

    void* vaddr = page + off;
    void* paddr = _mk_g_paddr(vaddr);

    ahci_rw->ctb[i] = vaddr;
    
    ahci_rw->clb[i].prdtl = 8;
    ahci_rw->clb[i].ctba = (uint64_t) paddr & 0xFFFFFFFF;
    ahci_rw->clb[i].ctbau = ((uint64_t) paddr >> 32) & 0xFFFFFFFF;

    _memset(vaddr, 0, 256);
  }

  ahci_rw->port = port;

  ahci_rw->n = ahci_rw_head;
  ahci_rw_head = ahci_rw;

  return ahci_rw;
}

void ahci_port_init(struct ahci_port_t* port) {
  stop_cmd(port);

  struct ahci_rw_port_t* rw_p = ahci_port_push(port);

  void* paddr_clb = mk_g_paddr(rw_p->clb);
  void* paddr_fisb = mk_g_paddr(rw_p->fisb);

  port->clb = (uint32_t) ((uint64_t) paddr_clb & 0xffffffff);
  port->clbu = (uint32_t) (((uint64_t) paddr_clb >> 32) & 0xffffffff);
  port->fb = (uint32_t) ((uint64_t) paddr_fisb & 0xffffffff);
  port->fbu = (uint32_t) (((uint64_t) paddr_fisb >> 32) & 0xffffffff);

  start_cmd(port);
}

int mk_ahci_init() {
	struct ahci_hba_mem_t* mmio = mk_g_ahci_mmio();

	if (mmio == 0)
		return -1;

  for (int i = 0; i < 32; i++) {
    if (mmio->ghc.pi & (1 << i) == 0)
      continue;

    uint32_t status = mmio->ports[i].ssts;
    uint8_t detection = status & 0xf;
    uint8_t power = (status >> 8) & 0xf;

    // power off or not active
    if (detection != 3 || power != 1)
      continue;

    // SATA
    if (mmio->ports[i].sig != 0x0101)
      continue;

    ahci_port_init(&mmio->ports[i]);
  }
}

void* mk_g_ahci_mmio() {

	uint32_t addr = pci_enum_device((uint16_t) INTEL_VEND_ID, (uint16_t) ICH9_AHCI_DEV);	

	if (addr == ~0)
		return 0;

	uint8_t bus = PCI_ADDR_BUS(addr);
	uint8_t slot = PCI_ADDR_SLOT(addr);
	uint8_t func = PCI_ADDR_FUNC(addr);

	// CLASS CODE: 01 - Mass Storage Container
	// SUBCLASS: 06 SATA Controller
	// Prog IF: 01 AHCI 1.0
	if (pci_config_read_dword(bus, slot, func, 0x8) >> 8 != 0x010601)
		return 0;

	uint32_t phys_abar_mmio = pci_config_read_dword(bus, slot, func, 0x24) & ~0xfff;
	uint64_t abar_mmio = mk_phys_to_mmio_virt((uint8_t *) phys_abar_mmio);

	return abar_mmio;
}

uint32_t find_free_slot(struct ahci_port_t* port) {
    uint32_t slots = port->ci | port->sact;
    
    for (int i = 0; i < 32; i++) {
        if (!(slots & (1 << i))) {
            return i;  
        }
    }
    
    return -1;
}

int mk_ahci_read(struct ahci_rw_port_t *port, uint64_t lba, uint32_t count, void *buffer) {
    
    uint32_t slot = find_free_slot(port->port);

    while (slot == -1)
      slot = find_free_slot(port->port);
    
    // Get command header and table
    HBA_CMD_HEADER *cmd_header = &port->clb[slot];
    HBA_CMD_TBL *cmd_table = port->ctb[slot];
    _memset(cmd_table, 0, 256);
    
    // Clear command header
    cmd_header->cfl = sizeof(FIS_REG_H2D) / 4;  // Command FIS size (5 DWORDs)
    cmd_header->w = 0;     // Read from device
    cmd_header->prdtl = 1; // One PRDT entry
    cmd_header->a = 0;     // Not ATAPI
    cmd_header->c = 0;     // Not clear busy
    cmd_header->p = 0;     // Not prefetchable
    
    // Set up PRDT entry
    uintptr_t buffer_phys = mk_g_paddr(buffer);
    cmd_table->prdt_entry[0].dba = buffer_phys & 0xFFFFFFFF;
    cmd_table->prdt_entry[0].dbau = (buffer_phys >> 32) & 0xFFFFFFFF;
    cmd_table->prdt_entry[0].dbc = (count * 512) - 1;  // Byte count (0-based)
    cmd_table->prdt_entry[0].i = 0;  // No interrupt
    
    // Set up command FIS
    FIS_REG_H2D *fis = (FIS_REG_H2D*)cmd_table->cfis;
    _memset(fis, 0, sizeof(FIS_REG_H2D));
    
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c = 1;  // Command register
    fis->command = ATA_CMD_READ_DMA_EX;
    
    // LBA (48-bit)
    fis->lba0 = (uint8_t)(lba);
    fis->lba1 = (uint8_t)(lba >> 8);
    fis->lba2 = (uint8_t)(lba >> 16);
    fis->lba3 = (uint8_t)(lba >> 24);
    fis->lba4 = (uint8_t)(lba >> 32);
    fis->lba5 = (uint8_t)(lba >> 40);
    
    fis->device = 1 << 6;  // LBA mode
    
    // Sector count
    fis->countl = count & 0xFF;
    fis->counth = (count >> 8) & 0xFF;

    // Issue command
    port->port->ci = 1 << slot;
    
    // Wait for completion
    while (1) {
        if (!(port->port->ci & (1 << slot)))
            break;

        if (port->port->is & (1 << 30)) {  // Task file error
            return 0;
        }
    }
   
    // Check for errors
    if (port->port->is & (1 << 30)) {  // Task file error bit
        return 0;
    }

    port->port->is = 0xffffffff;  // Clear interrupts
    
    return 1;
}

int mk_ahci_write(struct ahci_rw_port_t *port, uint64_t lba, uint32_t count, const void *buffer) {
    
    uint32_t slot = find_free_slot(port->port);
    while (slot == -1)
      slot = find_free_slot(port->port);

    // Get command header and table
    HBA_CMD_HEADER *cmd_header = &port->clb[slot];
    HBA_CMD_TBL *cmd_table = port->ctb[slot];
    _memset(cmd_table, 0, 256);
    
    // Set up command header
    cmd_header->cfl = sizeof(FIS_REG_H2D) / 4;  // Command FIS size (5 DWORDs)
    cmd_header->w = 1;     // Write to device (THIS IS THE KEY CHANGE)
    cmd_header->prdtl = 1; // One PRDT entry
    cmd_header->a = 0;     // Not ATAPI
    cmd_header->c = 0;     // Not clear busy
    cmd_header->p = 0;     // Not prefetchable
    
    // Set up PRDT entry
    uintptr_t buffer_phys = mk_g_paddr((void*)buffer);
    cmd_table->prdt_entry[0].dba = buffer_phys & 0xFFFFFFFF;
    cmd_table->prdt_entry[0].dbau = (buffer_phys >> 32) & 0xFFFFFFFF;
    cmd_table->prdt_entry[0].dbc = (count * 512) - 1;  // Byte count (0-based)
    cmd_table->prdt_entry[0].i = 0;  // No interrupt
    
    // Set up command FIS
    FIS_REG_H2D *fis = (FIS_REG_H2D*)cmd_table->cfis;
    _memset(fis, 0, sizeof(FIS_REG_H2D));
    
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c = 1;  // Command register
    fis->command = ATA_CMD_WRITE_DMA_EX;  // WRITE command instead of READ
    
    // LBA (48-bit)
    fis->lba0 = (uint8_t)(lba);
    fis->lba1 = (uint8_t)(lba >> 8);
    fis->lba2 = (uint8_t)(lba >> 16);
    fis->lba3 = (uint8_t)(lba >> 24);
    fis->lba4 = (uint8_t)(lba >> 32);
    fis->lba5 = (uint8_t)(lba >> 40);
    
    fis->device = 1 << 6;  // LBA mode
    
    // Sector count
    fis->countl = count & 0xFF;
    fis->counth = (count >> 8) & 0xFF;
    
    // Issue command
    port->port->ci = 1 << slot;
    
    // Wait for completion
    while (1) {
        if (!(port->port->ci & (1 << slot)))
            break;
        if (port->port->is & (1 << 30)) {  // Task file error
            return 0;
        }
    }

    // Check for errors
    if (port->port->is & (1 << 30)) {  // Task file error bit
        return 0;
    }

    port->port->is = 0xffffffff;  // Clear interrupts
    
    return 1;
}

