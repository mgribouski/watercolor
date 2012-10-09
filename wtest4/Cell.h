//
//  Cell.h
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//

#ifndef __wtest4__Cell__
#define __wtest4__Cell__

#include <iostream>
#include <vector.h>

class Cell
{
private:
    int M; // wet-area-mask 1 if wet
    int x, y; // location on paper
    float u, v; // water velocity on x and y directions
    float l_bound; // boundaries store water velocities
    float r_bound;
    float u_bound;
    float d_bound;
    float water_pressure;
    float water_saturation;
    // [0] = r, [1] = g, [2] = b
    vector<int> pigment;
    vector<int> pigmentPrime;
    float height;
public:
    Cell(int, int, float, vector<int>);
    Cell(vector<int>);
    Cell(void);
    
    void set_location (int,int);
    void set_pigment (vector<int>);
    void set_pigmentPrime (vector<int>);
    void set_water_pressure (float);
    void set_water_saturation(float);
    void set_height(void);
    void set_height(float);
    
    void set_U(float);
    void set_V(float);
    
    void set_lb(float);
    void set_rb(float);
    void set_ub(float);
    void set_db(float);
    
    vector<int> get_pigment (void);
     vector<int> get_pigmentPrime (void);
    float get_water_pressure (void);
    float get_water_saturation (void);
    float get_height(void);
    
    float get_u(void);
    float get_v(void);
    
    float get_lb(void);
    float get_rb(void);
    float get_ub(void);
    float get_db(void);
    
};

#endif /* defined(__wtest4__Cell__) */
