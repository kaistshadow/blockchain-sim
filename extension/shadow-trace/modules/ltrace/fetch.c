/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012 Petr Machata, Red Hat Inc.
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

#include <stdlib.h>
#include <string.h>

#include "fetch.h"
#include "sysdep.h"
#include "value.h"
#include "type.h"
#include "value_dict.h"
#include "prototype.h"
#ifdef ARCH_HAVE_FETCH_ARG
struct fetch_context *arch_fetch_arg_init(enum tof type, struct process *proc,
										  struct arg_type_info *ret_info);

struct fetch_context *arch_fetch_arg_clone(struct process *proc,
										   struct fetch_context *context);

int arch_fetch_arg_next(struct fetch_context *ctx, enum tof type,
						struct process *proc, struct arg_type_info *info,
						struct value *valuep);

int arch_fetch_retval(struct fetch_context *ctx, enum tof type,
					  struct process *proc, struct arg_type_info *info,
					  struct value *valuep);

void arch_fetch_arg_done(struct fetch_context *context);

#ifdef ARCH_HAVE_FETCH_PACK
int arch_fetch_param_pack_start(struct fetch_context *context,
								enum param_pack_flavor ppflavor);

void arch_fetch_param_pack_end(struct fetch_context *context);
#endif

#else
/* Fall back to gimme_arg.  */

long gimme_arg(enum tof type, struct process *proc, int arg_num,
			   struct arg_type_info *info);

struct fetch_context
{
	int argnum;
};

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
					struct arg_type_info *ret_info)
{
	return calloc(sizeof(struct fetch_context), 1);
}

struct fetch_context *
arch_fetch_arg_clone(struct process *proc, struct fetch_context *context)
{
	struct fetch_context *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;
	return memcpy(ret, context, sizeof(*ret));
}

int arch_fetch_arg_next(struct fetch_context *context, enum tof type,
						struct process *proc,
						struct arg_type_info *info, struct value *valuep)
{
	long l = gimme_arg(type, proc, context->argnum++, info);
	value_set_word(valuep, l);
	return 0;
}

int arch_fetch_retval(struct fetch_context *context, enum tof type,
					  struct process *proc,
					  struct arg_type_info *info, struct value *valuep)
{
	long l = gimme_arg(type, proc, -1, info);
	value_set_word(valuep, l);
	return 0;
}

void arch_fetch_arg_done(struct fetch_context *context)
{
	free(context);
}
#endif

#if !defined(ARCH_HAVE_FETCH_ARG) || !defined(ARCH_HAVE_FETCH_PACK)
int arch_fetch_param_pack_start(struct fetch_context *context,
								enum param_pack_flavor ppflavor)
{
	return 0;
}

void arch_fetch_param_pack_end(struct fetch_context *context)
{
}
#endif

struct fetch_context *
fetch_arg_init(enum tof type, struct process *proc,
			   struct arg_type_info *ret_info)
{
	return arch_fetch_arg_init(type, proc, ret_info);
}

struct fetch_context *
fetch_arg_clone(struct process *proc, struct fetch_context *context)
{
	return arch_fetch_arg_clone(proc, context);
}

int fetch_arg_next(struct fetch_context *context, enum tof type,
				   struct process *proc,
				   struct arg_type_info *info, struct value *valuep)
{
	return arch_fetch_arg_next(context, type, proc, info, valuep);
}

int fetch_retval(struct fetch_context *context, enum tof type,
				 struct process *proc,
				 struct arg_type_info *info, struct value *valuep)
{
	return arch_fetch_retval(context, type, proc, info, valuep);
}

void fetch_arg_done(struct fetch_context *context)
{
	return arch_fetch_arg_done(context);
}

int fetch_param_pack_start(struct fetch_context *context,
						   enum param_pack_flavor ppflavor)
{
	return arch_fetch_param_pack_start(context, ppflavor);
}
static int
fetch_simple_param(enum tof type, struct process *proc,
				   struct fetch_context *context,
				   struct value_dict *arguments,
				   struct arg_type_info *info, int own,
				   struct value *valuep)
{
	/* Arrays decay into pointers per C standard.  We check for
	 * this here, because here we also capture arrays that come
	 * from parameter packs.  */
	if (info->type == ARGTYPE_ARRAY)
	{
		struct arg_type_info *tmp = malloc(sizeof(*tmp));
		if (tmp != NULL)
		{
			type_init_pointer(tmp, info, own);
			tmp->lens = info->lens;
			info = tmp;
			own = 1;
		}
	}

	struct value value;
	value_init(&value, proc, NULL, info, own);
	if (fetch_arg_next(context, type, proc, info, &value) < 0)
		return -1;

	if (val_dict_push_next(arguments, &value) < 0)
	{
		value_destroy(&value);
		return -1;
	}

	if (valuep != NULL)
		*valuep = value;

	return 0;
}
static void
fetch_param_stop(struct value_dict *arguments, ssize_t *params_leftp)
{
	if (*params_leftp == -1)
		*params_leftp = val_dict_count(arguments);
}
static int
fetch_param_pack(enum tof type, struct process *proc,
				 struct fetch_context *context,
				 struct value_dict *arguments, struct param *param,
				 ssize_t *params_leftp)
{
	struct param_enum *e = param_pack_init(param, arguments);
	if (e == NULL)
		return -1;

	int ret = 0;
	while (1)
	{
		int insert_stop = 0;
		struct arg_type_info *info = malloc(sizeof(*info));
		if (info == NULL || param_pack_next(param, e, info, &insert_stop) < 0)
		{
		fail:
			free(info);
			ret = -1;
			break;
		}

		if (insert_stop)
			fetch_param_stop(arguments, params_leftp);

		if (info->type == ARGTYPE_VOID)
		{
			type_destroy(info);
			free(info);
			break;
		}

		struct value val;
		if (fetch_simple_param(type, proc, context, arguments,
							   info, 1, &val) < 0)
			goto fail;

		int stop = 0;
		switch (param_pack_stop(param, e, &val))
		{
		case PPCB_ERR:
			goto fail;
		case PPCB_STOP:
			stop = 1;
		case PPCB_CONT:
			break;
		}

		if (stop)
			break;
	}

	param_pack_done(param, e);
	return ret;
}

static int
fetch_one_param(enum tof type, struct process *proc,
				struct fetch_context *context,
				struct value_dict *arguments, struct param *param,
				ssize_t *params_leftp)
{
	switch (param->flavor)
	{
		int rc;
	case PARAM_FLAVOR_TYPE:
		return fetch_simple_param(type, proc, context, arguments,
								  param->u.type.type, 0, NULL);

	case PARAM_FLAVOR_PACK:
		if (fetch_param_pack_start(context,
								   param->u.pack.ppflavor) < 0)
			return -1;
		rc = fetch_param_pack(type, proc, context, arguments,
							  param, params_leftp);
		fetch_param_pack_end(context);
		return rc;

	case PARAM_FLAVOR_STOP:
		fetch_param_stop(arguments, params_leftp);
		return 0;
	}

	assert(!"Invalid param flavor!");
	abort();
}
static enum callback_status
fetch_one_param_cb(struct prototype *proto, struct param *param, void *data)
{
	struct fetch_one_param_data *cb_data = data;
	return CBS_STOP_IF(fetch_one_param(cb_data->tof, cb_data->proc,
									   cb_data->context,
									   cb_data->arguments, param,
									   cb_data->params_leftp) < 0);
}

int fetch_params(enum tof type, struct process *proc,
				 struct fetch_context *context,
				 struct value_dict *arguments, struct prototype *func,
				 ssize_t *params_leftp)
{
	struct fetch_one_param_data cb_data = {proc, context, arguments, params_leftp, type};
	if (prototype_each_param(func, NULL,
							 &fetch_one_param_cb, &cb_data) != NULL)
		return -1;

	/* Implicit stop at the end of parameter list.  */
	fetch_param_stop(arguments, params_leftp);

	return 0;
}
void fetch_param_pack_end(struct fetch_context *context)
{
	return arch_fetch_param_pack_end(context);
}
