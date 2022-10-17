#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <X11/X.h>
#include "decode.h"

#define BSIZEX 0.1f
#define BSIZEY 0.1f
#define NUMSCALEX 0.046875f
#define NUMSCALEY 0.0625f
#define DRL(x) x + 0.375f
#define USER_IMAGE_SIZE 2048.0f
#define USER_IMAGE_SIZE_INT 2048
#define PLOD_RAMKA 0
#define PLOD_RAMKA2 8
#define PLOD_BUTTON 24
#define PLOD_BUTTONP 16
#define PLOD_POS_EDIT 32
#define PLOD_ANIM_POS 40
#define PLOD_ANIM_ROT 48
#define PLOD_FULL 56
#define SETRECT(from, bias) rectTex[0]=from[bias];rectTex[1]=from[bias+1];rectTex[2]=from[bias+2];rectTex[3]=from[bias+3];rectTex[4]=from[bias+4];rectTex[5]=from[bias+5];rectTex[6]=from[bias+6];rectTex[7]=from[bias+7]
#define SETCOORD(startx, starty, scalex, scaley) rectCoord[0]=startx; rectCoord[1]=starty; rectCoord[2]=startx+scalex; rectCoord[3]=starty; rectCoord[4]=startx+scalex; rectCoord[5]=starty+scaley; rectCoord[6]=startx; rectCoord[7]=starty+scaley
#define VEC2(qx,qy) (vec2){qx,qy}
#define GET_FROM_MARKDOWN(img) markdown[img], markdown[img+1], markdown[img+2], markdown[img+3], markdown[img+4], markdown[img+5], markdown[img+6], markdown[img+7]
#define GET_PLOD_MARKDOWN(img) plod_markdown[img], plod_markdown[img+1], plod_markdown[img+2], plod_markdown[img+3], plod_markdown[img+4], plod_markdown[img+5], plod_markdown[img+6], plod_markdown[img+7]
#define GLROTATE(deg) glRotatef(-deg, 0.0f, 0.0f, 1.0f)
#define GLTRANSLATE(dx, dy) glTranslatef(-dx, -dy, 0.0f)
#define IMG_PART(startx, starty, scalex, scaley) startx, starty+scaley, startx+scalex, starty+scaley, startx+scalex, starty, startx, starty
#define RAD(deg) deg * (3.141593f / 180.0f)
#define ASSERT(expr) if((expr) == NULL) puts("null")
#define PRINT(x) printf("%p\n", x)
#define PRINT_SELECTION() printf("layer: %d object: %d\n", selected.layer, selected.obj)

typedef struct{
    float x, y;
}vec2;

typedef struct{
  float m00, m01;
  float m10, m11;
} mat2;

typedef struct{
    unsigned char active;
    float dt_rot;
    unsigned int time;
    vec2 pos;
}anim;

typedef struct{
    unsigned int mc;
    float rotation;
    vec2 pos;
    vec2 scale;
    unsigned int isNull;
    anim animation;
    unsigned int is_animated;
} object;

typedef struct{
    object *objects;
    object *selected_object;
    int sel_obj;
    unsigned int lenght;
    unsigned int has_empty;
    unsigned int ram_size;
    unsigned int objects_pos;
}layer;

typedef struct{
    int layer;
    int obj;
    int room;
}selection;

typedef struct{
    float verts[8];
    float parralax_level;
    unsigned char img_code;
}back_obj;

typedef struct{
    vec2 pos;
    float rot;
    vec2 scale;
    unsigned int img_code;
    unsigned int is_static;
    anim a;
}platform;

typedef struct{
    back_obj background[64];
    platform foreground[64];
}subscene;

//static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

GLuint texture, user_texture;
Display   *dpy;
Window     win;

unsigned int toast;
//char out[8];
float rectCoord[8];
float rectTex[8];
float sS;
float biasx, biasy;
/*float guiB[] = {
    -0.8f, 0.8f,
    -0.7f, 0.8f,
    -0.6f, 0.8f,
    -0.5f, 0.8f,
    -0.4f, 0.8f,
    -0.3f, 0.8f,
    -0.2f, 0.8f,
    -0.1f, 0.8f,

};*/
//unsigned int bcount = 16;
float stpsx, stpsy;
static const float plod_markdown[] = {
0.000000f,0.250000f,0.250000f,0.250000f,0.250000f,0.000000f,0.000000f,0.000000f,
0.250000f,0.250000f,0.500000f,0.250000f,0.500000f,0.000000f,0.250000f,0.000000f,
0.500000f,0.250000f,0.750000f,0.250000f,0.750000f,0.000000f,0.500000f,0.000000f,
0.750000f,0.250000f,1.000000f,0.250000f,1.000000f,0.000000f,0.750000f,0.000000f,
0.000000f,0.500000f,0.250000f,0.500000f,0.250000f,0.250000f,0.000000f,0.250000f,
0.250000f,0.500000f,0.500000f,0.500000f,0.500000f,0.250000f,0.250000f,0.250000f,
0.500000f,0.500000f,0.750000f,0.500000f,0.750000f,0.250000f,0.500000f,0.250000f,
0,1,1,1,1,0,0,0
};
unsigned int markdown_len = 18;
static const float markdown[] = {
0.000000f,0.375000f,0.250000f,0.375000f,0.250000f,0.000000f,0.000000f,0.000000f,
0.250000f,0.250000f,0.625000f,0.250000f,0.625000f,0.000000f,0.250000f,0.000000f,
0.625000f,0.250000f,0.875000f,0.250000f,0.875000f,0.000000f,0.625000f,0.000000f,
0.250000f,0.500000f,0.625000f,0.500000f,0.625000f,0.250000f,0.250000f,0.250000f,
0.625000f,0.500000f,0.875000f,0.500000f,0.875000f,0.250000f,0.625000f,0.250000f,
0.000000f,0.625000f,0.250000f,0.625000f,0.250000f,0.375000f,0.000000f,0.375000f,
0.000000f,0.875000f,0.250000f,0.875000f,0.250000f,0.625000f,0.000000f,0.625000f,
0.250000f,0.875000f,0.500000f,0.875000f,0.500000f,0.625000f,0.250000f,0.625000f,
0.500000f,0.875000f,0.750000f,0.875000f,0.750000f,0.625000f,0.500000f,0.625000f,
0.750000f,0.875000f,1.000000f,0.875000f,1.000000f,0.625000f,0.750000f,0.625000f,
0.875000f,0.250000f,1.000000f,0.250000f,1.000000f,0.000000f,0.875000f,0.000000f,
0.875000f,0.500000f,1.000000f,0.500000f,1.000000f,0.250000f,0.875000f,0.250000f,
0.250000f,0.625000f,0.375000f,0.625000f,0.375000f,0.500000f,0.250000f,0.500000f,
0.375000f,0.625000f,0.500000f,0.625000f,0.500000f,0.500000f,0.375000f,0.500000f,
0.500000f,0.531250f,0.531250f,0.531250f,0.531250f,0.500000f,0.500000f,0.500000f,
0.531250f,0.531250f,0.562500f,0.531250f,0.562500f,0.500000f,0.531250f,0.500000f,
0.562500f,0.531250f,0.593750f,0.531250f,0.593750f,0.500000f,0.562500f,0.500000f,
0.000000f,1.000000f,0.625000f,1.000000f,0.625000f,0.875000f,0.000000f,0.875000f
};
typedef struct{
    layer ierarchy[10];
    unsigned int inactt_obj;
    unsigned int pos_move;
    unsigned int is_layer_move;
    unsigned int selected_lay_code;
    subscene mainscene;
    layer *selected_layer;
    vec2 coords;
}room;
unsigned int selected_markdown;
float g_scale = 1.0f;
vec2 cam_pos;
selection selected = {0, -1, 0};
room rooms[4] = {};
unsigned int rooms_len = 1;
room *selected_room;

#endif // MAIN_H_INCLUDED
