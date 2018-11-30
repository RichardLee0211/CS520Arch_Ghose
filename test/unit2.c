#include<iostream>
#include"unit2.h"
#include"global.h"

void function2(){
    global_int++;
    std::cout<<"function2: "<<global_int<<std::endl;
}
