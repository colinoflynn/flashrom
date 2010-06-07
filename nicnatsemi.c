/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2010 Andrew Morgan <ziltro@ziltro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#if defined(__i386__) || defined(__x86_64__)

#include <stdlib.h>
#include "flash.h"

#define PCI_VENDOR_ID_NATSEMI	0x100b

#define BOOT_ROM_ADDR		0x50
#define BOOT_ROM_DATA		0x54

struct pcidev_status nics_natsemi[] = {
	{0x100b, 0x0020, NT, "National Semiconductor", "DP83815/DP83816"},
	{0x100b, 0x0022, NT, "National Semiconductor", "DP83820"},
	{},
};

int nicnatsemi_init(void)
{
	get_io_perms();

	io_base_addr = pcidev_init(PCI_VENDOR_ID_NATSEMI, PCI_BASE_ADDRESS_0,
				   nics_natsemi, programmer_param);

	buses_supported = CHIP_BUSTYPE_PARALLEL;

	return 0;
}

int nicnatsemi_shutdown(void)
{
	free(programmer_param);
	pci_cleanup(pacc);
	release_io_perms();
	return 0;
}

void nicnatsemi_chip_writeb(uint8_t val, chipaddr addr)
{
	OUTL((uint32_t)addr & 0x0000FFFF, io_base_addr + BOOT_ROM_ADDR);
	/*
	 * The datasheet requires 32 bit accesses to this register, but it seems
	 * that requirement might only apply if the register is memory mapped.
	 * Bit 8-31 of this register are apparently don't care, and if this
	 * register is I/O port mapped 8 bit accesses to the lowest byte of the
	 * register seem to work fine. Due to that, we ignore the advice in the
	 * data sheet.
	 */
	OUTB(val, io_base_addr + BOOT_ROM_DATA);
}

uint8_t nicnatsemi_chip_readb(const chipaddr addr)
{
	OUTL(((uint32_t)addr & 0x0000FFFF), io_base_addr + BOOT_ROM_ADDR);
	/*
	 * The datasheet requires 32 bit accesses to this register, but it seems
	 * that requirement might only apply if the register is memory mapped.
	 * Bit 8-31 of this register are apparently don't care, and if this
	 * register is I/O port mapped 8 bit accesses to the lowest byte of the
	 * register seem to work fine. Due to that, we ignore the advice in the
	 * data sheet.
	 */
	return INB(io_base_addr + BOOT_ROM_DATA);
}

#else
#error PCI port I/O access is not supported on this architecture yet.
#endif
