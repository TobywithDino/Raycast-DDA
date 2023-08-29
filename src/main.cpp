#include <stdio.h>
#include <SDL2/SDL.h>
#include "headers/drawCircle.h"
#include "headers/vf2d.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 704
#define FPS 60
#define GRID_SIZE 32
#define MAP_WIDTH (WINDOW_WIDTH/GRID_SIZE)
#define MAP_HEIGHT (WINDOW_HEIGHT/GRID_SIZE)

SDL_Renderer* renderer;
SDL_Window* window;
bool isRunning=true;
Uint64 lastTime=0;
int deltaTime=0;

bool left,right,up,down,drawBlock,lb,rb;
bool foundIntersectionPoint;
int pV = 500;
vf2d pPos=vf2d(0,0);
vf2d mPos=vf2d(0,0);
vf2d iPos=vf2d(0,0);

int map[MAP_WIDTH*MAP_HEIGHT]={0};

int getMapIndex(vf2d pos){
    int x = (int)pos.x / GRID_SIZE;
    int y = (int)pos.y / GRID_SIZE;
    int index = x + y * MAP_WIDTH;
    return index;
}

vf2d getMapPos(vf2d pos){
    int x = (int)pos.x / GRID_SIZE;
    int y = (int)pos.y / GRID_SIZE;
    return vf2d(x,y);
}

void handleEvent(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch (e.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_MOUSEMOTION:
            mPos.x = e.button.x;
            mPos.y = e.button.y;
            break;
        case SDL_KEYDOWN:
            if(e.key.keysym.scancode == SDL_SCANCODE_A) left = true;
            if(e.key.keysym.scancode == SDL_SCANCODE_D) right = true;
            if(e.key.keysym.scancode == SDL_SCANCODE_W) up = true;
            if(e.key.keysym.scancode == SDL_SCANCODE_S) down = true;
            break;
        case SDL_KEYUP:
            if(e.key.keysym.scancode == SDL_SCANCODE_A) left = false;
            if(e.key.keysym.scancode == SDL_SCANCODE_D) right = false;
            if(e.key.keysym.scancode == SDL_SCANCODE_W) up = false;
            if(e.key.keysym.scancode == SDL_SCANCODE_S) down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(e.button.button == SDL_BUTTON_LEFT) lb = true;
            if(e.button.button == SDL_BUTTON_RIGHT){
                rb = true;
                if(map[getMapIndex(mPos)] == 0) drawBlock = true;
                if(map[getMapIndex(mPos)] == 1) drawBlock = false;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(e.button.button == SDL_BUTTON_LEFT) lb = false;
            if(e.button.button == SDL_BUTTON_RIGHT) rb = false;
            break;
        default:
            break;
        }
    }
}

void update(){
    //--Update Player Pos--
    vf2d pD = vf2d();
    if(left) pD.x = -1;
    if(right) pD.x = 1;
    if(up) pD.y = -1;
    if(down) pD.y = 1;
    pD = pD.norm();
    pPos = pPos + pD * pV * deltaTime/1000;

    //--Update Map Value--
    if(rb){
        if(drawBlock){
            map[getMapIndex(mPos)] = 1;
        }else{
            map[getMapIndex(mPos)] = 0;
        }
    }
    
    //--Update Ray--
    if(lb){
        vf2d checkMapPos = getMapPos(pPos);
        vf2d rDir = (mPos - pPos).norm();
        vf2d unitStep = vf2d(abs(1/rDir.x), abs(1/rDir.y));
        vf2d mapMoveDir;
        vf2d length;
        vf2d offset;
        if(rDir.x > 0){
            mapMoveDir.x = 1;
            offset.x = GRID_SIZE - ((int)pPos.x % GRID_SIZE);
        }else{
            mapMoveDir.x = -1;
            offset.x = ((int)pPos.x % GRID_SIZE);
        }
        if(rDir.y > 0){
            mapMoveDir.y = 1;
            offset.y =  GRID_SIZE - ((int)pPos.y % GRID_SIZE);
        }else{
            mapMoveDir.y = -1;
            offset.y = ((int)pPos.y % GRID_SIZE);
        }

        length.x = unitStep.x * offset.x;
        length.y = unitStep.y * offset.y;
        foundIntersectionPoint = false;
        float distance;
        float maxDistance = 1500;
        while(!foundIntersectionPoint && distance < maxDistance){
            if(length.x < length.y){
                checkMapPos.x += mapMoveDir.x;
                distance = length.x;
                length.x += (unitStep.x * GRID_SIZE);
            }else{
                checkMapPos.y += mapMoveDir.y;
                distance = length.y;
                length.y += (unitStep.y * GRID_SIZE);
            }
            if(checkMapPos.x >= 0 && checkMapPos.x < MAP_WIDTH && checkMapPos.y >= 0 && checkMapPos.y < MAP_HEIGHT){
                if(map[(int)checkMapPos.x+(int)checkMapPos.y*MAP_WIDTH] == 1){
                    foundIntersectionPoint = true;
                    iPos = pPos + rDir * distance;
                }
            }
        }
    }
}

void render(){
    SDL_SetRenderDrawColor(renderer,30,30,30,255);
    SDL_RenderClear(renderer);

    //--Draw Rect in Map
    for(int i=0;i<MAP_WIDTH*MAP_HEIGHT;i++){
        SDL_Rect rec;
        rec.w = rec.h = GRID_SIZE;
        rec.x = i%MAP_WIDTH * GRID_SIZE;
        rec.y = i/MAP_WIDTH * GRID_SIZE;
        if(map[i] == 1) SDL_SetRenderDrawColor(renderer,0,0,255,255);
        if(map[i] == 0) SDL_SetRenderDrawColor(renderer,30,30,30,255);
        SDL_RenderFillRect(renderer,&rec);
    }

    //--Draw Grid Lines--
    for(int i=0;i<WINDOW_WIDTH/GRID_SIZE;i++){
        SDL_SetRenderDrawColor(renderer,220,220,220,255);
        SDL_RenderDrawLine(renderer,i*GRID_SIZE,0,i*GRID_SIZE,WINDOW_HEIGHT);
    }
    for(int j=0;j<WINDOW_HEIGHT/GRID_SIZE;j++){
        SDL_SetRenderDrawColor(renderer,220,220,220,255);
        SDL_RenderDrawLine(renderer,0,j*GRID_SIZE,WINDOW_WIDTH,j*GRID_SIZE);
    }

    //--Draw Player--
    SDL_SetRenderDrawColor(renderer,230,0,0,255);
    SDL_RenderFillCircle(renderer,pPos.x,pPos.y,6);
    //--Draw Mouse--
    SDL_SetRenderDrawColor(renderer,255,170,0,255);
    SDL_RenderFillCircle(renderer,mPos.x,mPos.y,6);
    //--Draw Ray--
    if(lb)
    {
        SDL_SetRenderDrawColor(renderer,232,85,242,255);
        SDL_RenderDrawLine(renderer,pPos.x,pPos.y,mPos.x,mPos.y);
        if(foundIntersectionPoint){
            SDL_SetRenderDrawColor(renderer,255,255,0,255);
            SDL_RenderDrawCircle(renderer,iPos.x,iPos.y,3);
        }
    }
    SDL_RenderPresent(renderer);
}

void quit(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

int main(int argc, char** argv){
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Raycast(DDA)V2",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window) printf("Window Error : %s\n", SDL_GetError());
    renderer = SDL_CreateRenderer(window,0,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) printf("Window Error : %s\n", SDL_GetError());

    isRunning = true;
    while(isRunning){
        handleEvent();
        Uint64 currentTime = SDL_GetTicks64();
        if(currentTime - lastTime < 1000/FPS)
            continue;
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        update();
        render();
    }
    quit();
    return 0;
}