/* SPDX-License-Identifier: GPL-2.0 */
#include <asm-generic/vmfreax.lds.h>
#include <asm/vmfreax.lds.h>
#include <asm/thread_info.h>
#include <asm/page.h>
#include <asm/sclp.h>
#include "boot.h"

OUTPUT_FORMAT("elf64-s390", "elf64-s390", "elf64-s390")
OUTPUT_ARCH(s390:64-bit)

ENTRY(startup)

SECTIONS
{
	. = 0;
	.ipldata : {
		*(.ipldata)
	}
	. = IPL_START;
	.head.text : {
		_head = . ;
		HEAD_TEXT
		_ehead = . ;
	}
	. = PARMAREA;
	.parmarea : {
		*(.parmarea)
	}
	.text :	{
		_text = .;	/* Text */
		*(.text)
		*(.text.*)
		_etext = . ;
	}
	.rodata : {
		_rodata = . ;
		*(.rodata)	 /* read-only data */
		*(.rodata.*)
		_erodata = . ;
	}
	NOTES
	.data :	{
		_data = . ;
		*(.data)
		*(.data.*)
		_edata = . ;
	}

	BOOT_DATA
	BOOT_DATA_PRESERVED

	/*
	 * This is the BSS section of the decompressor and not of the decompressed freax kernel.
	 * It will consume place in the decompressor's image.
	 */
	. = ALIGN(8);
	.bss : {
		_bss = . ;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		/*
		 * Stacks for the decompressor
		 */
		. = ALIGN(PAGE_SIZE);
		_dump_info_stack_start = .;
		. += PAGE_SIZE;
		_dump_info_stack_end = .;
		. = ALIGN(PAGE_SIZE);
		_stack_start = .;
		. += BOOT_STACK_SIZE;
		_stack_end = .;
		_ebss = .;
	}

	/*
	 * uncompressed image info used by the decompressor it should match
	 * struct vmfreax_info. It comes from .vmfreax.info section of
	 * uncompressed vmfreax in a form of info.o
	 */
	. = ALIGN(8);
	.vmfreax.info : {
		_vmfreax_info = .;
		*(.vmfreax.info)
	}

	.decompressor.syms : {
		. += 1; /* make sure we have \0 before the first entry */
		. = ALIGN(2);
		_decompressor_syms_start = .;
		*(.decompressor.syms)
		_decompressor_syms_end = .;
	}

	_decompressor_end = .;

#ifdef CONFIG_KERNEL_UNCOMPRESSED
	. = 0x100000;
#else
	. = ALIGN(8);
#endif
	.rodata.compressed : {
		_compressed_start = .;
		*(.vmfreax.bin.compressed)
		_compressed_end = .;
	}

#define SB_TRAILER_SIZE 32
	/* Trailer needed for Secure Boot */
	. += SB_TRAILER_SIZE; /* make sure .sb.trailer does not overwrite the previous section */
	. = ALIGN(4096) - SB_TRAILER_SIZE;
	.sb.trailer : {
		QUAD(0)
		QUAD(0)
		QUAD(0)
		QUAD(0x000000207a49504c)
	}
	_end = .;

	/* Sections to be discarded */
	/DISCARD/ : {
		*(.eh_frame)
		*(__ex_table)
		*(*__ksymtab*)
		*(___kcrctab*)
	}
}
