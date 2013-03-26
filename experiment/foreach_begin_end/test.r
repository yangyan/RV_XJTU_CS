Rule fbe(for_each_begin_end){
	within("f_main()"){
		key a, b, ai, bi, ix, iy;
		G(~("f_vector_end(b,bi)" & (N (("f_vector_begin(a,ai)") & (N "f_for_each(ix,iy)")))));
	}solution_check_fbe(a, b, ai, bi, ix, iy) && stop();
}
