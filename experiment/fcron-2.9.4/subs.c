
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

 /* $Id: subs.c,v 1.20 2003/12/25 22:52:48 thib Exp thib $ */

#include "global.h"
#include "subs.h"

void init_conf(void);

extern char debug_opt;

/* fcron.conf parameters */
char *fcronconf = NULL;
char *fcrontabs = NULL;
char *pidfile = NULL;
char *fifofile = NULL;
char *fcronallow = NULL;
char *fcrondeny = NULL;
char *shell = NULL;
char *sendmail = NULL;
char *editor = NULL;

int
remove_blanks(char *str)
    /* remove blanks at the the end of str */
    /* return the length of the new string */
{
    char *c = str;

    /* scan forward to the null */
    while (*c)
	c++;

    /* scan backward to the first character that is not a space */
    do	{c--;}
    while (c >= str && isspace( (int) *c));

    /* if last char is a '\n', we remove it */
    if ( *c == '\n' )
	*c = '\0';
    else
	/* one character beyond where we stopped above is where the null
	 * goes. */
	*++c = '\0';

    /* return the new length */
    return ( c - str );
    
}


char *
strdup2(const char *str)
{
    char *ptr = (char *)malloc(strlen(str) + 1);

    if ( ! ptr)
	die_e("Could not calloc");

    strcpy(ptr, str);
    return(ptr);
}


int
get_word(char **str)
    /* make str point the next word and return word length */
{
    char *ptr;

    Skip_blanks(*str);
    ptr = *str;

    while ( (isalnum( (int) *ptr) || *ptr == '_' || *ptr == '-') 
	    && *ptr != '=' && ! isspace( (int) *ptr) )
	ptr++;
    
    return (ptr - *str);
}

void
init_conf(void)
/* initialises config with compiled in constants */
{
    /* set fcronconf if cmd line option -c has not been used */
    if (fcronconf == NULL)
	fcronconf = strdup2(ETC "/" FCRON_CONF);
    fcrontabs = strdup2(FCRONTABS);
    pidfile = strdup2(PIDFILE);
    fifofile = strdup2(FIFOFILE);
    fcronallow = strdup2(ETC "/" FCRON_ALLOW);
    fcrondeny = strdup2(ETC "/" FCRON_DENY);
    shell = strdup2(SHELL);
#ifdef SENDMAIL
    sendmail = strdup2(SENDMAIL);
#endif
    editor = strdup2(EDITOR);
}


void
read_conf(void)
/* reads in a config file and updates the necessary global variables */
{
    FILE *f = NULL;
    struct stat st;
    char buf[LINE_LEN];
    char *ptr1 = NULL, *ptr2 = NULL;
    short namesize = 0;
    char err_on_enoent = 0;

    if (fcronconf != NULL)
	/* fcronconf has been set by -c option : file must exist */
	err_on_enoent = 1;
	
    init_conf();

    if ( (f = fopen(fcronconf, "r")) == NULL ) {
	if ( errno == ENOENT ) {
	    if ( err_on_enoent )
		die_e("Could not read %s", fcronconf);
	    else
		/* file does not exist, it is not an error  */
		return;
	}
	else {
	    error_e("Could not read %s : config file ignored", fcronconf);
	    return;
	}
    }

    /* check if the file is secure : owned and writable only by root */
    if ( fstat(fileno(f), &st) != 0 || st.st_uid != ROOTUID
	 || st.st_mode & S_IWGRP || st.st_mode & S_IWOTH ) {
	error("Conf file (%s) must be owned by root and (no more than) 644 : "
	      "ignored", fcronconf);
	fclose(f);
	return;
    }
    
    while ( (ptr1 = fgets(buf, sizeof(buf), f)) != NULL ) {

	Skip_blanks(ptr1); /* at the beginning of the line */
	
	/* ignore comments and blank lines */
	if ( *ptr1 == '#' || *ptr1 == '\n' || *ptr1 == '\0')
	    continue;

	remove_blanks(ptr1); /* at the end of the line */

	/* get the name of the var */
	if ( ( namesize = get_word(&ptr1) ) == 0 )
	    /* name is zero-length */
	    error("Zero-length var name at line %s : line ignored", buf);

	ptr2 = ptr1 + namesize;

	/* skip the blanks and the "=" and go to the value */
	while ( isspace( (int) *ptr2 ) ) ptr2++;
	if ( *ptr2 == '=' ) ptr2++;
	while ( isspace( (int) *ptr2 ) ) ptr2++;

	/* find which var the line refers to and update it */
	if ( strncmp(ptr1, "fcrontabs", namesize) == 0 )
	    fcrontabs = strdup2(ptr2);
	else if ( strncmp(ptr1, "pidfile", namesize) == 0 )
	    pidfile = strdup2(ptr2);
	else if ( strncmp(ptr1, "fifofile", namesize) == 0 )
	    fifofile = strdup2(ptr2);
	else if ( strncmp(ptr1, "fcronallow", namesize) == 0 )
	    fcronallow = strdup2(ptr2);
	else if ( strncmp(ptr1, "fcrondeny", namesize) == 0 )
	    fcrondeny = strdup2(ptr2);
	else if ( strncmp(ptr1, "shell", namesize) == 0 )
	    shell = strdup2(ptr2);
	else if ( strncmp(ptr1, "sendmail", namesize) == 0 )
	    sendmail = strdup2(ptr2);
	else if ( strncmp(ptr1, "editor", namesize) == 0 )
	    editor = strdup2(ptr2);
	else
	    error("Unknown var name at line %s : line ignored", buf);

    }

    if (debug_opt) {
	debug("  fcronconf=%s", fcronconf);
/*  	debug("  fcronallow=%s", fcronallow); */
/*  	debug("  fcrondeny=%s", fcrondeny); */
/*  	debug("  fcrontabs=%s", fcrontabs); */
/*  	debug("  pidfile=%s", pidfile); */
/*   	debug("  fifofile=%s", fifofile); */
/*  	debug("  editor=%s", editor); */
/*  	debug("  shell=%s", shell); */
/*  	debug("  sendmail=%s", sendmail); */
    }

    fclose(f);

}
