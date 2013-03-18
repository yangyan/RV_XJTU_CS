/*
 * FCRON - periodic command scheduler 
 *
 *  Copyright 2000-2004 Thibault Godouet <fcron@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *  The GNU General Public License can also be found in the file
 *  `LICENSE' that comes with the fcron source distribution.
 */

 /* $Id: fcrondyn.h,v 1.3 2003/12/25 22:42:35 thib Exp thib $ */

#ifndef __FCRONDYN_H__
#define __FCRONDYN_H__

#include "global.h"
#include "dyncom.h"

/* global variables */
extern char debug_opt;
extern char dosyslog;
extern pid_t daemon_pid;


/* types def */
#define MAX_NUM_OPT 4
typedef struct cmd_list_ent {
    char *cmd_name;
    char *cmd_desc;
    int cmd_numopt;
    int cmd_code;
    int cmd_opt[MAX_NUM_OPT];
    int cmd_default[MAX_NUM_OPT];
} cmd_list_ent;

#endif /* __FCRONDYN_H__ */
