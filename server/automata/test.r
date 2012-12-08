Rule test(test){
    within("f_main()")
    {
        key x,y;
        (G "f_a(x,y,z,m)") & (F "f_b(x,e,f)");
    }
    exit;
}
