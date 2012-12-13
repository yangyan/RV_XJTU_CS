Rule test(rule1){
	within("f_often()"){
		key x;
		"f_malloc(x)" & (N ("f_free(x)" & (X (~"f_free(x)"))));
	}solution_skipfree(x);
}


