#ifndef __SDL2VIDEO_H__
#define __SDL2VIDEO_H__

#include "SDL.h"

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    //void *buffer;
    SDL_Surface *surface;
    SDL_Joystick *joystick;
} sdl2video_data_type;

extern sdl2video_data_type sdl2video_data;

extern int sdl2video_init();
extern int sdl2video_openWindow();
extern void sdl2video_closeWindow();
extern void sdl2video_done();

extern void sdl2video_getSurfaceInfo( int *rshift, int *gshift, int *bshift, int *rloss, int *gloss, int *bloss );

extern int sdl2video_getWindowWidth();
extern int sdl2video_getWindowHeight();
extern int sdl2video_getWindowBPP();

extern void *sdl2video_getSurfacePixels();
extern int sdl2video_getSurfacePitch();

extern void sdl2video_lock();
extern void sdl2video_unlock();

#endif