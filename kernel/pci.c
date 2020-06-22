
#include "pci.h"
#include "printf.h"
#include "ioports.h"

#define PCI_DATA_PORT  0xCFC
#define PCI_CMD_PORT 0xCF8


//----------------------------Private functions--------------------------------------

void pci_write_cmd_port(uint32_t cmd);
void pci_write_data_port(uint32_t data);
uint32_t pci_read_data_port();
uint32_t pci_read_dev_register(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
uint32_t pci_write_dev_register(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value);
bool pci_dev_has_functions(uint16_t bus, uint16_t device);
struct pci_dev pci_get_dev(uint16_t bus, uint16_t device, uint16_t function);
struct pci_bar get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);
uint32_t get_number_of_lowest_set_bit(uint32_t value);
uint32_t get_number_of_highest_set_bit(uint32_t value);





//-----------------------------------------------------------------------------------


void pci_init() {

	
	printf("\nInitializing PCI:\n");

	int num_devices = 0;

	for(int bus = 0; bus < 256; bus++)
    {
    	for(int device = 0; device < 32; device++)
        {
        	bool dev_has_functions = pci_dev_has_functions(bus, device);
            int numFunctions = dev_has_functions ? 8 : 1;

            for(int function = 0; function < numFunctions; function++)
            {
                struct pci_dev dev = pci_get_dev(bus, device, function);
                
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;
                else {

                	num_devices++;

                	printf("PCI Device Info: Bus:%x, Device:%x, Function:%x, Vendor:%x, Device ID:%x", 
                		bus, device, function, dev.vendor_id, dev.device_id);

                	for(int barNum = 0; barNum < 6; barNum++) {

	                    struct pci_bar bar = get_bar(bus, device, function, barNum);
	                    if(bar.address && (bar.type == InputOutput)) {

	                    	printf(", PCI Device Address:%x\n", bar.address);

	                        dev.portBase = (uint32_t)bar.address;
	                    }
                	}

                	printf("\n");


                }


            }
        }
    }
}


void pci_write_cmd_port(uint32_t cmd) {

	outl(cmd, PCI_CMD_PORT);
}

void pci_write_data_port(uint32_t data) {

	outl(data, PCI_DATA_PORT);
}

uint32_t pci_read_data_port() {

	return inl(PCI_DATA_PORT);
}

uint32_t pci_read_dev_register(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);

    pci_write_cmd_port(id);
    uint32_t result = pci_read_data_port();
    return result >> (8* (registeroffset % 4));
}

uint32_t pci_write_dev_register(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);

    pci_write_cmd_port(id);
    pci_write_data_port(value);
}

bool pci_dev_has_functions(uint16_t bus, uint16_t device)
{
    return pci_read_dev_register(bus, device, 0, 0x0E) & (1<<7);
}

struct pci_dev pci_get_dev(uint16_t bus, uint16_t device, uint16_t function)
{
    struct pci_dev result;
    
    result.bus = bus;
    result.device = device;
    result.function = function;
    
    result.vendor_id = pci_read_dev_register(bus, device, function, 0x00);
    result.device_id = pci_read_dev_register(bus, device, function, 0x02);

    result.class_id = pci_read_dev_register(bus, device, function, 0x0b);
    result.subclass_id = pci_read_dev_register(bus, device, function, 0x0a);
    result.interface_id = pci_read_dev_register(bus, device, function, 0x09);

    result.revision = pci_read_dev_register(bus, device, function, 0x08);
    result.interrupt = pci_read_dev_register(bus, device, function, 0x3c);
    
    return result;
}

struct pci_bar get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    struct pci_bar result;

    
    
    uint32_t headertype = pci_read_dev_register(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4*headertype);
    if(bar >= maxBARs)
        return result;
    
    
    uint32_t bar_value = pci_read_dev_register(bus, device, function, 0x10 + 4*bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp;
    
    
    
    if(result.type == MemoryMapping)
    {
        
        switch((bar_value >> 1) & 0x3)
        {
            
            case 0: // 32 Bit Mode
            case 1: // 20 Bit Mode
            case 2: // 64 Bit Mode
                break;
        }
        
    }
    else // InputOutput
    {
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = 0;
    }
    
    return result;
}

uint32_t get_number_of_lowest_set_bit(uint32_t value)
{
  uint32_t pos = 0;
  uint32_t mask = 0x00000001;
  while (!(value & mask))
   { ++pos; mask=mask<<1; }
  return pos;
}

uint32_t get_number_of_highest_set_bit(uint32_t value)
{
  uint32_t pos = 31;
  uint32_t mask = 0x80000000;
  while (!(value & mask))
   { --pos; mask=mask>>1; }
  return pos;
}





