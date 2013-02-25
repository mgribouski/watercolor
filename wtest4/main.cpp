//
//  main.cpp
//  wtest4
//
//  Created by calliope on 10/7/12.
//  Copyright (c) 2012 calliope. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Canvas.h"
#include "Cell.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "Tuple.h"

using namespace std;
using namespace cv;

Mat src;
Mat dst;
Mat dst2;
Mat dst3;
Mat dst4;
Mat detected_edges;
int rows;
int cols;
int my_time = 0;
Canvas* canvas;
float viscosity = 0.1;
float viscous_drag = 0.01;
float epsilon = 0.1;
float tolerance = 0.01;
int DELAY_CAPTION = 1500;
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 9;

double alpha = 2.5; // contrast control between [1.0-3.0]
int beta = 60;  // brightness control between [0-100]

int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
int lowThreshold = 50;

float rand_FloatRange(float a, float b) {
    return ((b-a)*((float)rand()/RAND_MAX))+a;
}

void mat2Canvas() {
    // CONVERT MAT TO CANVAS
    for(int i = 0; i < src.rows; i++)
    {
        vector<Cell*> tmp;
        
        for(int j = 0; j < src.cols; j++)
        {
            // OPENCV STORES AS BGR
            Vec3b bgrPixel = src.at<Vec3b>(i, j);
            
            vector<int> rgbPixel;
            rgbPixel.push_back(bgrPixel[2]);
            rgbPixel.push_back(bgrPixel[1]);
            rgbPixel.push_back(bgrPixel[0]);
            
            Cell* cur_cell = new Cell(rgbPixel);
            float h = rand_FloatRange(0, 0.05);
            //cout << h << endl;
            cur_cell->set_height(h);

            tmp.push_back(cur_cell);
            
        }
        canvas->cells->push_back(tmp);
    }
}

void canvas2Mat() {
    // CONVERT CANVAS TO MAT
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            Vec3b bgrPixel;
            
            bgrPixel[2]= (canvas->cells->at(i).at(j)->get_pigment()[0]);
            bgrPixel[1] = (canvas->cells->at(i).at(j)->get_pigment()[1]);
            bgrPixel[0] = (canvas->cells->at(i).at(j)->get_pigment()[2]);
            
            dst.at<Vec3b>(i, j) = bgrPixel;
            
        }
    }
}


float thirdStepUV(Canvas* canvas, float A, float B, int i, int j) {
    // u_bound = u(i+.5)j + timechange(A - mewB?? + pij - p(i+1)j - ku(i+.5)j;
    float t;
    try {
        t = canvas->cells->at(i).at(j)->get_rb() + 1*(A - viscosity*B + canvas->cells->at(i).at(j)->get_water_pressure() - canvas->cells->at(i+1).at(j)->get_water_pressure() + viscous_drag*canvas->cells->at(i).at(j)->get_rb());
    } catch (...) {
        t = canvas->cells->at(i).at(j)->get_rb() + 1*(A - viscosity*B + canvas->cells->at(i).at(j)->get_water_pressure() - canvas->cells->at(i).at(j)->get_water_pressure() + viscous_drag*canvas->cells->at(i).at(j)->get_rb());
    }
    
    return t;
}

float sixthStepUV(Canvas* canvas, float A, float B, int i, int j) {
    // r_bound = vi(j+.5) + timechange(A - mewB?? + pij - p(i+1)j - kv(i+.5)j;
    float s;
    try {
        s = canvas->cells->at(i).at(j)->get_ub() + 1*(A - B + canvas->cells->at(i).at(j)->get_water_pressure() - canvas->cells->at(i).at(j+1)->get_water_pressure() + canvas->cells->at(i).at(j)->get_ub());
    } catch (...) {
        s = canvas->cells->at(i).at(j)->get_ub() + 1*(A - B + canvas->cells->at(i).at(j)->get_water_pressure() - canvas->cells->at(i).at(j)->get_water_pressure() + canvas->cells->at(i).at(j)->get_ub());
    }
    return s;
}

void updateVelocities(void) {
    
    float A;
    float B;
    vector<vector<std::pair<float, float> > >* newUs = new vector<vector<std::pair<float, float> > >;
    
    cout << "in updateVelocities before main loop";
    
    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {
            
            // if the cell next to it is higher/lower, sets the boundary velocity as the dif in height
            try {
                canvas->cells->at(i).at(j)->set_rb(canvas->cells->at(i).at(j)->get_height() - canvas->cells->at(i+1).at(j)->get_height());
            } catch (...) { }
            try {
                canvas->cells->at(i).at(j)->set_lb(canvas->cells->at(i).at(j)->get_height() - canvas->cells->at(i-1).at(j)->get_height());
            } catch (...) { }
            try {
                canvas->cells->at(i).at(j)->set_ub(canvas->cells->at(i).at(j)->get_height() - canvas->cells->at(i).at(j+1)->get_height());
            } catch (...) { }
            try {
                canvas->cells->at(i).at(j)->set_db(canvas->cells->at(i).at(j)->get_height() - canvas->cells->at(i).at(j-1)->get_height());
            } catch (...) { }
            
        }
    }
    
    for (int i = 0; i < src.rows; i++) {
        
        vector<std::pair<float, float> > tmp;
        
        for (int j = 0; j < src.cols; j++) {
            // i + .5 = i->get_rb or i+1->get_lb
            // i - .5 = i->get_lb or i-1->get_rp
            // j + .5 = j->get_ub or j+1->get_db
            // j - .5 = j->get_db or j-1->get_ub
            
            // A =  uij^2 - u(i+1)j^2 + (uv)(i+.5)(j-.5) - (uv)(i+.5)j+.5
            try {
                A = pow(canvas->cells->at(i).at(j)->get_u(), 2) - pow(canvas->cells->at(i+1).at(j)->get_u(), 2) + (canvas->cells->at(i).at(j)->get_rb()*canvas->cells->at(i).at(j)->get_db()) - (canvas->cells->at(i).at(j)->get_rb() * canvas->cells->at(i).at(j)->get_ub());
            } catch (...) {
                A = pow(canvas->cells->at(i).at(j)->get_u(), 2) - pow(canvas->cells->at(i).at(j)->get_u(), 2) + (canvas->cells->at(i).at(j)->get_rb()*canvas->cells->at(i).at(j)->get_db()) - (canvas->cells->at(i).at(j)->get_rb() * canvas->cells->at(i).at(j)->get_ub());
            }
            
            // B = u(i+1.5)j + u(i-.5)j + u(i+.5)(j+1) + u(i+.5)(j-1) - 4u(i+.5)j
            try {
                B = canvas->cells->at(i+1).at(j)->get_rb() + canvas->cells->at(i).at(j)->get_lb() + canvas->cells->at(i).at(j+1)->get_rb() + canvas->cells->at(i).at(j-1)->get_rb() + 4*(canvas->cells->at(i).at(j)->get_rb());
            } catch (...) {
                B = canvas->cells->at(i).at(j)->get_rb() + canvas->cells->at(i).at(j)->get_lb() + canvas->cells->at(i).at(j)->get_rb() + canvas->cells->at(i).at(j)->get_rb() + 4*(canvas->cells->at(i).at(j)->get_rb());
            }
            
            // u_bound = u(i+.5)j + timechange(A - mewB?? + pij - p(i+1)j - ku(i+.5)j;
            float t = thirdStepUV(canvas, A, B, i, j);
            
            // A =  vij^2 - v(i)(j+1)^2 + (uv)(i-.5)(j+.5) - (uv)(i+.5)j+.5
            try {
                A = pow(canvas->cells->at(i).at(j)->get_v(), 2) - pow(canvas->cells->at(i).at(j+1)->get_v(), 2) + (canvas->cells->at(i).at(j)->get_lb()*canvas->cells->at(i).at(j)->get_ub()) - (canvas->cells->at(i).at(j)->get_rb() * canvas->cells->at(i).at(j)->get_ub());
            } catch (...) {
                A = pow(canvas->cells->at(i).at(j)->get_v(), 2) - pow(canvas->cells->at(i).at(j)->get_v(), 2) + (canvas->cells->at(i).at(j)->get_lb()*canvas->cells->at(i).at(j)->get_ub()) - (canvas->cells->at(i).at(j)->get_rb() * canvas->cells->at(i).at(j)->get_ub());
            }
            // B = v(i+1)(j+.5) + v(i-1)(j+.5) + v(i)(j+.5) + v(i)(j-.5) - 4v(i)(j+.5)
            try {
                B = canvas->cells->at(i+1).at(j)->get_ub() + canvas->cells->at(i-1).at(j)->get_ub() + canvas->cells->at(i).at(j)->get_ub() + canvas->cells->at(i).at(j-1)->get_ub() + 4*(canvas->cells->at(i).at(j)->get_ub());
            } catch (...) {
                B = canvas->cells->at(i).at(j)->get_ub() + canvas->cells->at(i).at(j)->get_ub() + canvas->cells->at(i).at(j)->get_ub() + canvas->cells->at(i).at(j)->get_ub() + 4*(canvas->cells->at(i).at(j)->get_ub());
            }
            

            
            float s = sixthStepUV(canvas, A, B, i, j);
            pair <float,float> p (t,s);
            tmp.push_back(p);
            
        }
        newUs->push_back(tmp);
    }
    
    // update all new boundaries
    for (int i=0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            canvas->cells->at(i).at(j)->set_rb(newUs->at(i).at(j).first);
            try {
                canvas->cells->at(i+1).at(j)->set_lb(-1*newUs->at(i).at(j).first);
            } catch (...) { }
            
            canvas->cells->at(i).at(j)->set_ub(newUs->at(i).at(j).second);
            try {
                canvas->cells->at(i).at(j+1)->set_db(-1*newUs->at(i).at(j).second);
            } catch (...) { }
        }
    }

    
}




void movePigments() {
    // find the max velocity
    float max=0;
    for(int ii=0; ii<src.rows;ii++)
    {
        for(int jj=0;jj<src.cols; jj++)
        {
            if(canvas->cells->at(ii).at(jj)->get_ub() > max)
            {
                max = canvas->cells->at(ii).at(jj)->get_ub();
            }
            if(canvas->cells->at(ii).at(jj)->get_db() > max)
            {
                max = canvas->cells->at(ii).at(jj)->get_db();
            }
            if(canvas->cells->at(ii).at(jj)->get_rb() > max)
            {
                max = canvas->cells->at(ii).at(jj)->get_rb();
            }
            if(canvas->cells->at(ii).at(jj)->get_lb() > max)
            {
                max = canvas->cells->at(ii).at(jj)->get_lb();
            }
        }
    }
    
    
    for(int ii=0; ii<src.rows;ii++) //initilize pigmentPrimes to be same values as Pigment
    {
        for(int jj=0;jj<src.cols; jj++)
        {
            vector<int> tmp;
            
            tmp.push_back(canvas->cells->at(ii).at(jj)->get_pigment().at(0));
            tmp.push_back(canvas->cells->at(ii).at(jj)->get_pigment().at(1));
            tmp.push_back(canvas->cells->at(ii).at(jj)->get_pigment().at(2));
            canvas->cells->at(ii).at(jj)->set_pigmentPrime(tmp);
            
        }
    }
    
    for(int ii=0; ii<src.rows;ii++)
    {
        for(int jj=0;jj<src.cols; jj++)
        {
            if(canvas->cells->at(ii).at(jj)->get_ub() > 0) //move pigment into North neighbor
            {
                int myRed = (canvas->cells->at(ii).at(jj)->get_ub()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(0);
                int myGreen = (canvas->cells->at(ii).at(jj)->get_ub()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(1);
                int myBlue = (canvas->cells->at(ii).at(jj)->get_ub()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(2);
                
                try {
                    int northRed = canvas->cells->at(ii).at(jj+1)->get_pigmentPrime().at(0);
                    int northGreen = canvas->cells->at(ii).at(jj+1)->get_pigmentPrime().at(1);
                    int northBlue = canvas->cells->at(ii).at(jj+1)->get_pigmentPrime().at(2);
                    
                    vector<int> newPigs;
                    newPigs.push_back((myRed+northRed)/2);
                    newPigs.push_back((myGreen+northGreen)/2);
                    newPigs.push_back((myBlue+northBlue)/2);
                    
                    canvas->cells->at(ii).at(jj+1)->set_pigmentPrime(newPigs);
           
                } catch (...) { }
                
                
            }
            if(canvas->cells->at(ii).at(jj)->get_db() > 0) //move pigment into South neighbor
            {
                int myRed = (canvas->cells->at(ii).at(jj)->get_db()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(0);
                int myGreen = (canvas->cells->at(ii).at(jj)->get_db()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(1);
                int myBlue = (canvas->cells->at(ii).at(jj)->get_db()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(2);
                
                try {
                    int southRed = canvas->cells->at(ii).at(jj-1)->get_pigmentPrime().at(0);
                    int southGreen = canvas->cells->at(ii).at(jj-1)->get_pigmentPrime().at(1);
                    int southBlue = canvas->cells->at(ii).at(jj-1)->get_pigmentPrime().at(2);
                    
                    vector<int> newPigs;
                    newPigs.push_back((myRed+southRed)/2);
                    newPigs.push_back((myGreen+southGreen)/2);
                    newPigs.push_back((myBlue+southBlue)/2);
                    
                    canvas->cells->at(ii).at(jj-1)->set_pigmentPrime(newPigs);
                    
                } catch (...) { }
                
                
            }
            if(canvas->cells->at(ii).at(jj)->get_rb() > 0) //move pigment into East neighbor
            {
                int myRed = (canvas->cells->at(ii).at(jj)->get_rb()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(0);
                int myGreen = (canvas->cells->at(ii).at(jj)->get_rb()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(1);
                int myBlue = (canvas->cells->at(ii).at(jj)->get_rb()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(2);
                
                try {
                    int eastRed = canvas->cells->at(ii+1).at(jj)->get_pigmentPrime().at(0);
                    int eastGreen = canvas->cells->at(ii+1).at(jj)->get_pigmentPrime().at(1);
                    int eastBlue = canvas->cells->at(ii+1).at(jj)->get_pigmentPrime().at(2);
                    
                    vector<int> newPigs;
                    newPigs.push_back((myRed+eastRed)/2);
                    newPigs.push_back((myGreen+eastGreen)/2);
                    newPigs.push_back((myBlue+eastBlue)/2);
                    
                    canvas->cells->at(ii+1).at(jj)->set_pigmentPrime(newPigs);
                    
                } catch (...) { }
                
                
            }
            
            if(canvas->cells->at(ii).at(jj)->get_lb() > 0) //move pigment into West neighbor
            {
                int myRed = (canvas->cells->at(ii).at(jj)->get_rb()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(0);
                int myGreen = (canvas->cells->at(ii).at(jj)->get_rb()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(1);
                int myBlue = (canvas->cells->at(ii).at(jj)->get_rb()/max)*canvas->cells->at(ii).at(jj)->get_pigment().at(2);
                
                try {
                    int westRed = canvas->cells->at(ii-1).at(jj)->get_pigmentPrime().at(0);
                    int westGreen = canvas->cells->at(ii-1).at(jj)->get_pigmentPrime().at(1);
                    int westBlue = canvas->cells->at(ii-1).at(jj)->get_pigmentPrime().at(2);
                    
                    vector<int> newPigs;
                    newPigs.push_back((myRed+westRed)/2);
                    newPigs.push_back((myGreen+westGreen)/2);
                    newPigs.push_back((myBlue+westBlue)/2);
                    
                    canvas->cells->at(ii-1).at(jj)->set_pigmentPrime(newPigs);

                } catch (...) { }
            }
        }
    }
    
    for(int ii=0; ii<src.rows;ii++) {
        for(int jj=0;jj<src.cols; jj++) {
            canvas->cells->at(ii).at(jj)->set_pigment(canvas->cells->at(ii).at(jj)->get_pigmentPrime());
        }
    }
   
}


void relaxDivergence(void) {

    vector<vector<std::pair<float, float> > >* newUs = new vector<vector<std::pair<float, float> > >;
    float deltamax = 0;
    
    
    // (u', v') = (u,v)
    for (int i = 0; i < src.rows; i++) {
        vector<std::pair<float, float> > tmp;
        for (int j = 0; j < src.cols; j++) {
            pair <float,float> p (canvas->cells->at(i).at(j)->get_u(), canvas->cells->at(i).at(j)->get_v() );
            tmp.push_back(p);
        }
        newUs->push_back(tmp);
    }
    

    
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            deltamax = epsilon*(canvas->cells->at(i).at(j)->get_rb() - canvas->cells->at(i).at(j)->get_lb() + canvas->cells->at(i).at(j)->get_ub() - canvas->cells->at(i).at(j)->get_db());
            canvas->cells->at(i).at(j)->set_water_pressure( canvas->cells->at(i).at(j)->get_water_pressure() + deltamax );
            newUs->at(i).at(j).first = newUs->at(i).at(j).first + deltamax;
            try {
                newUs->at(i-1).at(j).first = newUs->at(i).at(j).first - deltamax;
            } catch (...) { }
            newUs->at(i).at(j).second = newUs->at(i).at(j).second + deltamax;
            try {
                newUs->at(i).at(j-1).second = newUs->at(i).at(j).second + deltamax;
            } catch (...) { }
            deltamax = max((float)abs(deltamax), deltamax);
        }
        
        if (deltamax >= tolerance) {
            break;
        }
    }
    

    
    // update all new boundaries
    for (int i=0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            
            canvas->cells->at(i).at(j)->set_ub(newUs->at(i).at(j).first);
            try {
                canvas->cells->at(i+1).at(j)->set_db(newUs->at(i).at(j).first);
            } catch (...) { }
            
            canvas->cells->at(i).at(j)->set_rb(newUs->at(i).at(j).second);
            try {
                canvas->cells->at(i).at(j+1)->set_lb(newUs->at(i).at(j).second);
            } catch (...) { }
            
        }
    }
    
}



void upBrightnessImg() {
    
    for( int y = 0; y < src.rows; y++ ) {
        for( int x = 0; x < src.cols; x++ ) {
            for( int c = 0; c < 3; c++ ) {
                dst3.at<Vec3b>(y,x)[c] =
                saturate_cast<uchar>( alpha*( dst2.at<Vec3b>(y,x)[c] ) + beta );
            }
        }
    }
}


void cannyTest() {
    
    Mat src_gray;
    
    src_gray.create(src.size(), src.type());

    /// Convert the image to grayscale
    cvtColor( dst3, src_gray, CV_BGR2GRAY );
    
    /// Reduce noise with a kernel 3x3
    blur( src_gray, detected_edges, Size(3,3) );
    
    /// Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    
    /// Using Canny's output as a mask, we display our result
    dst4 = Scalar::all(0);
    
    dst3.copyTo( dst4, detected_edges);
    
}


int main(int argc, const char * argv[])
{
    src = imread( "mushroom.jpg" );
    if( !src.data )
    {
        cout << "image failed to load" << endl;
        return -1;
    }
    dst.create( src.size(), src.type() );
    canvas = new Canvas(src.rows, src.cols);
    
    mat2Canvas();

    for (my_time = 0; my_time < 1; my_time++) {
        updateVelocities();
        movePigments();
        
    }
    
    canvas2Mat();
 
    dst2.create(src.size(), src.type());
    
    // blur the results
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    { GaussianBlur( dst, dst2, Size( i, i ), 0, 0 );
    }
    
    dst3.create(src.size(), src.type());
    upBrightnessImg();
    //cannyTest();
    

    imshow( "WATERCOLOR IMAGE", dst3 );
    imwrite("mushroom2.jpg", dst3);
    waitKey(0);

    return 0;
}

