#pragma once
#include <math.h>
class vf2d{
public:
    vf2d():x(0.0),y(0.0){}
    vf2d(float x,float y){
        this->x = x;
        this->y = y;
    }
    vf2d operator+(vf2d b) {return vf2d(x+b.x, y+b.y);}
    vf2d operator-(vf2d b) {return vf2d(x-b.x, y-b.y);}
    vf2d operator*(float b) {return vf2d(x*b,y*b);}
    vf2d operator/(float b) {
        if(b==0) return vf2d(0,0);
        return vf2d(x/b,y/b);
    }
    float getDist(vf2d a, vf2d b){
        return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
    }
    vf2d norm(){
        float length = sqrt(x*x+y*y);
        if(length == 0) return vf2d(x,y);
        return vf2d(x/length,y/length);
    }
    float x,y;
};