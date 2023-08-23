#include <SDL2/SDL.h>
#include <stdio.h>
#include "headers/vf2d.h"
#include "headers/vi2d.h"
#include "headers/drawCircle.h"

#define SCREEN_WIDTH 1024 
#define SCREEN_HEIGHT 576
#define GridSize 32
#define MAP_WIDTH SCREEN_WIDTH/GridSize
#define MAP_HEIGHT SCREEN_HEIGHT/GridSize
#define FPS 60

SDL_Renderer *renderer;
SDL_Window *window;
int lastFTime=0;
int elapFTime=0;
vf2d pPos=vf2d();
vf2d mPos=vf2d();
vf2d rDir=vf2d();
int pV=600;
bool left=false, right=false, up=false, down=false, pL=false, pR=false, drawing=false;
bool foundBlock=false;
vf2d intersectionPos=vf2d();
int map[MAP_WIDTH*MAP_HEIGHT] = {0};
int mapIndex=0;

void updateIndex(){
    vi2d mPos_map = vi2d(mPos.x,mPos.y);
    mPos_map = mPos_map / GridSize;
    mapIndex = mPos_map.x+mPos_map.y*MAP_WIDTH;
} 
void update(){
    // update player pos 
    vf2d dir;
    if(left) dir.x = -1;
    if(right) dir.x = 1; 
    if(up) dir.y = -1;
    if(down) dir.y = 1;
    dir = dir.norm() * ((float)pV * elapFTime / 1000);
    pPos = pPos + dir;

    // draw block
    if(pR){
        updateIndex();
        if(map[mapIndex] == 0 && drawing){
            map[mapIndex] = 1;   
        }else if(map[mapIndex] == 1 && !drawing){
            map[mapIndex] = 0;
        }
    }

    // update ray and find intersection point
    foundBlock = false;
    if(pL){
        rDir = (mPos - pPos).norm();
        vf2d rUnitStepSize = vf2d(sqrt(1+(rDir.y/rDir.x)*(rDir.y/rDir.x)),sqrt((rDir.x/rDir.y)*(rDir.x/rDir.y)+1));
        vf2d rayStep;
        vf2d rayLength;
        vi2d rayPos2Map = vi2d((int)(pPos.x/GridSize),(int)(pPos.y/GridSize));
        if(rDir.x < 0) {
            rayStep.x = -1;
            rayLength.x = (pPos.x - rayPos2Map.x*GridSize) * rUnitStepSize.x;
        }
        else{
            rayStep.x = 1;
            rayLength.x = ((rayPos2Map.x+1)*GridSize - pPos.x) * rUnitStepSize.x;
        }
        if(rDir.y < 0){
            rayStep.y = -1;
            rayLength.y = (pPos.y - rayPos2Map.y*GridSize) * rUnitStepSize.y;
        }
        else{
            rayStep.y = 1;
            rayLength.y = ((rayPos2Map.y+1)*GridSize - pPos.y) * rUnitStepSize.y;
        }   
        int maxDist = 3000;
        float dist = 0;
        while(!foundBlock && maxDist > dist){
            if(rayLength.x < rayLength.y){
                rayPos2Map.x += rayStep.x;
                dist = rayLength.x;
                rayLength.x += (rUnitStepSize.x * GridSize);
            }
            else{
                rayPos2Map.y += rayStep.y;
                dist = rayLength.y;
                rayLength.y += (rUnitStepSize.y * GridSize);
            }
            if(rayPos2Map.x >= 0 && rayPos2Map.x < MAP_WIDTH && rayPos2Map.y >= 0 && rayPos2Map.y < MAP_HEIGHT){
                if(map[rayPos2Map.x+rayPos2Map.y*MAP_WIDTH] == 1){
                    foundBlock = true;
                }
            }else{
                break;
            }
        }
        if(foundBlock){
            intersectionPos = pPos + rDir * dist;
        }
    }
}

void render(){
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // draw map
    SDL_Rect rec;
    rec.w = rec.h = GridSize;
    for(int i=0;i<MAP_HEIGHT;i++){
        for(int j=0;j<MAP_WIDTH;j++){
            rec.x = GridSize*j;
            rec.y = GridSize*i;
            if(map[i*MAP_WIDTH+j] == 0){
                SDL_SetRenderDrawColor(renderer,210,210,210,255);
                SDL_RenderFillRect(renderer, &rec);
            }
            if(map[i*MAP_WIDTH+j] == 1){
                SDL_SetRenderDrawColor(renderer,0,0,255,255);
                SDL_RenderFillRect(renderer, &rec);
            }
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
            SDL_RenderDrawRect(renderer, &rec);
        }
    }
    

    // draw player and mouse
    SDL_SetRenderDrawColor(renderer, 255,0,0,255);
    SDL_RenderFillCircle(renderer, pPos.x,pPos.y, 10); // player
    SDL_SetRenderDrawColor(renderer, 0,255,0,255);
    SDL_RenderFillCircle(renderer, mPos.x,mPos.y,10); // mouse

    // draw line between player and mouse
    if(pL){
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderDrawLine(renderer,pPos.x,pPos.y,mPos.x,mPos.y);
    }

    // draw intersection on the block
    if(foundBlock){
        SDL_SetRenderDrawColor(renderer, 255,255,0,255);
        SDL_RenderDrawCircle(renderer, intersectionPos.x,intersectionPos.y, 3);
        SDL_RenderDrawCircle(renderer, intersectionPos.x,intersectionPos.y, 4);
        SDL_RenderDrawCircle(renderer, intersectionPos.x,intersectionPos.y, 5);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Raycast DDA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(!window){
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.scancode == SDL_SCANCODE_A) left=true;
                    if(event.key.keysym.scancode == SDL_SCANCODE_D) right=true;
                    if(event.key.keysym.scancode == SDL_SCANCODE_W) up=true;
                    if(event.key.keysym.scancode == SDL_SCANCODE_S) down=true;
                    break;
                case SDL_KEYUP:
                    if(event.key.keysym.scancode == SDL_SCANCODE_A) left=false;
                    if(event.key.keysym.scancode == SDL_SCANCODE_D) right=false;
                    if(event.key.keysym.scancode == SDL_SCANCODE_W) up=false;
                    if(event.key.keysym.scancode == SDL_SCANCODE_S) down=false;
                    break;
                case SDL_MOUSEMOTION:
                    mPos.x = event.motion.x;
                    mPos.y = event.motion.y;    
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT) pL=true;
                    if(event.button.button == SDL_BUTTON_RIGHT) {
                        pR=true;
                        updateIndex();
                        map[mapIndex] == 0 ? drawing = true : drawing = false;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT) pL=false;
                    if(event.button.button == SDL_BUTTON_RIGHT) pR=false;
                    break;
                default:
                    break;
            }
        }
        if(SDL_GetTicks() - lastFTime > 1000/FPS){
            elapFTime = SDL_GetTicks() - lastFTime;
            lastFTime = SDL_GetTicks();
            update();
            render();
        }
        
    }

    return 0;
}