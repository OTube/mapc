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
#define USER_IMAGE_SIZE 4096.0f
#define USER_IMAGE_SIZE_INT 4096
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
#define GLTRANSLATE(dx, dy) glTranslatef(-dx, -dy, 0.0f)
#define IMG_PART(startx, starty, scalex, scaley) startx, starty+scaley, startx+scalex, starty+scaley, startx+scalex, starty, startx, starty
#define RAD(deg) deg * (3.141593f / 180.0f)
#define PRINT(x) printf("%p\n", x)
#define PRINT_SELECTION() printf("layer: %d object: %d\n", selected.layer, selected.obj)

#define TAG_HERO 1
#define TAG_ENEMY 2
#define TAG_GROUND 3
#define TAG_WOOD 4
#define TAG_IRON 5
#define TAG_WATER 6
#define TAG_ELECTRON 7
#define TAG_START 8
#define TAG_FINISH 9

typedef struct{
    float x, y;
}vec2;

typedef struct{
  float m00, m01;
  float m10, m11;
} mat2;

typedef struct{
    vec2 pos;
    float dt_rot;
    unsigned short time;
    unsigned char owner;
}anim;

typedef struct{
    anim animation;
    vec2 pos;
    vec2 scale;
    float rotation;
    uint is_animated;
    uint mc;
    uint tag;
    uint rigid;
} object;

typedef struct{
    object *objects;
    object *selected_object;
    int sel_obj;
    unsigned int lenght;
    unsigned int ram_size;
    unsigned int objects_pos;
}layer;

typedef struct{
    int layer;
    int obj;
    int room;
}selection;

typedef struct{
    vec2 pos;
    vec2 scale;
    float rotation;
    unsigned char is_back;
    unsigned char img_code;
}back_obj;

typedef struct{
    vec2 pos;
    vec2 scale;
    float rot;
    unsigned char rigid;
    unsigned char tag;
    unsigned char is_static;
    unsigned char img_code;
}platform;

typedef struct{
    back_obj background[64];
    platform foreground[64];
    platform owners[64];
    anim anims[64];
}subscene;

//static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

GLuint texture, user_texture;
Display   *dpy;
Window     win;

vec2 spawned_scale;
float spawned_rot = 0.0f;
anim  am;
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
0.500000f,0.500000f,0.750000f,0.500000f,0.750000f,0.250000f,0.500000f,0.250000f
};
unsigned int markdown_len = 32;
static const float markdown[] ={
0.000000f,0.125000f,0.125000f,0.125000f,0.125000f,0.000000f,0.000000f,0.000000f,
0.125000f,0.125000f,0.250000f,0.125000f,0.250000f,0.000000f,0.125000f,0.000000f,
0.250000f,0.125000f,0.375000f,0.125000f,0.375000f,0.000000f,0.250000f,0.000000f,
0.375000f,0.125000f,0.500000f,0.125000f,0.500000f,0.000000f,0.375000f,0.000000f,
0.500000f,0.125000f,0.625000f,0.125000f,0.625000f,0.000000f,0.500000f,0.000000f,
0.625000f,0.125000f,0.750000f,0.125000f,0.750000f,0.000000f,0.625000f,0.000000f,
0.750000f,0.125000f,0.875000f,0.125000f,0.875000f,0.000000f,0.750000f,0.000000f,
0.875000f,0.125000f,1.000000f,0.125000f,1.000000f,0.000000f,0.875000f,0.000000f,
0.000000f,0.312500f,0.125000f,0.312500f,0.125000f,0.125000f,0.000000f,0.125000f,
0.125000f,0.250000f,0.312500f,0.250000f,0.312500f,0.125000f,0.125000f,0.125000f,
0.312500f,0.250000f,0.437500f,0.250000f,0.437500f,0.125000f,0.312500f,0.125000f,
0.437500f,0.250000f,0.562500f,0.250000f,0.562500f,0.125000f,0.437500f,0.125000f,
0.562500f,0.250000f,0.625000f,0.250000f,0.625000f,0.125000f,0.562500f,0.125000f,
0.625000f,0.250000f,0.687500f,0.250000f,0.687500f,0.125000f,0.625000f,0.125000f,
0.687500f,0.250000f,0.750000f,0.250000f,0.750000f,0.125000f,0.687500f,0.125000f,
0.750000f,0.250000f,0.937500f,0.250000f,0.937500f,0.125000f,0.750000f,0.125000f,
0.125000f,0.312500f,0.187500f,0.312500f,0.187500f,0.250000f,0.125000f,0.250000f,
0.187500f,0.312500f,0.250000f,0.312500f,0.250000f,0.250000f,0.187500f,0.250000f,
0.250000f,0.281250f,0.437500f,0.281250f,0.437500f,0.250000f,0.250000f,0.250000f,
0.437500f,0.265625f,0.453125f,0.265625f,0.453125f,0.250000f,0.437500f,0.250000f,
0.453125f,0.265625f,0.468750f,0.265625f,0.468750f,0.250000f,0.453125f,0.250000f,
0.468750f,0.265625f,0.484375f,0.265625f,0.484375f,0.250000f,0.468750f,0.250000f,
0.484375f,0.265625f,0.500000f,0.265625f,0.500000f,0.250000f,0.484375f,0.250000f,
0.500000f,0.265625f,0.515625f,0.265625f,0.515625f,0.250000f,0.500000f,0.250000f,
0.000000f,0.500000f,0.125000f,0.500000f,0.125000f,0.312500f,0.000000f,0.312500f,
0.125000f,0.500000f,0.312500f,0.500000f,0.312500f,0.312500f,0.125000f,0.312500f,
0.312500f,0.437500f,0.500000f,0.437500f,0.500000f,0.312500f,0.312500f,0.312500f,
0.500000f,0.437500f,0.625000f,0.437500f,0.625000f,0.312500f,0.500000f,0.312500f,
0.625000f,0.437500f,0.750000f,0.437500f,0.750000f,0.312500f,0.625000f,0.312500f,
0.750000f,0.437500f,0.875000f,0.437500f,0.875000f,0.312500f,0.750000f,0.312500f,
0.875000f,0.437500f,1.000000f,0.437500f,1.000000f,0.312500f,0.875000f,0.312500f,
0.000000f,0.531250f,0.562500f,0.531250f,0.562500f,0.500000f,0.000000f,0.500000f
};
typedef struct{
    layer ierarchy[4];
    unsigned int inactt_obj;
    unsigned int pos_move;
    unsigned int is_layer_move;
    unsigned int selected_lay_code;
    layer *selected_layer;
    vec2 coords;
}room;

subscene scene;
unsigned int selected_markdown;
float g_scale = 1.0f;
vec2 cam_pos;
selection selected = {0, -1, 0};
room rooms[4] = {};
unsigned int rooms_len = 1;
room *selected_room;

#endif // MAIN_H_INCLUDED
