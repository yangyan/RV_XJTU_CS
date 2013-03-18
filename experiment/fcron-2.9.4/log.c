
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

 /* $Id: log.c,v 1.14 2003/12/25 22:41:03 thib Exp thib $ */

/* This code is inspired by Anacron's sources of
   Itai Tzur <itzur@actcom.co.il> */


#include "fcron.h"

#include "log.h"

static void xopenlog(void);

static char truncated[] = " (truncated)";
static int log_open=0;


static void
xopenlog(void)
{
    if (!log_open) {
	openlog(prog_name, LOG_PID, SYSLOG_FACILITY);
	log_open=1;
    }
}


void
xcloselog()
{
    if (log_open) closelog();
    log_open = 0;
}


/* Construct the message string from its parts */
char *
make_msg(char *fmt, va_list args)
{
    int len;
    char *msg = NULL;
   
    if ( (msg = (char *)calloc(1, MAX_MSG + 1)) == NULL )
	return NULL;
    /* There's some confusion in the documentation about what vsnprintf
     * returns when the buffer overflows.  Hmmm... */
    len = vsnprintf(msg, MAX_MSG + 1, fmt, args);
    if (len >= MAX_MSG)
	strcpy(msg + (MAX_MSG - 1) - sizeof(truncated), truncated);

    return msg;
}


/* Log a message, described by "fmt" and "args", with the specified 
 * "priority". */
static void
log(int priority, char *fmt, va_list args)
{
    char *msg;

    if ( (msg = make_msg(fmt, args)) == NULL)
	return;

    if (dosyslog) {
	xopenlog();
	syslog(priority, "%s", msg);
    }

    if (foreground == 1) {
	time_t t = time(NULL);
	struct tm *ft;
	char date[30];

	ft = localtime(&t);
	date[0] = '\0';
	strftime(date, sizeof(date), "%H:%M:%S", ft);
	fprintf(stderr, "%s %s\n", date, msg);

    }

    free(msg);
}


/* Same as log(), but also appends an error description corresponding
 * to "errno". */
static void
log_e(int priority, char *fmt, va_list args)
{
    int saved_errno;
    char *msg;

    saved_errno=errno;

    if ( (msg = make_msg(fmt, args)) == NULL )
	return ;

    if ( dosyslog ) {
	xopenlog();
	syslog(priority, "%s: %s", msg, strerror(saved_errno));
    }

    if (foreground == 1) {
	time_t t = time(NULL);
	struct tm *ft;
	char date[30];

	ft = localtime(&t);
	date[0] = '\0';
	strftime(date, sizeof(date), "%H:%M:%S", ft);
	fprintf(stderr, "%s %s: %s\n", date, msg, strerror(saved_errno));
    }
}


#ifdef HAVE_LIBPAM
/* Same as log(), but also appends an error description corresponding
 * to the pam_error. */
static void
log_pame(int priority, pam_handle_t *pamh, int pamerrno, char *fmt, va_list args)
{
    char *msg;

    if ( (msg = make_msg(fmt, args)) == NULL )
        return ;

    xopenlog();
    syslog(priority, "%s: %s", msg, pam_strerror(pamh, pamerrno));

    if (foreground == 1) {
        time_t t = time(NULL);
        struct tm *ft;
        char date[30];

        ft = localtime(&t);
        date[0] = '\0';
        strftime(date, sizeof(date), "%H:%M:%S", ft);
        fprintf(stderr, "%s %s: %s\n", date, msg, pam_strerror(pamh, pamerrno));
    }
    xcloselog();
}
#endif


/* Log an "explain" level message */
void
explain(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log(EXPLAIN_LEVEL, fmt, args);
    va_end(args);
}


/* Log an "explain" level message, with an error description */
void
explain_e(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_e(EXPLAIN_LEVEL, fmt, args);
    va_end(args);
}


/* Log a "warning" level message */
void
warn(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log(WARNING_LEVEL, fmt, args);
    va_end(args);
}


/* Log a "warning" level message, with an error description */
void
warn_e(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_e(WARNING_LEVEL, fmt, args);
    va_end(args);
}


/* Log a "complain" level message */
void
error(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log(COMPLAIN_LEVEL, fmt, args);
    va_end(args);
}


/* Log a "complain" level message, with an error description */
void
error_e(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_e(COMPLAIN_LEVEL, fmt, args);
    va_end(args);
}


#ifdef HAVE_LIBPAM
/* Log a "complain" level message, with a PAM error description */
void
error_pame(pam_handle_t *pamh, int pamerrno, char *fmt, ...)
{
    va_list args;

    xcloselog();  /* PAM is likely to have used openlog() */

    va_start(args, fmt);
    log_pame(COMPLAIN_LEVEL, pamh, pamerrno, fmt, args);
    va_end(args);
}
#endif

/* Log a "complain" level message, and exit */
void
die(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log(COMPLAIN_LEVEL, fmt, args);
    va_end(args);
    if (getpid() == daemon_pid) error("Aborted");

    exit(EXIT_ERR);

}  


/* Log a "complain" level message, with an error description, and exit */
void
die_e(char *fmt, ...)
{
   va_list args;
   int err_no = 0;

   err_no = errno;

   va_start(args, fmt);
   log_e(COMPLAIN_LEVEL, fmt, args);
   va_end(args);
   if (getpid() == daemon_pid) error("Aborted");

   exit(err_no);

}  

#ifdef HAVE_LIBPAM
/* Log a "complain" level message, with a PAM error description, and exit */
void
die_pame(pam_handle_t *pamh, int pamerrno, char *fmt, ...)
{
    va_list args;

    xcloselog();  /* PAM is likely to have used openlog() */

    va_start(args, fmt);
    log_pame(COMPLAIN_LEVEL, pamh, pamerrno, fmt, args);
    va_end(args);
    pam_end(pamh, pamerrno);  
    if (getpid() == daemon_pid) error("Aborted");

    exit(EXIT_ERR);

}
#endif

void
Debug(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log(DEBUG_LEVEL, fmt, args);
    va_end(args);
}


