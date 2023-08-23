#pragma once
#include <math.h>
class vi2d{
public:
    vi2d():x(0),y(0){}
    vi2d(int x,int y){
        this->x = x;
        this->y = y;
    }
    vi2d operator+(vi2d b) {return vi2d(x+b.x, y+b.y);}
    vi2d operator-(vi2d b) {return vi2d(x-b.x, y-b.y);}
    vi2d operator*(int b) {return vi2d(x*b,y*b);}
    vi2d operator/(int b) {
        if(b==0) return vi2d(0,0);
        return vi2d(x/b,y/b);
    }
    float getDist(vi2d a, vi2d b){
        return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
    }
    int x,y;
};