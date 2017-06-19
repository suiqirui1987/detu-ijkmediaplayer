#include "ijksdl_vout_win_ffmpeg.h"

#include <assert.h>
#include "ijksdl/ijksdl_vout.h"
#include "ijksdl/ijksdl_vout_internal.h"
#include "ijksdl/ijksdl_container.h"
#include "ijksdl/ffmpeg/ijksdl_vout_overlay_ffmpeg.h"


typedef struct SDL_Vout_Opaque {
	int private;
	FILE *fp;
	//TODO video callback function

} SDL_Vout_Opaque;

static SDL_VoutOverlay *func_create_overlay_l(int width, int height, int frame_format, SDL_Vout *vout)
{
	return SDL_VoutFFmpeg_CreateOverlay(width, height, frame_format, vout);
}

static SDL_VoutOverlay *func_create_overlay(int width, int height, int frame_format, SDL_Vout *vout)
{
    SDL_LockMutex(vout->mutex);
    SDL_VoutOverlay *overlay = func_create_overlay_l(width, height, frame_format, vout);
    SDL_UnlockMutex(vout->mutex);
    return overlay;
}

static void func_free_l(SDL_Vout *vout)
{
    if (!vout)
        return;

    SDL_Vout_Opaque *opaque = vout->opaque;
    if (opaque) {
		//TODO,free resource
    }

    SDL_Vout_FreeInternal(vout);
}

#define OUTPUT_YUV420P
static int func_display_overlay_l(SDL_Vout *vout, SDL_VoutOverlay *overlay)
{
    SDL_Vout_Opaque *opaque = vout->opaque;

    if (!overlay) {
        ALOGE("func_display_overlay_l: NULL overlay");
        return -1;
    }

    if (overlay->w <= 0 || overlay->h <= 0) {
        ALOGE("func_display_overlay_l: invalid overlay dimensions(%d, %d)", overlay->w, overlay->h);
        return -1;
    }

	//TODO, callback frame to user
#ifdef OUTPUT_YUV420P
	int y_size = overlay->w * overlay->h;
	fwrite(overlay->pixels[0], 1, y_size, opaque->fp);		//Y 
	fwrite(overlay->pixels[1], 1, y_size / 4, opaque->fp);  //U
	fwrite(overlay->pixels[2], 1, y_size / 4, opaque->fp);  //V
	fflush(opaque->fp);
	SDL_Delay(30);
#endif

    return 0; 
}

static int func_display_overlay(SDL_Vout *vout, SDL_VoutOverlay *overlay)
{
    SDL_LockMutex(vout->mutex);
    int retval = func_display_overlay_l(vout, overlay);
    SDL_UnlockMutex(vout->mutex);
    return retval;
}

SDL_Vout *SDL_VoutWin_CreateForWindows()
{
    SDL_Vout *vout = SDL_Vout_CreateInternal(sizeof(SDL_Vout_Opaque));
    if (!vout)
        return NULL;

    SDL_Vout_Opaque *opaque = vout->opaque;
	opaque->fp = fopen("output.yuv", "wb+");

    vout->create_overlay  = func_create_overlay;
    vout->free_l          = func_free_l;
    vout->display_overlay = func_display_overlay;

    return vout;
}

void SDL_VoutWin_SetVideoDataCallback()
{

}