#include <stdint.h>
#include <stdbool.h>

#include "print.h"
#include "thread.h"
#include "drivers/ahci.h"
#include "cmd.h"
#include "mklib.h"
#include "inlines.c"
#include "pci.h"
#include "virt.h"

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

 */
int mk_ahci_init() {
	struct ahci_hba_mem_t* mmio = mk_g_ahci_mmio();

	if (mmio == 0)
		return -1;

	print_dword(mmio->ghc.pi);

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

