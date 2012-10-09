//
//  Canvas.cpp
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//




#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "Cell.h"
#include "Canvas.h"

using namespace std;



Canvas::Canvas(int x, int y, float wp, float h) {
    height = x;
    length = y;
    cells = new vector<vector<Cell*> >;
}

Canvas::Canvas(int x, int y) {
    height = x;
    length = y;
    cells = new vector<vector<Cell*> >;
    M = new vector<vector<int> >;

}

vector<int> Canvas::get_cell_p (int x, int y) {
	return cells->at(x).at(y)->get_pigment();
}

float Canvas::get_cell_wp (int x, int y) {
	return cells->at(x).at(y)->get_water_pressure();
}
