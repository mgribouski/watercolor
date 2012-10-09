//
//  Tuple.cpp
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//

#include "Tuple.h"


using namespace std;
// constructors
Tuple::Tuple (int x, int y, int z) {
    i1 = x;
    i2 = y;
    i3 = z;
}

Tuple::Tuple (float x, float y, float z) {
    f1 = x;
    f2 = y;
    f3 = z;
}

Tuple::Tuple (void) {
    
}

// member functions


void Tuple::set_i1(int i) {
    i1 = i;
}
void Tuple::set_i2(int i ) {
    i2 = i;
}
void Tuple::set_i3(int i ) {
    i3 = i;
}


void Tuple::set_i(int i , int num) {
    if (num == 0) {
        set_i1(i);
    }
    if (num == 1) {
        set_i2(i);
    }
    if (num == 3) {
        set_i3(i);
    }
}

int Tuple::get_i (int num) {
    if (num == 0) {
        return get_i1();
    }
    else if (num == 1) {
        return get_i2();
    }
    else
        return get_i3();
}


int Tuple::get_i1(void) {
    return i1;
}
int Tuple::get_i2(void) {
    return i2;
}
int Tuple::get_i3(void) {
    return i3;
}