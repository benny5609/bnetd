/*
 * Copyright (C) 1999  Ross Combs (rocombs@cs.nmsu.edu)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef INCLUDED_TIMER_TYPES
#define INCLUDED_TIMER_TYPES

#ifdef JUST_NEED_TYPES
# ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
# else
#  if HAVE_SYS_TIME_H
#   include <sys/time.h>
#  else
#   include <time.h>
#  endif
# endif
# include "connection.h"
#else
# define JUST_NEED_TYPES
# ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
# else
#  if HAVE_SYS_TIME_H
#   include <sys/time.h>
#  else
#   include <time.h>
#  endif
# endif
# include "connection.h"
# undef JUST_NEED_TYPES
#endif

typedef union
{
    unsigned long n;
    void *        p;
} t_timer_data;

typedef void (* t_timer_cb)(t_connection * owner, time_t when, t_timer_data data);

#ifdef TIMER_INTERNAL_ACCESS
typedef struct
{
    t_connection * owner; /* who to notify */
    time_t         when;  /* when the timer expires */
    t_timer_cb     cb;    /* what to call */
    t_timer_data   data;  /* data argument */
} t_timer;
#endif

#endif

#ifndef JUST_NEED_TYPES
#ifndef INCLUDED_TIMER_PROTOS
#define INCLUDED_TIMER_PROTOS

#define JUST_NEED_TYPES
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include "connection.h"
#undef JUST_NEED_TYPES

extern int timerlist_create(void);
extern int timerlist_destroy(void);
extern int timerlist_add_timer(t_connection * owner, time_t when, t_timer_cb cb, t_timer_data data);
extern int timerlist_del_all_timers(t_connection * owner);
extern int timerlist_check_timers(time_t when);

#endif
#endif
