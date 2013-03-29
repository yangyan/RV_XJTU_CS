Rule fbe(for_each_begin_end){
	within("f_main()"){
		key a;
		G("f_vector_end(a)" | "f_vector_begin(a)");
	}solution_check_fbe(a) && stop();
}
