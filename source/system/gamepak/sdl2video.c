#include "sdl2video.h"

sdl2video_data_type sdl2video_data = { 0 };

#define WIDTH 256
#define HEIGHT 240

/*********************************************/

int sdl2video_init()
{
    if ( SDL_Init( SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) ) {
        printf( "error at sdl init!\n" );
        return 1;
    }

    if ( SDL_NumJoysticks() > 0 ) {
        SDL_JoystickEventState( SDL_ENABLE );
        sdl2video_data.joystick = SDL_JoystickOpen( 0 );
    }

    return 0;
}

int sdl2video_openWindow( int width, int height, bool fullscreen )
{
    if ( fullscreen ) {
        sdl2video_data.window = SDL_CreateWindow( "nesemu2",
                                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                  0, 0,
                                                  SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL );
    }
    else {
        sdl2video_data.window = SDL_CreateWindow( "nesemu2",
                                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                  width, height,
                                                  SDL_WINDOW_OPENGL );
    }

    if ( sdl2video_data.window == 0 ) {
        return 1;
    }

    sdl2video_data.renderer = SDL_CreateRenderer( sdl2video_data.window, -1, 0 );
    if ( sdl2video_data.renderer == NULL ) {
        return 1;
    }

    sdl2video_data.texture = SDL_CreateTexture( sdl2video_data.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT );

    sdl2video_data.surface = SDL_CreateRGBSurface( 0, WIDTH, HEIGHT, 32,
                                                   0x00FF0000,
                                                   0x0000FF00,
                                                   0x000000FF,
                                                   0xFF000000 );

    if ( sdl2video_data.surface == NULL ) {
        return 1;
    }

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );
    SDL_RenderSetLogicalSize( sdl2video_data.renderer, 640, 480 );

    SDL_SetRenderDrawColor( sdl2video_data.renderer, 0, 0, 0, 255 );
    SDL_RenderClear( sdl2video_data.renderer );


    SDL_ShowCursor( 0 );

    return 0;
}

/*********************************************/
void sdl2video_closeWindow()
{
    SDL_FreeSurface( sdl2video_data.surface );

    SDL_DestroyTexture( sdl2video_data.texture );
    SDL_DestroyRenderer( sdl2video_data.renderer );
    SDL_DestroyWindow( sdl2video_data.window );
}

/*********************************************/
void sdl2video_done()
{
    SDL_ShowCursor( 1 );

    if ( SDL_JoystickGetAttached( sdl2video_data.joystick ) )
        SDL_JoystickClose( sdl2video_data.joystick );

    SDL_Quit();
}

/*********************************************/
void sdl2video_getSurfaceInfo( int *rshift, int *gshift, int *bshift, int *rloss, int *gloss, int *bloss )
{
    //SDL_PixelFormat *pf = (sdl2video_data.texture)->format;

    //*rshift = 16;
    //*gshift = 8;
    //*bshift = 32;
    *rshift = 16;
    *gshift = 8;
    *bshift = 0;

    *rloss = 1;
    *gloss = 1;
    *bloss = 1;

    //SDL_PixelFormat *pf = s->format;

    //log_printf( "get_surface_info:  sdl surface info:\n" );
    //log_printf( "  bits per pixel:  %d\n", pf->BitsPerPixel );
    //log_printf( "    red:    mask:  %08X    shift:  %d    loss: %d\n", pf->Rmask, pf->Rshift, pf->Rloss );
    //log_printf( "    green:  mask:  %08X    shift:  %d    loss: %d\n", pf->Gmask, pf->Gshift, pf->Gloss );
    //log_printf( "    blue:   mask:  %08X    shift:  %d    loss: %d\n", pf->Bmask, pf->Bshift, pf->Bloss );

    //rshift = pf->Rshift;
    //gshift = pf->Gshift;
    //bshift = pf->Bshift;
    //rloss = pf->Rloss;
    //gloss = pf->Gloss;
    //bloss = pf->Bloss;
}

/*********************************************/
int sdl2video_getWindowWidth()
{
    return 640;
}

/*********************************************/
int sdl2video_getWindowHeight()
{
    return 480;
}

/*********************************************/
int sdl2video_getWindowBPP()
{
    return 32;
}

/*********************************************/
void *sdl2video_getSurfacePixels()
{
    return sdl2video_data.surface->pixels;
}

/*********************************************/
int sdl2video_getSurfacePitch()
{
    return sdl2video_data.surface->pitch;
}

/*********************************************/
void sdl2video_lock()
{
    SDL_LockSurface( sdl2video_data.surface );
}

/*********************************************/
void sdl2video_unlock()
{
    //flip buffers and unlock surface
    SDL_UpdateTexture( sdl2video_data.texture, NULL, sdl2video_data.surface->pixels, sdl2video_data.surface->pitch );
    SDL_RenderClear( sdl2video_data.renderer );
    SDL_RenderCopy( sdl2video_data.renderer, sdl2video_data.texture, NULL, NULL );
    SDL_RenderPresent( sdl2video_data.renderer );
    SDL_UnlockSurface( sdl2video_data.surface );
}
