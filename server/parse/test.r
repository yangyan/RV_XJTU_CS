Rule test(test){
    within("f_main()")
    {
        key xy,y,z;
        G "f_a(xy,y,z)";
    }
    exit(x,y) && over();
}
