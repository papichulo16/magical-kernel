#include <stdint.h>
#include <stdbool.h>

#include "print.h"
#include "thread.h"
#include "drivers.h"
#include "cmd.h"
#include "mklib.h"
#include "inlines.c"
#include "pci.h"

void mk_ahci_init() {

	uint32_t addr = pci_enum_device((uint16_t) INTEL_VEND_ID, (uint16_t) ICH9_AHCI_DEV);	

	uint8_t bus = PCI_ADDR_BUS(addr);
	uint8_t slot = PCI_ADDR_SLOT(addr);
	uint8_t func = PCI_ADDR_FUNC(addr);

	print_dword((uint32_t) bus);
	print_char(' ');
	print_dword((uint32_t) slot);
	print_char(' ');
	print_dword((uint32_t) func);
}

