//
//  Canvas.h
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//

#ifndef __wtest4__Canvas__
#define __wtest4__Canvas__

#include <iostream>
#include "Cell.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

class Canvas {
private:
    int height;
    int length;
public:
    //Canvas canvas(void);
    vector<vector<Cell*> >* cells;
    vector<vector<int> >* M;
    Canvas(int x, int y, float wp, float h);
    Canvas(int x, int y);
    void initiate(void);
    float get_cell_wp (int,int);
    vector<int> get_cell_p (int,int);
};




#endif /* defined(__wtest4__Canvas__) */
