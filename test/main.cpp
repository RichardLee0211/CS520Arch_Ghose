#include<iostream>
#include<deque>
#include<set>

#include"unit1.h"
#include"unit2.h"
#include"global.h"

struct My{
    int valid;
    int data;
    int tag;
    My(int data): data{data}{
    }
    My(int tag, int data): data{data}, tag{tag}{
    }
};

bool operator<(const My& lhl, const My& lhr){
    return lhl.data < lhr.data;
}

struct FU{
    std::set<My> entry;
};

std::deque<int> order;

// My my[4];


int main(){
    // function1();
    // function2();
    FU *f = (FU*)malloc(sizeof(FU));
    f->entry.insert(My(1, 1));
    f->entry.insert(My(2, 2));
    f->entry.insert(My(3, 3));
    f->entry.insert(My(4, 4));
    std::cout<<f->entry.size();
}
