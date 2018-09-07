ENTRY(reset_start)
SECTIONS
{
	. = 0x10000;
	.text : { tsBMP.o *( .text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
	.data : { *(image1.o) } /* include image.o as a data section */

	/* stack areas */
	. = ALIGN(8);
   . = . + 0x10000; /* 0x1000 for 4KB; x10000=64kB of stack memory */
   stack_top = .;
}