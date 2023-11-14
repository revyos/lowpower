/*
 * SPDX-License-Identifier: BSD-2-Clause
 */
MEMORY { .str_mem : ORIGIN = 0xffe0000000, LENGTH = 0x1000 }

OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
	/* Beginning of the code section */
	.text : {
		. = ALIGN(4);
		PROVIDE(_text_start = .);
		*(.entry)
		*start.S.o(*.text)
		*(.text)
		*(.text*)
		*(.text.*)
		. = ALIGN(4);
		PROVIDE(_text_end = .);
	} > .str_mem
	. = ALIGN(4);
	/* End of the code sections */

	/* Beginning of the read-only data sections */
	. = ALIGN(4);
	.rodata :
	{
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		. = ALIGN(8);
		PROVIDE(_rodata_end = .);
	} > .str_mem
	/* End of the read-only data sections */

	/* Beginning of the read-write data sections */
	. = ALIGN(4);
	.data :
	{
		PROVIDE(_data_start = .);

		*(.sdata)
		*(.sdata.*)
		*(.data)
		*(.data.*)
		*(.readmostly.data)
		*(*.data)
		. = ALIGN(8);

		PROVIDE(_data_end = .);
	} > .str_mem

	.dynsym : {
		PROVIDE(__dyn_sym_start = .);
		*(.dynsym)
		PROVIDE(__dyn_sym_end = .);
	} > .str_mem

	.rela.dyn : {
		PROVIDE(__rel_dyn_start = .);
		*(.rela*)
		. = ALIGN(8);
		PROVIDE(__rel_dyn_end = .);
	} > .str_mem

	. = ALIGN(4);

	.bss : {
		PROVIDE(_bss_start = .);
		*(.sbss)
		*(.sbss.*)
		*(.bss)
		*(.bss.*)
		. = ALIGN(8);
		PROVIDE(_bss_end = .);
	} > .str_mem
	/* End of the read-write data sections */

	. = ALIGN(16);
	. += 64;
	
	/* Beginning of the stack */
	.stack : {
		PROVIDE(_stack_start = .);
		*(.stack*)
		PROVIDE(_stack_end = .);
	} > .str_mem
	/* end of the stack */
	. = ALIGN(0x1000); /* Need this to create proper sections */

	_end = .;

	PROVIDE(_fw_end = .);
}
