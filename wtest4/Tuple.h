//
//  Tuple.h
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//

#ifndef __wtest4__Tuple__
#define __wtest4__Tuple__

#include <iostream>



#include <stdlib.h>
#include <stdio.h>
#include <vector.h>
#include <iostream>


class Tuple
{
private:
    int i1;
    int i2;
    int i3;
    float f1;
    float f2;
    float f3;
public:
    Tuple(int, int, int);
    Tuple(float, float, float);
    Tuple(void);
    
    void set_i1(int);
    void set_i2(int);
    void set_i3(int);
    
    void set_i(int, int);
    int get_i(int);
    
    int get_i1(void);
    int get_i2(void);
    int get_i3(void);
    
    void set_f1(float);
    void set_f2(float);
    void set_f3(float);
    
    float get_f1(void);
    float get_f2(void);
    float get_f3(void);
    
};




#endif /* defined(__wtest4__Tuple__) */
