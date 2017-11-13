#include <mem/mem_internal.h>
#include <sys/sys.h>

extern void *grub_struct;
extern void *kernel_end;

struct mmap_struct
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_reg
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry{
    uint16_t offset_low;    // offset bits 0..15
    uint16_t selector;      // a code segment selector in GDT or LDT
    uint8_t zero;           // unused, set to 0
    uint8_t type_attr;      // type and attributes, see below
    uint16_t offset_high;   // offset bits 16..31
} __attribute__((packed));

static struct gdt_reg 		GDTR;
static uint16_t				gdt_count;
static struct gdt_entry     local_GDT[5];   // Default GDT created by GRUB has 5 entries (40 bytes)

// Public variable
uint16_t                    mem_code_selector;

static struct gdt_reg 		IDTR;
static struct idt_entry 	IDT[256];

struct gdt_reg *get_gdtr()
{
	asm(
        "sgdt %0\n"
        : : "m" (GDTR)
    );

	return &GDTR;
}

void set_gdtr()
{
    asm(
        "lgdt %0\n"
        : : "m" (GDTR)
        );
}

void setup_gdt()
{
	// Get the current GDT (set by GRUB)
	get_gdtr();
    
    if (GDTR.limit + 1 > 40)
    {
        core_fatal("GDT is bigger than local buffer");
    }
    
    // Create a local copy and update GDTR
    struct gdt_entry *org_GDT = (void *)GDTR.base;
    
    mem_code_selector = 0;
    
    for (int i = 0 ; i < GDTR.limit + 1 ; i++)
    {
        local_GDT[i] = org_GDT[i];
        
        // Found a data segment in the GDT
        if (local_GDT[i].access == 0x9A)
        {
            mem_code_selector = i;
        }
    }
    
    if (mem_code_selector == 0)
    {
        core_fatal("No data segment!");
    }
    
    GDTR.base = (uint32_t)local_GDT;
    gdt_count = (GDTR.limit + 1) / 8;
    
    set_gdtr();
}

void setup_idt()
{
	IDTR.base = (uint32_t)IDT;
	IDTR.limit = 256 * 8 - 1;

	asm(
        "lidt %0\n"
        : "=m" (IDTR)
    );

	for (int vector = 0 ; vector < 256 ; vector++)
	{
		IDT[vector].zero = 0;
		IDT[vector].selector = 0;
		IDT[vector].type_attr = 0;
		IDT[vector].offset_low = 0;
		IDT[vector].offset_high = 0;
	}
}

void set_isr(void *isr, unsigned char vector)
{
    IDT[vector].zero = 0;
    IDT[vector].selector = mem_code_selector * 8;
    // P=1, DPL=00, S=0, type=1110
    IDT[vector].type_attr = 0b10001110;
    IDT[vector].offset_low = (uint32_t)isr & 0xFFFF;
    IDT[vector].offset_high = ((uint32_t)isr >> 16) & 0xFFFF;
}

int mem_scan_blocks(struct BlockStruct *blockArray)
{
    uint32_t grub_flags = *((uint32_t *)grub_struct);
    
    // mmap is present on Multiboot struct
    if (grub_flags & (1 << 6))
    {
        // WARNING: offsets (44 and 48) are dynamic and depend on optional fields:
        // https://www.gnu.org/software/grub/manual/multiboot/html_node/Boot-information-format.html#Boot-information-format
        // We need to check for multiboot flags, fields present and calculate the offsets
        
        uint32_t mmap_length = *((uint32_t *)(grub_struct + 44));
        void **p = grub_struct + 48;
        void *mmap_address = *p;
        
        struct mmap_struct *mmap_unit = mmap_address;
        
        unsigned int buf_position = 0;
        
        while (buf_position < mmap_length)
        {
            // RAM block
            if (mmap_unit->type == 1)
            {
                // Discard conventional memory, only use RAM over the first MB
                if (mmap_unit->base_addr > 0)
                {
                    uint32_t kernelSize = (uint32_t)(kernel_end - mmap_unit->base_addr);
                    
                    blockArray[0].block = kernel_end;
                    blockArray[0].blockSize = mmap_unit->length - kernelSize;
                    
                    return 1;
                }
            }
            
            // Go to next struct
            unsigned int currSize = mmap_unit->size;
            mmap_unit = ((void *)(mmap_unit)) + mmap_unit->size + 4;
            buf_position += currSize + 4;
        }
    }
    
    // No memory found, we should probably abort here, but let's do it in the arch independant part
    return 0;
}

void mem_internal_setup()
{
    setup_gdt();
    setup_idt();
}
