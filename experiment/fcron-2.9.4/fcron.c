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

 /* $Id: fcron.c,v 1.69 2003/12/25 22:43:18 thib Exp thib $ */

#include "fcron.h"

#include "database.h"
#include "conf.h"
#include "job.h"
#include "temp_file.h"
#ifdef FCRONDYN
#include "socket.h"
#endif

char rcs_info[] = "$Id: fcron.c,v 1.69 2003/12/25 22:43:18 thib Exp thib $";

void main_loop(void);
void check_signal(void);
void info(void);
void usage(void);
void print_schedule(void);
RETSIGTYPE sighup_handler(int x);
RETSIGTYPE sigterm_handler(int x);
RETSIGTYPE sigchild_handler(int x);
RETSIGTYPE sigusr1_handler(int x);
RETSIGTYPE sigusr2_handler(int x);
int parseopt(int argc, char *argv[]);
void get_lock(void);
void create_spooldir(char *dir);



/* command line options */
#ifdef DEBUG
char debug_opt = 1;       /* set to 1 if we are in debug mode */
#else
char debug_opt = 0;       /* set to 1 if we are in debug mode */
#endif

#ifdef FOREGROUND
char foreground = 1; /* set to 1 when we are on foreground, else 0 */
#else
char foreground = 0; /* set to 1 when we are on foreground, else 0 */
#endif

time_t first_sleep = FIRST_SLEEP;
time_t save_time = SAVE;
char once = 0;      /* set to 1 if fcron shall return immediately after running
		     * all jobs that are due at the time when fcron is started */
char dosyslog = 1;  /* set to 1 when we log messages to syslog, else 0 */

/* used in temp_file() : create it in current dir (normally spool dir) */
char *tmp_path = "";

/* process identity */
pid_t daemon_pid;
mode_t saved_umask;           /* default root umask */
char *prog_name = NULL;         

/* have we got a signal ? */
char sig_conf = 0;            /* is 1 when we got a SIGHUP, 2 for a SIGUSR1 */ 
char sig_chld = 0;            /* is 1 when we got a SIGCHLD */  
char sig_debug = 0;           /* is 1 when we got a SIGUSR2 */  

/* jobs database */
struct cf_t *file_base;         /* point to the first file of the list */
struct job_t *queue_base;       /* ordered list of normal jobs to be run */
unsigned long int next_id;    /* id for next line to enter database */

struct cl_t **serial_array;     /* ordered list of job to be run one by one */
short int serial_array_size;  /* size of serial_array */
short int serial_array_index; /* the index of the first job */
short int serial_num;         /* number of job being queued */
short int serial_running;     /* number of running serial jobs */

/* do not run more than this number of serial job simultaneously */
short int serial_max_running = SERIAL_MAX_RUNNING; 
short int serial_queue_max   = SERIAL_QUEUE_MAX;
short int lavg_queue_max     = LAVG_QUEUE_MAX;

struct lavg_t *lavg_array;      /* jobs waiting for a given system load value */
short int lavg_array_size;    /* size of lavg_array */
short int lavg_num;           /* number of job being queued */
short int lavg_serial_running;/* number of serialized lavg job being running */

struct exe_t *exe_array;        /* jobs which are executed */
short int exe_array_size;     /* size of exe_array */
short int exe_num;            /* number of job being executed */

time_t begin_sleep;           /* the time at which sleep began */
time_t now;                   /* the current time */

#ifdef HAVE_LIBPAM
pam_handle_t *pamh = NULL;
const struct pam_conv apamconv = { NULL };
#endif

void
info(void)
    /* print some informations about this program :
     * version, license */
{
    fprintf(stderr,
	    "fcron " VERSION_QUOTED " - periodic command scheduler\n"
	    "Copyright 2000-2004 Thibault Godouet <fcron@free.fr>\n"
	    "This program is free software distributed WITHOUT ANY WARRANTY.\n"
            "See the GNU General Public License for more details.\n"
	);

    exit(EXIT_OK);

}


void
usage(void)
  /*  print a help message about command line options and exit */
{
    fprintf(stderr, "\nfcron " VERSION_QUOTED "\n\n"
	    "fcron [-d] [-f] [-b]\n"
	    "fcron -h\n"
	    "  -s t   --savetime t     Save fcrontabs on disk every t sec.\n"
	    "  -l t   --firstsleep t   Sets the initial delay before any job is executed"
	    ",\n                          default to %d seconds.\n"
	    "  -m n   --maxserial n    Set to n the max number of running serial jobs.\n"
	    "  -c f   --configfile f   Make fcron use config file f.\n"
	    "  -n d   --newspooldir d  Create d as a new spool directory.\n"
	    "  -f     --foreground     Stay in foreground.\n"
	    "  -b     --background     Go to background.\n"
	    "  -y     --nosyslog       Don't log to syslog at all.\n"
	    "  -o     --once           Execute all jobs that need to be run, wait for "
	    "them,\n                          then return. Sets firstsleep to 0.\n"
	    "                          Especially useful with -f and -y.\n"
	    "  -d     --debug          Set Debug mode.\n"
	    "  -h     --help           Show this help message.\n"
	    "  -V     --version        Display version & infos about fcron.\n",
	    FIRST_SLEEP
	);
    
    exit(EXIT_ERR);
}


void
print_schedule(void)
    /* print the current schedule on syslog */
{
    cf_t *cf;
    cl_t *cl;
    struct tm *ftime;

    explain("Printing schedule ...");
    for (cf = file_base; cf; cf = cf->cf_next) {
	explain(" File %s", cf->cf_user);
	for (cl = cf->cf_line_base; cl; cl = cl->cl_next) {
	    ftime = localtime( &(cl->cl_nextexe) );
	    explain("  cmd %s next exec %d/%d/%d wday:%d %02d:%02d",
		    cl->cl_shell, (ftime->tm_mon + 1), ftime->tm_mday,
		    (ftime->tm_year + 1900), ftime->tm_wday,
		    ftime->tm_hour, ftime->tm_min); 
	    
	}
    }
    explain("... end of printing schedule.");
}


void 
xexit(int exit_value) 
    /* exit after having freed memory and removed lock file */
{
    cf_t *f = NULL;

    now = time(NULL);

    /* we save all files now and after having waiting for all
     * job being executed because we might get a SIGKILL
     * if we don't exit quickly */
    save_file(NULL);
    
#ifdef FCRONDYN
    close_socket();
#endif

    f = file_base;
    while ( f != NULL ) {
	if ( f->cf_running > 0 ) {
	    /* */
	    debug("waiting jobs for %s ...", f->cf_user);
	    /* */
	    wait_all( &f->cf_running );	    
	    save_file(f);
	}
	delete_file(f->cf_user);    

	/* delete_file remove the f file from the list :
	 * next file to remove is now pointed by file_base. */
	f = file_base;
    }

    remove(pidfile);
    
    explain("Exiting with code %d", exit_value);
    exit (exit_value);

}

void
get_lock()
    /* check if another fcron daemon is running with the same config (-c option) :
     * in this case, die. if not, write our pid to /var/run/fcron.pid in order to lock,
     * and to permit fcrontab to read our pid and signal us */
{
    int otherpid = 0;
    FILE *daemon_lockfp = NULL;
    int fd;

    if (((fd = open(pidfile, O_RDWR|O_CREAT, 0644)) == -1 )
	|| ((daemon_lockfp = fdopen(fd, "r+"))) == NULL)
	die_e("can't open or create %s", pidfile);
	
#ifdef HAVE_FLOCK
    if ( flock(fd, LOCK_EX|LOCK_NB) != 0 )
#else /* HAVE_FLOCK */
	if ( lockf(fileno(daemon_lockfp), F_TLOCK, 0) != 0 )
#endif /* ! HAVE_FLOCK */
	    {
		fscanf(daemon_lockfp, "%d", &otherpid);
		die_e("can't lock %s, running daemon's pid may be %d",
		      pidfile, otherpid);
	    }

    fcntl(fd, F_SETFD, 1);

    rewind(daemon_lockfp);
    fprintf(daemon_lockfp, "%d\n", (int) daemon_pid);
    fflush(daemon_lockfp);
    ftruncate(fileno(daemon_lockfp), ftell(daemon_lockfp));

    /* abandon fd and daemon_lockfp even though the file is open. we need to
     * keep it open and locked, but we don't need the handles elsewhere.
     */

}


int
parseopt(int argc, char *argv[])
  /* set options */
{

    int c;
    int i;

#ifdef HAVE_GETOPT_LONG
    static struct option opt[] =
    {
	{"debug", 0, NULL, 'd'},
	{"foreground", 0, NULL, 'f'},
	{"background", 0, NULL, 'b'},
 	{"nosyslog", 0, NULL, 'y'},
	{"help", 0, NULL, 'h'},
	{"version", 0, NULL, 'V'},
 	{"once", 0, NULL, 'o'},
	{"savetime", 1, NULL, 's'},
 	{"firstsleep", 1, NULL, 'l'},
	{"maxserial", 1, NULL, 'm'},
	{"configfile", 1, NULL, 'c'},
	{"newspooldir", 1, NULL, 'n'},
	{"queuelen", 1, NULL, 'q'},
	{0,0,0,0}
    };
#endif /* HAVE_GETOPT_LONG */

    extern char *optarg;
    extern int optind, opterr, optopt;

    /* constants and variables defined by command line */

    while(1) {
#ifdef HAVE_GETOPT_LONG
	c = getopt_long(argc, argv, "dfbyhVos:l:m:c:n:q:", opt, NULL);
#else
	c = getopt(argc, argv, "dfbyhVos:l:m:c:n:q:");
#endif /* HAVE_GETOPT_LONG */
	if ( c == EOF ) break;
	switch ( (char)c ) {

	case 'V':
	    info(); break;

	case 'h':
	    usage(); break;

	case 'd':
	    debug_opt = 1; break;

	case 'f':
	    foreground = 1; break;

	case 'b':
	    foreground = 0; break;

 	case 'y':
	    dosyslog = 0; break;
	    
 	case 'o':
	    once = 1; first_sleep = 0; break;

	case 's':
	    if ( (save_time = strtol(optarg, NULL, 10)) < 60 || save_time >= LONG_MAX )
		die("Save time can only be set between 60 and %d.", LONG_MAX); 
	    break;

 	case 'l':
	    if ( (first_sleep = strtol(optarg, NULL, 10)) < 0 || first_sleep >= LONG_MAX)
		die("First sleep can only be set between 0 and %d.", LONG_MAX); 
	    break;
	    
	case 'm':
	    if ( (serial_max_running = strtol(optarg, NULL, 10)) <= 0 
		 || serial_max_running >= SHRT_MAX )
		die("Max running can only be set between 1 and %d.",SHRT_MAX);
	    break;

	case 'c':
	    Set(fcronconf, optarg);
	    break;

	case 'n':
	    create_spooldir(optarg);
	    break;

	case 'q':
	    if ( (lavg_queue_max = serial_queue_max = strtol(optarg, NULL, 10)) < 5 
		|| serial_queue_max >= SHRT_MAX )
		die("Queue length can only be set between 5 and %d.", SHRT_MAX);
	    break;

	case ':':
	    error("(parseopt) Missing parameter");
	    usage();

	case '?':
	    usage();

	default:
	    warn("(parseopt) Warning: getopt returned %c", c);
	}
    }

    if (optind < argc) {
	for (i = optind; i<=argc; i++)
	    error("Unknown argument \"%s\"", argv[i]);
	usage();
    }

    return OK;

}

void 
create_spooldir(char *dir)
    /* create a new spool dir for fcron : set correctly its mode and owner */
{
    int dir_fd = -1;
    struct passwd *pass = NULL;
    struct group *grp = NULL;
    struct stat st;

    if ( mkdir(dir, 0) != 0 && errno != EEXIST )
	die_e("Cannot create dir %s", dir);

    if ( (dir_fd = open(dir, 0)) < 0 )
	die_e("Cannot open dir %s", dir);

    if ( fstat(dir_fd, &st) != 0 ) {
	close(dir_fd);
	die_e("Cannot fstat %s", dir);
    }

    if ( ! S_ISDIR(st.st_mode) ) {
	close(dir_fd);
	die("%s exists and is not a directory", dir);
    }

    if ( (pass = getpwnam(USERNAME)) == NULL )
	die_e("Cannot getpwnam(%s)", USERNAME);

    if ( (grp = getgrnam(GROUPNAME)) == NULL )
	die_e("Cannot getgrnam(%s)", GROUPNAME);

    if ( fchown(dir_fd, pass->pw_uid, grp->gr_gid) != 0 ) {
	close(dir_fd);
	die_e("Cannot fchown dir %s to %s:%s", dir, USERNAME, GROUPNAME);
    }

    if (fchmod(dir_fd, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP) != 0) {
	close(dir_fd);
	die_e("Cannot change dir %s's mode to 770", dir);
    }

    close(dir_fd);

    exit(EXIT_OK);

}


RETSIGTYPE
sigterm_handler(int x)
  /* exit safely */
{
    debug("");
    explain("SIGTERM signal received");
    xexit(EXIT_OK);
}

RETSIGTYPE
sighup_handler(int x)
  /* update configuration */
{
    signal(SIGHUP, sighup_handler);
    siginterrupt(SIGHUP, 0);
    /* we don't call the synchronize_dir() function directly,
       because it may cause some problems if this signal
       is not received during the sleep
    */
    sig_conf = 1;
}

RETSIGTYPE
sigchild_handler(int x)
  /* call wait_chld() to take care of finished jobs */
{
    
    sig_chld = 1;

    (void)signal(SIGCHLD, sigchild_handler);
    siginterrupt(SIGCHLD, 0);
}


RETSIGTYPE
sigusr1_handler(int x)
  /* reload all configurations */
{
    signal(SIGUSR1, sigusr1_handler);
    siginterrupt(SIGUSR1, 0);
    /* we don't call the synchronize_dir() function directly,
       because it may cause some problems if this signal
       is not received during the sleep
    */
    sig_conf = 2;
}


RETSIGTYPE
sigusr2_handler(int x)
  /* print schedule and switch on/off debug mode */
{
    signal(SIGUSR2, sigusr2_handler);
    siginterrupt(SIGUSR2, 0);
    sig_debug = 1;
}


int
main(int argc, char **argv)
{

    /* we set it to 022 in order to get a pidfile readable by fcrontab
     * (will be set to 066 later) */
    saved_umask = umask(022);

    /* parse options */

    if ( strrchr(argv[0], '/') == NULL) prog_name = argv[0];
    else prog_name = strrchr(argv[0], '/') + 1;

    {
	uid_t daemon_uid;                 
	if ( (daemon_uid = getuid()) != ROOTUID )
	    die("Fcron must be executed as root");
    }

    /* we have to set daemon_pid before the fork because it's
     * used in die() and die_e() functions */
    daemon_pid = getpid();

    parseopt(argc, argv);

    /* read fcron.conf and update global parameters */
    read_conf();

    /* change directory */

    if (chdir(fcrontabs) != 0)
	die_e("Could not change dir to %s", fcrontabs);
    

    freopen("/dev/null", "r", stdin);

    if (foreground == 0) {

	/* close stdout and stderr.
	 * close unused descriptors
	 * optional detach from controlling terminal */

	int fd;
	pid_t pid;

	switch ( pid = fork() ) {
	case -1:
	    die_e("fork");
	    break;
	case 0:
	    /* child */
	    break;
	default:
	    /* parent */
/*  	    printf("%s[%d] " VERSION_QUOTED " : started.\n", */
/*  		   prog_name, pid); */
	    exit(0);
	}

	daemon_pid = getpid();

	if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
	    ioctl(fd, TIOCNOTTY, 0);
	    close(fd);
	}
	
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	/* close most other open fds */
	xcloselog();
	for(fd = 3; fd < 250; fd++) (void) close(fd);

	/* finally, create a new session */
	if ( setsid() == -1 )
	    error("Could not setsid()");

    }

    /* check if another fcron daemon is running, create pid file and lock it */
    get_lock();
    
    /* this program belongs to root : we set default permission mode
     * to  600 for security reasons, but we reset them to the saved
     * umask just before we run a job */
    umask(066);

    explain("%s[%d] " VERSION_QUOTED " started", prog_name, daemon_pid);

    signal(SIGTERM, sigterm_handler);
    signal(SIGHUP, sighup_handler);
    siginterrupt(SIGHUP, 0);
    signal(SIGCHLD, sigchild_handler);
    siginterrupt(SIGCHLD, 0);
    signal(SIGUSR1, sigusr1_handler);
    siginterrupt(SIGUSR1, 0);
    signal(SIGUSR2, sigusr2_handler);
    siginterrupt(SIGUSR2, 0);
    /* we don't want SIGPIPE to kill fcron, and don't need to handle it */
    signal(SIGPIPE, SIG_IGN);

    /* initialize job database */
    next_id = 0;

    /* initialize exe_array */
    exe_num = 0;
    exe_array_size = EXE_INITIAL_SIZE;
    if ( (exe_array = (exe_t *)calloc(exe_array_size, sizeof(struct exe_t))) == NULL )
	die_e("could not calloc exe_array");

    /* initialize serial_array */
    serial_running = 0;
    serial_array_index = 0;
    serial_num = 0;
    serial_array_size = SERIAL_INITIAL_SIZE;
    if ( (serial_array = (cl_t **)calloc(serial_array_size, sizeof(cl_t *))) == NULL )
	die_e("could not calloc serial_array");

    /* initialize lavg_array */
    lavg_num = 0;
    lavg_serial_running = 0;
    lavg_array_size = LAVG_INITIAL_SIZE;
    if ( (lavg_array = (lavg_t *)calloc(lavg_array_size, sizeof(lavg_t))) == NULL )
	die_e("could not calloc lavg_array");

#ifdef FCRONDYN
    /* initialize socket */
    init_socket();
#endif

    /* initialize random number generator :
     * WARNING : easy to guess !!! */
    srand(time(NULL));

    main_loop();

    /* never reached */
    return EXIT_OK;
}


void
check_signal()
    /* check if a signal has been received and handle it */
{

    if (sig_chld > 0) {
	wait_chld();
	sig_chld = 0;
    }
    if (sig_conf > 0) {

	if (sig_conf == 1)
	    /* update configuration */
	    synchronize_dir(".");
	else
	    /* reload all configuration */
	    reload_all(".");

	sig_conf = 0;
    }
    if (sig_debug > 0) {
	print_schedule();
	debug_opt = (debug_opt > 0)? 0 : 1;
	explain("debug_opt = %d", debug_opt);
	sig_debug = 0;
    }

}

void
main_loop()
  /* main loop - get the time to sleep until next job execution,
   *             sleep, and then test all jobs and execute if needed. */
{
    time_t save;           /* time remaining until next save */
    struct timeval tv;     /* we use usec field to get more precision */
    time_t stime;          /* time to sleep until next job
			    * execution */
#ifdef FCRONDYN
    int retcode = 0;
#endif

    debug("Entering main loop");

    now = time(NULL);

    synchronize_dir(".");

    /* synchronize save with jobs execution */
    save = now + save_time;

    if ( serial_num > 0 || once )
	stime = first_sleep;
    else if ( (stime = time_to_sleep(save)) < first_sleep )
	/* force first execution after first_sleep sec : execution of jobs
	 * during system boot time is not what we want */
	stime = first_sleep;

    for (;;) {
	
#ifdef HAVE_GETTIMEOFDAY
#ifdef FCRONDYN
	gettimeofday(&tv, NULL);
	tv.tv_sec = (stime > 1) ? stime - 1 : 0;
	tv.tv_usec = 1000000 - tv.tv_usec;
	if((retcode = select(set_max_fd+1, &read_set, NULL, NULL, &tv)) < 0 && errno != EINTR)
	    die_e("select return %d", errno);
#else
	if (stime > 1)
	    sleep(stime - 1);
	gettimeofday(&tv, NULL);
	usleep( 1000000 - tv.tv_usec );
#endif /* FCRONDYN */
#else
	sleep(stime);
#endif /* HAVE_GETTIMEOFDAY */

	now = time(NULL);

	check_signal();

	debug("\n");
	test_jobs();

    	while ( serial_num > 0 && serial_running < serial_max_running )
     	    run_serial_job();

 	if ( once ) {
	    explain("Running with option once : exiting ... ");
	    xexit(EXIT_OK);
	}

	if ( save <= now ) {
	    save = now + save_time;
	    /* save all files */
	    save_file(NULL);
	}

#ifdef FCRONDYN
	/* check if there's a new connection, a new command to answer, etc ... */
	/* we do that *after* other checks, to avoid Denial Of Service attacks */
	check_socket(retcode);
#endif

	stime = check_lavg(save);
	debug("next sleep time : %ld", stime);

	check_signal();

    }

}
