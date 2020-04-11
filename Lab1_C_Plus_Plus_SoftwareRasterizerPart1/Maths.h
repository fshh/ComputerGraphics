#ifndef MATHS_H
#define MATHS_H

/** @file Maths.h
 *  @brief Structs with math data types
 *  
 *  Note this is implemented as a header only library.
 *  This is to make this code easy to be shared.
 *
 *
 *  @author Mike Shah
 *  @bug No known bugs.
 */

#include <iostream>
#include <string>

// Structure for plotting integer points.
struct Vec2{
    int x,y;
    // Default Constructor   
    Vec2(){
        x = y = 0;
    }
    // Constructor with two arguments.
    Vec2(int _x, int _y): x{_x},y{_y} {
    }
    // Operator Overloads
    // Below is a list of operations vectors can perform
    // This is a technique known as operating overloading.
    // Add operator
    Vec2 operator+(const Vec2& a) const{
        Vec2 result;
        result.x = this->x + a.x;
        result.y = this->y + a.y;
        return result;
    } 
    // Subtract operator
    Vec2 operator-(const Vec2& a) const{
        Vec2 result;
        result.x = this->x - a.x;
        result.y = this->y - a.y;
        return result;
    }
    // Scalar Multiplication
    Vec2 operator*(const float& a) const{
        Vec2 result;
        result.x = this->x * a;
        result.y = this->y * a;
        return result;
    }
    std::string toStr() {
        return std::string("(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")");
    }
};

struct Triangle {
private:
    // swap v0 and v1 if v0 has larger y-value than v1
    void swapIfHigher(Vec2& v0, Vec2& v1) {
        if (v0.y > v1.y) {
            std::swap(v0, v1);
        }
    }

public:
    Vec2 v0, v1, v2;

    Triangle(Vec2 _v0, Vec2 _v1, Vec2 _v2): v0{_v0}, v1{_v1}, v2{_v2} { }

    // sort the points by height using a Bose-Nelson optimized sorting network, since we always know n=3
    // http://pages.ripco.net/~jgamble/nw.html
    void sortPointsByHeight() {
        // don't bother sorting if already sorted
        if (this->v0.y <= this->v1.y && this->v1.y <= this->v2.y) {
            return;
        }

        // sort points
        swapIfHigher(this->v1, this->v2);
        swapIfHigher(this->v0, this->v2);
        swapIfHigher(this->v0, this->v1);
    }

    void print() {
        std::cout << "Triangle: " << this->v0.toStr() << ", " << this->v1.toStr() << ", " << this->v2.toStr() << std::endl;
    }
};




#endif
