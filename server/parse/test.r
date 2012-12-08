Rule test(test){
    within("f_main()")
    {
        key x,y,z;
        G "f_a(x,y,z)";
    }
    exit(x,y);
}
