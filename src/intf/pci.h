#pragma once

#include <stdint.h>

#define PCI_ADDR_BUS(addr)      (((addr) >> 16) & 0xFF)
#define PCI_ADDR_SLOT(addr)   (((addr) >> 11) & 0x1F)
#define PCI_ADDR_FUNC(addr) (((addr) >> 8) & 0x07)
#define PCI_ADDR_OFF(addr)   ((addr) & 0xFC)
#define PCI_ADDR_EN(addr)   (((addr) >> 31) & 0x01)

#define PCI_VENDOR_ID(data)     ((data) & 0xFFFF)
#define PCI_DEVICE_ID(data)     (((data) >> 16) & 0xFFFF)

#define PCI_CLASS_CODE(data)    (((data) >> 24) & 0xFF)
#define PCI_SUBCLASS(data)      (((data) >> 16) & 0xFF)
#define PCI_PROG_IF(data)       (((data) >> 8) & 0xFF)
#define PCI_REVISION(data)      ((data) & 0xFF)

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

uint32_t pci_enum_device(uint16_t vendor, uint16_t device);

