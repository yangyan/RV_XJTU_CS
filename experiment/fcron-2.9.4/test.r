Rule fcrontab(open_and_close){
	within("f_is_allowed()"){
		key fd;
		"f_fopen(fd)" & (N "f_fclose(fd)");
	}solution_doclose(fd) && stop();
}


