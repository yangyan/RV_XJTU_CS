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

 /* $Id: global.h,v 1.41 2003/12/25 22:43:53 thib Exp thib $ */


/* 
   WARNING : this file should not be modified.
   Compilation's options are in config.h
*/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/* config.h must be included before every other includes 
 * (contains the compilation options) */
#include "config.h"


#include <ctype.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef CONFIG_FLASK
#include <flask_util.h>
#include <fs_secure.h>
#include <ss.h>
#include <linux/flask/av_permissions.h>
#include <get_sid_list.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <pwd.h>
#include <signal.h>

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#elif HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_TERMIOS_H
#include <sys/termios.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#elif HAVE_SYS_FCNTL_H
#include <sys/fcntl.h>
#endif

#ifdef HAVE_LIBPAM
#include "pam.h"
#endif

#include "bitstring.h"     /* bit arrays */
#include "option.h"        /* manage fcrontab's options */

/* you should not change this (nor need to do it) */
#define ERR     -1           
#define OK       0

/* options for local functions */
#define STD 0

/* macros */
#define Alloc(PTR, TYPE) \
        if( (PTR = (TYPE *)calloc(1, sizeof(TYPE))) == NULL ) \
            die_e("Could not calloc.");

#define Set(VAR, VALUE) \
        { \
          free(VAR); \
          VAR = strdup2(VALUE); \
        }

#define Flush(VAR) \
        { \
          free(VAR); \
          VAR = NULL; \
	}

#define Skip_blanks(PTR) \
        while((*(PTR) == ' ') || (*(PTR) == '\t')) \
	    (PTR)++;

#define Overwrite(x) \
        do {                     \
          register char *__xx__; \
          if ((__xx__=(x)))      \
            while (*__xx__)      \
              *__xx__++ = '\0';  \
        } while (0)


#define debug if(debug_opt) Debug

typedef struct env_t {
    char         *e_val;        /* env value                            */
    struct env_t *e_next;
} env_t ;

typedef struct cf_t {
    struct cf_t  *cf_next;
    struct cl_t  *cf_line_base;
    char	 *cf_user;	/* user-name			             */
    struct env_t *cf_env_base;  /* list of all env variables to set          */
    int		  cf_running;	/* number of jobs running                    */
    signed char	  cf_tzdiff;    /* time diff between system and local hour   */
#ifdef CONFIG_FLASK
    security_id_t cf_user_sid;
    security_id_t cf_file_sid;
#endif
} cf_t;


#define OPTION_SIZE 4
#define LAVG_SIZE 3
/* warning : do not change the order of the members of this structure
 *   because some tests made are dependent to that order */
/* warning : if you change a field type, you may have to also make some changes
 *   in the save/load binary fcrontab functions */
typedef struct cl_t {
    struct cl_t   *cl_next;
    struct cf_t   *cl_file;       /* the file in which the line is           */
    char	  *cl_shell;      /* shell command			     */
    char          *cl_runas;      /* determine permissions of the job        */
    char          *cl_mailto;     /* mail output to cl_mailto                */
    long int       cl_id;         /* line's unique id number                 */
    time_t         cl_until;      /* timeout of the wait for a lavg value    */
    time_t         cl_first;      /* initial delay preserved for volatile entries */
    time_t         cl_nextexe;    /* time and date of the next execution     */
    long int       cl_timefreq;   /* Run every n seconds                     */
    unsigned short cl_remain;     /* remaining until next execution          */
    unsigned short cl_runfreq;    /* Run once every n matches                */
    unsigned char  cl_option[OPTION_SIZE]; /* line's option (see option.h)   */
    unsigned char  cl_lavg[LAVG_SIZE];/*load averages needed (1,5,15 mins)   */
    unsigned char  cl_numexe;     /* entries in queues & running processes   */
    char           cl_nice;       /* nice value to control priority          */
    /* see bitstring(3) man page for more details */
    bitstr_t	   bit_decl(cl_mins, 60); /* 0-59		             */
    bitstr_t	   bit_decl(cl_hrs, 24);  /* 0-23			     */
    bitstr_t	   bit_decl(cl_days, 32); /* 1-31			     */
    bitstr_t	   bit_decl(cl_mons, 12); /* 0-11                	     */
    bitstr_t	   bit_decl(cl_dow, 8);	  /* 0-7, 0 and 7 are both Sunday    */
} cl_t;

typedef struct job_t {
    struct cl_t  *j_line;
    struct job_t   *j_next;
} job_t;

typedef struct lavg_t {
    struct cl_t *l_line;  
    time_t       l_until;   /* the timeout of the wait for load averages */
} lavg_t;

typedef struct exe_t {
    struct cl_t *e_line;
    pid_t        e_ctrl_pid; /* pid of the fcron process controling the job */
    pid_t        e_job_pid;  /* pid of the job itself */
} exe_t;


/* local header files : we include here the headers which may use some types defined
 *                      above. */

/* constants for fcrontabs needed to load and save the fcrontabs to disk */
#include "save.h"
/* log part */
#include "log.h"
/* functions used by fcrontab, fcrondyn, and fcron */
#include "subs.h"


#endif /* __GLOBAL_H__ */

