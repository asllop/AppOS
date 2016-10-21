#include <mem/mem_internal.h>

extern void *grub_struct;
extern void *kernel_end;

struct mmap_struct
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));;

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
    uint16_t offset_low; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_high; // offset bits 16..31
} __attribute__((packed));

static struct gdt_reg 		GDTR;
static struct gdt_entry 	*GDT;
static uint16_t				gdt_count;

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

void setup_gdt()
{
	// Get the current GDT (set by GRUB)
	get_gdtr();
	GDT = (void *)GDTR.base;
	gdt_count = (GDTR.limit + 1) / 8;
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
     // Assumes that GDT[1] has the CS and it contains the whole addressable memory and starts on 0 (normal GRUB config)
    IDT[vector].selector = 8;
    // P=1, DPL=00, S=0, type=1110
    IDT[vector].type_attr = 0b10001110;
    IDT[vector].offset_low = (uint32_t)isr & 0xFFFF;
    IDT[vector].offset_high = ((uint32_t)isr >> 16) & 0xFFFF;
}

int scan_blocks(struct BlockStruct *blockArray)
{
    uint32_t grub_flags = *((uint32_t *)grub_struct);
    
    // mmap is present on Multiboot struct
    if (grub_flags & (1 << 6))
    {
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
                    // NOTE: Part of the HiMem is being used by the kernel code, we must reserve this
                    //uint32_t margin = 100*1024;
                    uint32_t margin = 0;
                    uint32_t kernelSize = (uint32_t)(kernel_end - mmap_unit->base_addr);
                    
                    blockArray[0].block = kernel_end + margin;
                    blockArray[0].totalSize = mmap_unit->length - kernelSize - margin;
                    blockArray[0].usedSize = 0;
                    blockArray[0].first = NULL;
                    blockArray[0].last = NULL;
                    
                    return 1;
                }
            }
            
            // Go to next struct
            unsigned int currSize = mmap_unit->size;
            mmap_unit = ((void *)(mmap_unit)) + mmap_unit->size + 4;
            buf_position += currSize + 4;
        }
    }
    
    // No memory found, we should probably abort here, but let it do at the arch independant part
    return 0;
}

void setup_mem()
{
    setup_gdt();
    setup_idt();
}
