Rule fcrontab(malloc_and_free){
	within("f_main()"){
		key p;
		"f_malloc(p)" -> (N "f_free(p)");
	}solution_dofree(p) && stop();
}


