//
//  Cell.cpp
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//

#include "Cell.h"
#include <iostream>


using namespace std;
// constructors
Cell::Cell (int x, int y, float water_pressure, vector<int> pigment) {
    set_location(x, y);
    set_water_pressure(water_pressure);
    set_pigment(pigment);
}

Cell::Cell (vector<int> p) {
    pigment = p;
}

Cell::Cell (void) {
    
}

// member functions
void Cell::set_location (int a, int b) {
    x = a;
    y = b;
}

void Cell::set_water_pressure (float wp) {
    water_pressure = wp;
}

void Cell::set_pigment (vector<int> p) {
    pigment = p;
}

void Cell::set_pigmentPrime (vector<int> p) {
    pigmentPrime = p;
}


void Cell::set_height(float h) {
    height = h;
}

void Cell::set_water_saturation(float s) {
    water_saturation = s;
}


void Cell::set_lb(float lb) {
    l_bound = lb;
}

void Cell::set_rb(float rb) {
    r_bound = rb;
}
void Cell::set_ub(float ub) {
    u_bound = ub;
}
void Cell::set_db(float db) {
    d_bound = db;
}


void Cell::set_U(float x) {
    u = x;
}

void Cell::set_V(float x) {
    v = x;
}

vector<int> Cell::get_pigment () {
    return pigment;
}

vector<int> Cell::get_pigmentPrime () {
    return pigmentPrime;
}

float Cell::get_water_pressure () {
    return water_pressure;
}

float Cell::get_water_saturation() {
    return water_saturation;
}

float Cell::get_height() {
    return height;
}

float Cell::get_u() {
    u = (get_lb() + get_rb())/2;
    return u;
}

float Cell::get_v() {
    v = (get_ub() - get_db())/2;
    return v;
}

float Cell::get_lb(void) {
    return l_bound;
}
float Cell::get_rb(void) {
    return r_bound;
}
float Cell::get_ub(void) {
    return u_bound;
}
float Cell::get_db(void) {
    return d_bound;
}