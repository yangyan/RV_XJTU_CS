Rule test(rule1){
	within("f_often()"){
		key x;
		G ("f_malloc(x)" -> (F "f_free(x)"));
	}solution_exit(x);
}


