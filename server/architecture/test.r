Rule test(rule1){
	within("f_main()"){
		key x;
		G ("f_malloc(x)" -> (F "f_free(x)"));
	}exit;
}


