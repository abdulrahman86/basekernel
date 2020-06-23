#ifndef PRINTF_H
#define PRINTF_H

#include "kernel/types.h"

enum pci_bar_type
{
    MemoryMapping = 0,
    InputOutput = 1
};

        
struct pci_bar {

    bool prefetchable;
    uint8_t* address;
    uint32_t size;
    enum pci_bar_type type;
};

struct pci_bdt {

    uint16_t bus;
    uint16_t device;
    uint16_t function;
};


struct pci_dev {

	uint32_t portBase;
    uint32_t interrupt;
            
    uint16_t bus;
    uint16_t device;
    uint16_t function;

    uint16_t vendor_id;
    uint16_t device_id;
            
    uint8_t class_id;
    uint8_t subclass_id;
    uint8_t interface_id;

    uint8_t revision;
    struct pci_bar bar[6];
};

void pci_init();
// void pci_write_cmd_port(uint32_t cmd);
// void pci_write_data_port(uint32_t data);
// uint32_t pci_read_data_port();
// uint32_t pci_read_dev_register(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
// uint32_t pci_write_dev_register(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value);
// bool pci_dev_has_functions(uint16_t bus, uint16_t device);
// struct pci_dev pci_get_dev(uint16_t bus, uint16_t device, uint16_t function);
// struct pci_bar get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);

#endif
