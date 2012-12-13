Rule test(rule1){
	within("f_often()"){
		key x;
		"f_hello(x)" & (N ("f_world(x)" & (X (~"f_world(x)"))));
	}solution_exit(x);
}


