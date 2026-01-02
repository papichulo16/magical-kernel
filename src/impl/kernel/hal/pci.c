#include <stdint.h>

#include "print.h"
#include "inlines.c"
#include "pci.h"

uint32_t pci_enum_device(uint16_t vendor, uint16_t device) {
	for (int bus = 0; bus < 8; bus++) {
		for (int slot = 0; slot < 32; slot++) {
			for (int func = 0; func < 8; func++) {
				uint32_t tmp = pci_config_read_dword(bus, slot, func, 0);

				if ((uint16_t) (tmp & 0xffff) != vendor) continue;
				if ((uint16_t) ((tmp >> 16) & 0xffff) != device) continue;

				uint32_t address;
				uint32_t lbus  = (uint32_t)bus;
			    	uint32_t lslot = (uint32_t)slot;
			    	uint32_t lfunc = (uint32_t)func;
			  
			    	// Create configuration address as per Figure 1
			    	address = (uint32_t)((lbus << 16) | (lslot << 11) |
			      		(lfunc << 8) | ((uint32_t)0x80000000));

				return address;
			}
		}
	}

	return (uint32_t) ~0;
			
}

uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = inl(0xCFC);
    return tmp;
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}


