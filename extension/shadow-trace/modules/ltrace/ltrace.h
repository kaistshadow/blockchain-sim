/*
 * This file is part of ltrace.
 * Copyright (C) 2009 Juan Cespedes
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

#ifndef LTRACE_H
#define LTRACE_H
#include "library.h"

enum Event_type {
	EVENT_NONE=0,
	EVENT_SIGNAL,
	EVENT_EXIT,
	EVENT_EXIT_SIGNAL,
	EVENT_SYSCALL,
	EVENT_SYSRET,
	EVENT_ARCH_SYSCALL,
	EVENT_ARCH_SYSRET,
	EVENT_CLONE,
	EVENT_VFORK,
	EVENT_EXEC,
	EVENT_BREAKPOINT,
	EVENT_LIBCALL,
	EVENT_LIBRET,
	EVENT_NEW,        /* in this case, proc is NULL */
	EVEMT_LIB_ADD,
	EVENT_MAX,
};
typedef enum Event_type Event_type;

typedef struct Event Event;
struct Event {
	struct Event * next;
	struct process *proc;
	Event_type type;
	union {
		int ret_val;     /* EVENT_EXIT */
		int signum;      /* EVENT_SIGNAL, EVENT_EXIT_SIGNAL */
		int sysnum;      /* EVENT_SYSCALL, EVENT_SYSRET */
		void * brk_addr; /* EVENT_BREAKPOINT */
		int newpid;      /* EVENT_CLONE, EVENT_NEW */
	} e_un;
};

typedef void (*callback_func) (Event *);
typedef int (*handler_library) (char *);

extern void ltrace_init(int argc, char **argv);
extern void ltrace_add_callback(callback_func f, Event_type type);
extern void ltrace_library_add_handler(handler_library f);
extern int library_load_handler(char *);
extern void ltrace_main(void);

#endif /* LTRACE_H */
