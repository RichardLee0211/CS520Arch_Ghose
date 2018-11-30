#include<iostream>
#include"unit1.h"
#include"global.h"

void function1(){
    global_int++;
    std::cout<<"function1: "<<global_int<<std::endl;
}
