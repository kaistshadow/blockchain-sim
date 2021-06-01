/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2008,2009 Juan Cespedes
 * Copyright (C) 2006 Eric Vaitl, Cisco Systems, Inc.
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

#include "config.h"

#include <stddef.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#include "proc.h"
#include "common.h"
#include "mips.h"

#if (!defined(PTRACE_PEEKUSER) && defined(PTRACE_PEEKUSR))
# define PTRACE_PEEKUSER PTRACE_PEEKUSR
#endif

#if (!defined(PTRACE_POKEUSER) && defined(PTRACE_POKEUSR))
# define PTRACE_POKEUSER PTRACE_POKEUSR
#endif

/**
   \addtogroup mips
   @{
 */


/**
   \param proc The process to work on.
   \return The current instruction pointer.
 */
void *
get_instruction_pointer(struct process *proc)
{
	return (void *)ptrace(PTRACE_PEEKUSER, proc->pid, off_pc, 0);
}

/**
   \param proc The process to work on.
   \param addr The address to set to.

   Called by \c continue_after_breakpoint().

   \todo Our mips kernel ptrace doesn't support PTRACE_SINGLESTEP, so
   we \c continue_process() after a breakpoint. Check if this is OK.
 */
void
set_instruction_pointer(struct process *proc, void *addr)
{
	ptrace(PTRACE_POKEUSER, proc->pid, off_pc, addr);
}

/**
   \param proc The process to work on.
   \return The current stack pointer.
 */
void *
get_stack_pointer(struct process *proc)
{
	return (void *)ptrace(PTRACE_PEEKUSER, proc->pid, off_sp, 0);
}

/**
   \param proc The process to work on.
   \param stack_pointer The current stack pointer for proc
   \return The current return address.

   Called by \c handle_breakpoint().

   Mips uses r31 for the return address, so the stack_pointer is
   unused.
 */
void *
get_return_addr(struct process *proc, void *stack_pointer)
{
	return (void *)ptrace(PTRACE_PEEKUSER, proc->pid, off_lr, 0);
}
