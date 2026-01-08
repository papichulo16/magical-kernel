#pragma once

#include <stdint.h>

#define INTEL_VEND_ID 0x8086
#define ICH9_AHCI_DEV 0x2922

void* mk_g_ahci_mmio();
int mk_ahci_init();

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

struct ahci_hba_mem_t {
	struct ahci_ghc_t ghc;
};

