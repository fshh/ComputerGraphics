/** @file main.cpp
 *  @brief Entry point into our program.
 *  
 *  Welcome to the Great Looking Software Render 
 *  code base (Yes, I needed something with a gl prefix).
 *
 *  This is where you will implement your graphics API.
 *
 *  Compile on the terminal with: 
 *
 *  clang++ -std=c++11 main.cpp -o main
 *
 *  @author Mike Shah
 *  @bug No known bugs.
 */

// Load our libraries
#include <iostream>

// Some define values
#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH 320

// C++ Standard Libraries
#include <iostream>
#include <vector>
#include <algorithm>

// User libraries
#include "GL.h"
#include "Color.h"
#include "TGA.h"
#include "Maths.h"

// Create a canvas to draw on.
TGA canvas(WINDOW_WIDTH,WINDOW_HEIGHT);

// Implementation of Bresenham's Line Algorithm
// The input to this algorithm is two points and a color
// This algorithm will then modify a canvas (i.e. image)
// filling in the appropriate colors.
void drawLine(Vec2 v0, Vec2 v1, TGA& image, ColorRGB c){
    bool steep = false;
    if(std::abs(v0.x-v1.x)<std::abs(v0.y-v1.y)){
        // If the line is steep we want to transpose the image.
        std::swap(v0.x,v0.y);
        std::swap(v1.x,v1.y);
        steep = true;
    }
    if(v0.x>v1.x){  // make it left-to-right
        std::swap(v0.x, v1.x);
        std::swap(v0.y, v1.y);
    }
    for(int x = v0.x; x <= v1.x; ++x){
        float t = (x-v0.x)/(float)(v1.x-v0.x);
        int y = v0.y*(1.0f-t) + v1.y*t;
        if(steep){
            image.setPixelColor(y,x,c);
        }else{
            image.setPixelColor(x,y,c);
        }
    }
}

// draw a horizontal line
void drawHorizontalLine(Vec2 v0, Vec2 v1, TGA& image, ColorRGB c) {
    if (v0.y != v1.y) {
        std::cerr << "Error: given points do not form a horizontal line" << std::endl;
    }

    if (v0.x > v1.x) {
        std::swap(v0, v1);
    }

    int y = v0.y;
    for (int x = v0.x; x <= v1.x; ++x) {
        image.setPixelColor(x,y,c);
    }
}

/*
 *  Information on triangle drawing algorithms taken from:
 *  http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
 */

// draws a triangle which is known to have a flat bottom
void drawFlatBottomTriangle(Triangle tri, TGA& image, ColorRGB c) {
    // sort points based on y value
    tri.sortPointsByHeight();

    Vec2 v0 = tri.v0, v1 = tri.v1, v2 = tri.v2;

    // verify that arguments are valid
    if (v1.y != v2.y) {
        std::cerr << "Error: given points do not form a flat bottom triangle" << std::endl;
        return;
    }

    // find inverse slopes of non-flat sides
    float invslope1 = (float)(v1.x - v0.x) / (v1.y - v0.y);
    float invslope2 = (float)(v2.x - v0.x) / (v2.y - v0.y);

    // variables to track x-coordinate on both sides
    float curx1 = v0.x;
    float curx2 = v0.x;

    // scan from top to bottom, drawing flat lines across
    for (int scanlineY = v0.y; scanlineY <= v1.y; ++scanlineY) {
        drawHorizontalLine(Vec2((int)curx1, scanlineY), Vec2((int)curx2, scanlineY), image, c);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

// draws a triangle which is known to have a flat top
void drawFlatTopTriangle(Triangle tri, TGA& image, ColorRGB c) {
    // sort points based on y value
    tri.sortPointsByHeight();

    Vec2 v0 = tri.v0, v1 = tri.v1, v2 = tri.v2;

    // verify that arguments are valid
    if (v0.y != v1.y) {
        std::cerr << "Error: given points do not form a flat top triangle" << std::endl;
        return;
    }

    // find inverse slopes of non-flat sides
    float invslope1 = (float)(v2.x - v0.x) / (v2.y - v0.y);
    float invslope2 = (float)(v2.x - v1.x) / (v2.y - v1.y);

    // variables to track x-coordinate on both sides
    float curx1 = v2.x;
    float curx2 = v2.x;

    // scan from bottom to top, drawing flat lines across
    for (int scanlineY = v2.y; scanlineY > v0.y; --scanlineY) {
        drawHorizontalLine(Vec2((int)curx1, scanlineY), Vec2((int)curx2, scanlineY), image, c);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

// draw a triangle to the given image with the given color
void drawTriangle(Triangle tri, TGA& image, ColorRGB c) {
    // draw three lines
    if(glFillMode==LINE){
        drawLine(tri.v0,tri.v1,image,c);
        drawLine(tri.v1,tri.v2,image,c);
        drawLine(tri.v2,tri.v0,image,c);
    }
    // draw filled triangle using standard algorithm
    else if (glFillMode==FILL) {
        // sort points based on y value
        tri.sortPointsByHeight();

        Vec2 v0 = tri.v0, v1 = tri.v1, v2 = tri.v2;

        // check for trivial cases
        if (v1.y == v2.y) {
            drawFlatBottomTriangle(tri, image, c);
        } else if (v0.y == v1.y) {
            drawFlatTopTriangle(tri, image, c);
        }
        // divide triangle into flat-bottom and flat-top, then draw two halves
        else {
            // find point that divides triangle into flatbottom and flattop triangles
            // y3 = y1
            // x3 = x0 + ((y1 - y0) / (y2 - y0)) * (x2 - x0)
            Vec2 v3((int)(v0.x + ((float)(v1.y - v0.y) / (float)(v2.y - v0.y)) * (v2.x - v0.x)), v1.y);
            Triangle flatBottom(v0, v1, v3);
            Triangle flatTop(v1, v3, v2);
            drawFlatBottomTriangle(flatBottom, image, c);
            drawFlatTopTriangle(flatTop, image, c);
        }

    }
}

// Main
int main(){
    // A sample of color(s) to play with
    ColorRGB red { 255, 0, 0 };
    ColorRGB ocean { 14, 71, 73 };
    ColorRGB lime { 149, 198, 35 };
    ColorRGB flame { 229, 88, 18 };
    
    // Points for our Line
    Vec2 line[2] = {Vec2(0,0), Vec2(100,100)};

    // Set the fill mode
    glPolygonMode(FILL);

    // Draw a line
    drawLine(line[0],line[1],canvas,red);

    // Data for our triangle
    Triangle t0(Vec2(160,60),Vec2(150,10),Vec2(75,190));

    // Draw a triangle
    drawTriangle(t0,canvas,ocean);

    // draw more triangles
    Triangle t1(Vec2(20,250),Vec2(250, 175),Vec2(300,300));
    drawTriangle(t1,canvas,lime);
    Triangle t2(Vec2(225,30),Vec2(290, 125),Vec2(250,150));
    drawTriangle(t2,canvas,flame);

    // Output the final image
    canvas.outputTGAImage("graphics_lab2.ppm");

    return 0;
}
