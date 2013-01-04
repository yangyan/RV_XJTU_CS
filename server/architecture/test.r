Rule test(malloc_and_free){
	within("f_main()"){
		key x;
		"f_malloc(x)" & (N "f_free(x)");
	}solution_free(x) && stop();
	within("f_main()"){
		key x;
		"f_malloc(x)" & (X ("f_free(x)" & (X (G (~ "f_free(x)")))));
	}solution_skipfree(x) && replace(false, f_free);

	within("f_often()"){
		key x;
		"f_malloc(x)" & (N "f_free(x)");
	}solution_free(x) && stop();
	within("f_often()"){
		key x;
		"f_malloc(x)" & (X ("f_free(x)" & (X (G (~ "f_free(x)")))));
	}solution_skipfree(x) && replace(false, f_free);
}


