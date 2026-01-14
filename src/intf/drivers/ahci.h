#pragma once

#include <stdint.h>

#define INTEL_VEND_ID 0x8086
#define ICH9_AHCI_DEV 0x2922

#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX    0x35
#define ATA_DEV_BUSY            0x80
#define ATA_DEV_DRQ             0x08

#define FIS_TYPE_REG_H2D    0x27

struct ahci_ghc_t {
	volatile uint32_t cap;        // 0x00: Host Capabilities
	volatile uint32_t ghc;        // 0x04: Global Host Control
	volatile uint32_t is;         // 0x08: Interrupt Status
	volatile uint32_t pi;         // 0x0C: Ports Implemented
	volatile uint32_t vs;         // 0x10: Version
	volatile uint32_t ccc_ctl;    // 0x14: Command Completion Coalescing Control
	volatile uint32_t ccc_ports;  // 0x18: Command Completion Coalescing Ports
	volatile uint32_t em_loc;     // 0x1C: Enclosure Management Location
	volatile uint32_t em_ctl;     // 0x20: Enclosure Management Control
	volatile uint32_t cap2;       // 0x24: Host Capabilities Extended
	volatile uint32_t bohc;       // 0x28: BIOS/OS Handoff Control and Status
};

struct ahci_port_t {
    uint32_t clb;        // Command List Base Address (lower 32-bit)
    uint32_t clbu;       // Command List Base Address (upper 32-bit)
    uint32_t fb;         // FIS Base Address (lower 32-bit)
    uint32_t fbu;        // FIS Base Address (upper 32-bit)
    uint32_t is;         // Interrupt Status
    uint32_t ie;         // Interrupt Enable
    volatile uint32_t cmd;        // Command and Status
    uint32_t rsv0;       // Reserved
    uint32_t tfd;        // Task File Data
    uint32_t sig;        // Signature
    uint32_t ssts;       // SATA Status (SCR0: SStatus)
    uint32_t sctl;       // SATA Control (SCR2: SControl)
    uint32_t serr;       // SATA Error (SCR1: SError)
    uint32_t sact;       // SATA Active (SCR3: SActive)
    uint32_t ci;         // Command Issue
    uint32_t sntf;       // SATA Notification (SCR4: SNotification)
    uint32_t fbs;        // FIS-based Switching Control
    uint32_t rsv1[11];   // Reserved
    uint32_t vendor[4];  // Vendor Specific
};

typedef struct {
    uint8_t cfl:5;       // Command FIS Length (in DWORDS)
    uint8_t a:1;         // ATAPI
    uint8_t w:1;         // Write (1: H2D, 0: D2H)
    uint8_t p:1;         // Prefetchable
    
    uint8_t r:1;         // Reset
    uint8_t b:1;         // BIST
    uint8_t c:1;         // Clear Busy upon R_OK
    uint8_t rsv0:1;
    uint8_t pmp:4;       // Port Multiplier Port
    
    uint16_t prdtl;      // Physical Region Descriptor Table Length
    uint32_t prdbc;      // Physical Region Descriptor Byte Count
    uint32_t ctba;       // Command Table Base Address (lower 32-bit)
    uint32_t ctbau;      // Command Table Base Address (upper 32-bit)
    uint32_t rsv1[4];
} HBA_CMD_HEADER;

typedef struct {
    uint32_t dba;        // Data Base Address (lower 32-bit)
    uint32_t dbau;       // Data Base Address (upper 32-bit)
    uint32_t rsv0;
    uint32_t dbc:22;     // Byte count (max 4MB - 1, 0-based)
    uint32_t rsv1:9;
    uint32_t i:1;        // Interrupt on completion
} HBA_PRDT_ENTRY;

typedef struct {
    uint8_t cfis[64];    // Command FIS
    uint8_t acmd[16];    // ATAPI Command
    uint8_t rsv[48];
    HBA_PRDT_ENTRY prdt_entry[1];  // Can have more entries
} HBA_CMD_TBL;

typedef struct {
    uint8_t fis_type;    // FIS_TYPE_REG_H2D
    
    uint8_t pmport:4;    // Port multiplier
    uint8_t rsv0:3;
    uint8_t c:1;         // 1: Command, 0: Control
    
    uint8_t command;     // Command register
    uint8_t featurel;    // Feature register, 7:0
    
    uint8_t lba0;        // LBA low register, 7:0
    uint8_t lba1;        // LBA mid register, 15:8
    uint8_t lba2;        // LBA high register, 23:16
    uint8_t device;      // Device register
    
    uint8_t lba3;        // LBA register, 31:24
    uint8_t lba4;        // LBA register, 39:32
    uint8_t lba5;        // LBA register, 47:40
    uint8_t featureh;    // Feature register, 15:8
    
    uint8_t countl;      // Count register, 7:0
    uint8_t counth;      // Count register, 15:8
    uint8_t icc;         // Isochronous command completion
    uint8_t control;     // Control register
    
    uint8_t rsv1[4];
} FIS_REG_H2D;

struct ahci_rw_port_t {
  struct ahci_port_t* port;

  HBA_CMD_HEADER* clb;
  HBA_CMD_TBL* ctb[32];
  void* fisb;

  struct ahci_rw_port_t* n;
};

struct ahci_hba_mem_t {
	struct ahci_ghc_t ghc;
  char resv[0x100 - 0x2c];
  struct ahci_port_t ports[32];
};

void* mk_g_ahci_mmio();
int mk_ahci_init();

struct ahci_rw_port_t* mk_g_ahci_head();

int mk_ahci_write(struct ahci_rw_port_t *port, uint64_t lba, uint32_t count, const void *buffer);
int mk_ahci_read(struct ahci_rw_port_t *port, uint64_t lba, uint32_t count, void *buffer);


