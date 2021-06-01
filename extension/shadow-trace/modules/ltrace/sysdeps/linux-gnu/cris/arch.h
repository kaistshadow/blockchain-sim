/*
 * This file is part of ltrace.
 * Copyright (C) 2010 Edgar E. Iglesias
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#define BREAKPOINT_VALUE { 0x38, 0xe9 }
#define BREAKPOINT_LENGTH 2
#define DECR_PC_AFTER_BREAK 0
#define ARCH_ENDIAN_LITTLE

#define LT_ELFCLASS	ELFCLASS32
#define LT_ELF_MACHINE	EM_CRIS
