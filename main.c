#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>

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
#define VEC2(qx,qy) (vec2){qx,qy}
#define GET_FROM_MARKDOWN(img) markdown[img], markdown[img+1], markdown[img+2], markdown[img+3], markdown[img+4], markdown[img+5], markdown[img+6], markdown[img+7]
#define GET_PLOD_MARKDOWN(img) plod_markdown[img], plod_markdown[img+1], plod_markdown[img+2], plod_markdown[img+3], plod_markdown[img+4], plod_markdown[img+5], plod_markdown[img+6], plod_markdown[img+7]
#define GLTRANSLATE(dx, dy) glTranslatef(-dx, -dy, 0.0f)
#define IMG_PART(startx, starty, scalex, scaley) startx, starty+scaley, startx+scalex, starty+scaley, startx+scalex, starty, startx, starty
#define RAD(deg) deg * (3.141593f / 180.0f)
#define PRINT(x) printf("%p\n", x)
#define PRINT_SELECTION() printf("layer: %d object: %d\n", selected.layer, selected.obj)
#define MK_LEN 37

#define TAG_HERO 1
#define TAG_ENEMY 2
#define TAG_GROUND 3
#define TAG_WOOD 4
#define TAG_IRON 5
#define TAG_WATER 6
#define TAG_SPIKES 7
#define TAG_WEAPON 8
#define TAG_BOMB 9
#define TAG_BUTTON 10
#define TAG_SWIPE 11
#define TAG_SPHERE 12
#define TAG_SPHERE_AREA 13
#define TAG_ELECTRON 14
#define TAG_ELECTRON_AREA 15
#define TAGS_COUNT 17

#define TAG_START 253
#define TAG_FINISH 254

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
    uint32_t is_animated;
    uint32_t mc;
    uint32_t tag;
    uint32_t rigid;
    char txt[32];
    uint32_t is_txt;
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
    char gap[2];
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
    vec2 pos;
    float scale;
    uint8_t txt_len;
    char *txt;
}text;

typedef struct{
    back_obj background[64];
    platform foreground[64];
    platform owners[64];
    text txts[16];
    anim anims[64];
}subscene;

GLuint texture, user_texture;
GLFWwindow* window;
uint8_t needRedraw = GL_FALSE;

vec2 spawned_scale;
float spawned_rot = 0.0f;
anim  am;
float rectCoord[8];
float rectTex[8];
float sS;
float biasx, biasy;
float stpsx, stpsy;
int eventer_index = -1;
static const float plod_markdown[] = {
0.000000f,0.250000f,0.250000f,0.250000f,0.250000f,0.000000f,0.000000f,0.000000f,
0.250000f,0.250000f,0.500000f,0.250000f,0.500000f,0.000000f,0.250000f,0.000000f,
0.500000f,0.250000f,0.750000f,0.250000f,0.750000f,0.000000f,0.500000f,0.000000f,
0.750000f,0.250000f,1.000000f,0.250000f,1.000000f,0.000000f,0.750000f,0.000000f,
0.000000f,0.500000f,0.250000f,0.500000f,0.250000f,0.250000f,0.000000f,0.250000f,
0.250000f,0.500000f,0.500000f,0.500000f,0.500000f,0.250000f,0.250000f,0.250000f,
0.500000f,0.500000f,0.750000f,0.500000f,0.750000f,0.250000f,0.500000f,0.250000f
};
static const float markdown[] ={
0.000926f,0.072726f,0.025983f,0.072726f,0.025983f,0.014857f,0.000926f,0.014857f,
0.044481f,0.075477f,0.073494f,0.075477f,0.073494f,0.009254f,0.044481f,0.009254f,
0.090768f,0.076525f,0.131233f,0.076525f,0.131233f,0.013201f,0.090768f,0.013201f,
0.154878f,0.080298f,0.192238f,0.080298f,0.192238f,0.011338f,0.154878f,0.011338f,
0.213379f,0.081473f,0.249429f,0.081473f,0.249429f,0.018585f,0.213379f,0.018585f,
0.270419f,0.081558f,0.328911f,0.081558f,0.328911f,0.056869f,0.270419f,0.056869f,
0.347452f,0.084477f,0.388301f,0.084477f,0.388301f,0.025878f,0.347452f,0.025878f,
0.41434f,0.083876f,0.43592f,0.083876f,0.43592f,0.007134f,0.41434f,0.007134f,
0.462551f,0.082147f,0.503037f,0.082147f,0.503037f,0.001268f,0.462551f,0.001268f,
0.531565f,0.0748f,0.595193f,0.0748f,0.595193f,0.015676f,0.531565f,0.015676f,
0.623508f,0.072841f,0.69406f,0.072841f,0.69406f,0.028422f,0.623508f,0.028422f,
0.739187f,0.073985f,0.786772f,0.073985f,0.786772f,0.018512f,0.739187f,0.018512f,
0.840427f,0.073149f,0.876994f,0.073149f,0.876994f,0.014369f,0.840427f,0.014369f,
0.920389f,0.085723f,0.984376f,0.085723f,0.984376f,0.035592f,0.920389f,0.035592f,
0.011078f,0.116711f,0.085823f,0.116711f,0.085823f,0.097032f,0.011078f,0.097032f,
0.113868f,0.127261f,0.137745f,0.127261f,0.137745f,0.08957f,0.113868f,0.08957f,
0.160672f,0.126902f,0.229396f,0.126902f,0.229396f,0.09282f,0.160672f,0.09282f,
0.261846f,0.128275f,0.298175f,0.128275f,0.298175f,0.094023f,0.261846f,0.094023f,
0.327172f,0.128692f,0.366446f,0.128692f,0.366446f,0.092875f,0.327172f,0.092875f,
0.380222f,0.128997f,0.414595f,0.128997f,0.414595f,0.097109f,0.380222f,0.097109f,
0.42689f,0.128072f,0.459545f,0.128072f,0.459545f,0.097496f,0.42689f,0.097496f,
0.573468f,0.169698f,0.617661f,0.169698f,0.617661f,0.120333f,0.573468f,0.120333f,
0.627348f,0.163099f,0.666845f,0.163099f,0.666845f,0.115192f,0.627348f,0.115192f,
0.671789f,0.164403f,0.71425f,0.164403f,0.71425f,0.115106f,0.671789f,0.115106f,
0.731887f,0.165159f,0.765603f,0.165159f,0.765603f,0.119818f,0.731887f,0.119818f,
0.480138f,0.134831f,0.5292f,0.134831f,0.5292f,0.092342f,0.480138f,0.092342f,
0.780933f,0.164004f,0.844986f,0.164004f,0.844986f,0.119631f,0.780933f,0.119631f,
0.863695f,0.165369f,0.908116f,0.165369f,0.908116f,0.1127f,0.863695f,0.1127f,
0.91676f,0.165078f,0.955186f,0.165078f,0.955186f,0.115719f,0.91676f,0.115719f,
0.957728f,0.166458f,0.991556f,0.166458f,0.991556f,0.107981f,0.957728f,0.107981f,
0.018991f,0.240784f,0.026453f,0.240784f,0.026453f,0.152445f,0.018991f,0.152445f,
0.0364f,0.240788f,0.039203f,0.240788f,0.039203f,0.15218f,0.0364f,0.15218f,
0.045007f,0.241788f,0.049986f,0.241788f,0.049986f,0.153362f,0.045007f,0.153362f,
0.05722f,0.241721f,0.063312f,0.241721f,0.063312f,0.143942f,0.05722f,0.143942f,
0.068647f,0.24151f,0.075022f,0.24151f,0.075022f,0.145465f,0.068647f,0.145465f,
0.081557f,0.241428f,0.083772f,0.241428f,0.083772f,0.179574f,0.081557f,0.179574f,
0.088236f,0.240266f,0.091912f,0.240266f,0.091912f,0.146631f,0.088236f,0.146631f
};
typedef struct{
    uint8_t eventer_index;
    uint8_t passive_index;
} state_event;
typedef struct{
    layer ierarchy[5];
    state_event events[4];
    int events_len;
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
room *rooms;
unsigned int rooms_len = 1;
room *selected_room;

vec2 mat_Mult_Vec(mat2 mt, vec2 rhs){
    return VEC2(mt.m00 * rhs.x + mt.m01 * rhs.y, mt.m10 * rhs.x + mt.m11 * rhs.y);
}

void mat2_Set(mat2 *obj, float radians){
    float c = cos(radians);
    float s = sin(radians);

    obj->m00 = c; obj->m01 = -s;
    obj->m10 = s; obj->m11 =  c;
}
vec2 vec2_Plus(vec2 v1, vec2 v2){
    return VEC2(v1.x + v2.x, v1.y + v2.y);
}
vec2 vec2_Minus(vec2 v1, vec2 v2){
    return VEC2(v1.x - v2.x, v1.y - v2.y);
}
float vec2_Len(vec2 obj){
    return sqrt(obj.x * obj.x + obj.y * obj.y);
}
void vec2_Normalize(vec2 *obj){
    float len = vec2_Len(*obj);
    if(len > 0.001f){
        float invLen = 1.0f / len;
        obj->x *= invLen;
        obj->y *= invLen;
    }
}
float oabs(float par) {
    return par>0?par:-par;
}
vec2 vec2_Multi_F(vec2 v1, float mf){
    return (vec2){v1.x*mf,v1.y*mf};
}
unsigned int aabb_in_cam(float aminx, float aminy, float scx, float scy){
    if(aminx + scx < 0.0f || aminx > 2.0f) return 0;
    if(aminy + scy < 0.0f || aminy > 1.0f) return 0;
    return 1;
}
unsigned int aabb_in_aabb(float *a, float *b){
    if(a[4] < b[0] || a[0] > b[4]) return 0;
    if(a[5] < b[1] || a[1] > b[5]) return 0;
    return 1;
}
void SETCOORD(float startx, float starty, float scalex, float scaley) {
    rectCoord[0]=startx; rectCoord[1]=starty;
    rectCoord[2]=startx+scalex; rectCoord[3]=starty;
    rectCoord[4]=startx+scalex; rectCoord[5]=starty+scaley;
    rectCoord[6]=startx; rectCoord[7]=starty+scaley;
}

unsigned char *decode(char *from, unsigned int rsize){
    FILE *fl1 = fopen(from, "rb");
    if(fl1 == NULL) return NULL;
    unsigned int fsize;
    fread(&fsize, 4, 1, fl1);
    unsigned char *userdata = (unsigned char*)malloc(rsize*rsize*4);
    unsigned char s;
    unsigned char prev[4];
    unsigned int writed = 0;
    for(unsigned int readed = 0; readed < fsize;){
        fread(&s, 1, 1, fl1);
        fread(prev, 4, 1, fl1);
        for(int k = 0; k < (unsigned int)s+1; ++k){
            userdata[writed++] = prev[0];
            userdata[writed++] = prev[1];
            userdata[writed++] = prev[2];
            userdata[writed++] = prev[3];
        }
        readed += 5;
    }
    fclose(fl1);
    return userdata;
}

void redraw(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, user_texture);
  for(int f = 0; f < 5; ++f){
    layer *lay = selected_room->ierarchy + f;
      for(int e = 0; e < lay->lenght; ++e){
        object *o_ptr = lay->objects + e;
        if(!o_ptr->is_txt){
            SETRECT(markdown,o_ptr->mc);
            mat2 rotm;
            mat2_Set(&rotm, RAD(o_ptr->rotation));
            vec2 verts[4]={
                VEC2(-o_ptr->scale.x/2.0f, -o_ptr->scale.y/2.0f),
                VEC2(o_ptr->scale.x/2.0f, -o_ptr->scale.y/2.0f),
                VEC2(o_ptr->scale.x/2.0f, o_ptr->scale.y/2.0f),
                VEC2(-o_ptr->scale.x/2.0f, o_ptr->scale.y/2.0f),
            };
            for(int h = 0; h < 4; ++h){
                vec2 v = vec2_Plus(vec2_Minus(o_ptr->pos, cam_pos), mat_Mult_Vec(rotm, verts[h]));
                rectCoord[h*2] = v.x*g_scale;
                rectCoord[h*2+1] = v.y*g_scale;
            }
            glDrawArrays(GL_QUADS, 0, 4);
        }else{
            char *txt = o_ptr->txt;
            rectTex[1] = 1.000000f;
            rectTex[3] = 1.000000f;
            rectTex[5] = 0.968750f;
            rectTex[7] = 0.968750f;
            float sclx = o_ptr->scale.x;
            float scly = sclx*2.0f;
            for (uint32_t j = 0; j < o_ptr->is_txt; ++j) {
                float c = (txt[j] > '@' ? txt[j] - '7' : txt[j] - '0') * 0.015625f;
                rectTex[0] = c;
                rectTex[6] = c;
                c += 0.015625f;
                rectTex[2] = c;
                rectTex[4] = c;
                SETCOORD((o_ptr->pos.x-cam_pos.x + j * sclx)*g_scale, (o_ptr->pos.y-cam_pos.y)*g_scale, sclx*g_scale, scly*g_scale);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
   }
  }
  if(selected.obj < 0){
      for(int e = 0; e < MK_LEN; ++e){
        SETRECT(markdown, e*8);
        SETCOORD(1.975f, 0.9725f-e/40.0f, 0.025f, 0.025f);
        glDrawArrays(GL_QUADS, 0, 4);
      }
  }
  glBindTexture(GL_TEXTURE_2D, texture);
    SETRECT(plod_markdown,PLOD_BUTTON);
    float scalee = selected_room->coords.y - selected_room->coords.x;
    if(selected.obj > -1){
        object *obj_p = selected_room->selected_layer->selected_object;
        vec2 r_bias = obj_p->pos;
        r_bias.x -= cam_pos.x;
        r_bias.y -= cam_pos.y;
        SETRECT(plod_markdown,PLOD_RAMKA);
        SETCOORD(r_bias.x-obj_p->scale.x/2.0f, r_bias.y-obj_p->scale.y/2.0f, obj_p->scale.x, obj_p->scale.y);
        for(int h= 0; h < 8; ++h){
            rectCoord[h] *= g_scale;
        }
        glDrawArrays(GL_QUADS, 0, 4);
        if(selected_room->pos_move){
            SETRECT(plod_markdown,PLOD_POS_EDIT);
            SETCOORD(r_bias.x-0.05f, r_bias.y-0.05f, 0.1f, 0.1f);
            for(int h = 0; h < 8; ++h)
                rectCoord[h] *= g_scale;
            glDrawArrays(GL_QUADS, 0, 4);
        }
        if(obj_p->is_animated){
            SETCOORD(r_bias.x-0.05f, r_bias.y-0.05f, 0.1f, 0.1f);
            for(int h = 0; h < 8; ++h) rectCoord[h] *= g_scale;
            if(obj_p->animation.dt_rot){
                SETRECT(plod_markdown,PLOD_ANIM_ROT);
            }else{
                SETRECT(plod_markdown,PLOD_ANIM_POS);
            }
            glDrawArrays(GL_QUADS, 0, 4);
            if(obj_p->animation.pos.x || obj_p->animation.pos.y){
                r_bias = vec2_Plus(r_bias, obj_p->animation.pos);
                SETCOORD(r_bias.x-0.05f, r_bias.y-0.05f, 0.1f, 0.1f);
                for(int h = 0; h < 8; ++h) rectCoord[h] *= g_scale;
                SETRECT(plod_markdown,PLOD_ANIM_POS);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    for(int k = selected_room->selected_layer->objects_pos; k < selected_room->selected_layer->lenght; ++k){
        if(selected.obj == k){
            SETRECT(plod_markdown,PLOD_BUTTON);
        }else{
            SETRECT(plod_markdown,PLOD_BUTTONP);
        }
        SETCOORD(0.0f, 0.9f-(k-selected_room->selected_layer->objects_pos)/10.0f, 0.2f, 0.1f);
        glDrawArrays(GL_QUADS, 0, 4);
    }
    for(int u = 0; u < 5; ++u){
        if(selected.layer == u){
            SETRECT(plod_markdown,PLOD_BUTTON);
        }else{
            SETRECT(plod_markdown,PLOD_BUTTONP);
        }
        SETCOORD(0.2f+u/10.0f, 0.9f, BSIZEX, BSIZEY);
        glDrawArrays(GL_QUADS, 0, 4);
    }
    for(int g = 0; g < rooms_len; ++g){
        if(selected.room == g){
            SETRECT(plod_markdown,PLOD_BUTTON);
        }else{
            SETRECT(plod_markdown,PLOD_BUTTONP);
        }
        SETCOORD(0.2f+g/10.0f, 0.0f, BSIZEX, BSIZEY);
        glDrawArrays(GL_QUADS, 0, 4);
    }
    if(selected.obj > -1){
        for(int g = 0; g < TAGS_COUNT; ++g){
            if(selected_room->selected_layer->selected_object->tag-1 == g){
                SETRECT(plod_markdown,PLOD_BUTTON);
            }else{
                SETRECT(plod_markdown,PLOD_BUTTONP);
            }
            SETCOORD(1.95f, 0.95f-g/20.0f, 0.05f, 0.05f);
            glDrawArrays(GL_QUADS, 0, 4);
        }
    }
  glfwSwapBuffers(window);
}

void load_scene(void){
    FILE *saved_scene = fopen("scene.scn", "rb");
    if(!saved_scene) return;
    unsigned short sizef;
    fread(&sizef, 2, 1, saved_scene);
    printf("scene size: %d\n", sizef);
    unsigned char len_char;
    fread(&len_char, 1, 1, saved_scene);
    rooms_len = len_char;

    for(unsigned int r = 0; r < rooms_len; ++r){
        selected_room = rooms + r;
        if(r > 0){
            for(int f = 0; f < 5; ++f){
                selected_room->ierarchy[f].objects = (object*)malloc(sizeof(object) * 20);
                memset(selected_room->ierarchy[f].objects, '\0', sizeof(object)*20);
                selected_room->ierarchy[f].lenght = 0;
                selected_room->ierarchy[f].ram_size = 20;
                selected_room->ierarchy[f].sel_obj = -1;
            }
        }
        unsigned char fore_lenght, back_lenght, anims_len, events_len, text_len;
        unsigned char conf[5];
        fread(conf, 5, 1, saved_scene);
        back_lenght = conf[0];
        fore_lenght = conf[1];
        anims_len = conf[2];
        events_len = conf[3];
        text_len = conf[4];
        printf("%d|%d|%d|%d\n", back_lenght, fore_lenght, anims_len, text_len);
        unsigned char dummy[16];
        fread(dummy, 16, 1, saved_scene);
        if(back_lenght)fread(scene.background, sizeof(back_obj) * back_lenght, 1, saved_scene);
        if(fore_lenght)fread(scene.foreground, sizeof(platform) * fore_lenght, 1, saved_scene);
        if(text_len){
            puts("txt");
            selected_room->ierarchy[3].lenght = text_len;
            for(int l = 0; l < text_len; ++l){
                object *obj = selected_room->ierarchy[3].objects + l;
                fread(&obj->pos, sizeof(vec2),1, saved_scene);
                fread(&obj->scale.y,sizeof(float),1,saved_scene);
                uint8_t _v;
                fread(&_v,sizeof(uint8_t),1,saved_scene);
                obj->is_txt = _v;
                fread(obj->txt,_v,1,saved_scene);
                obj->txt[_v+1]='\0';
                puts(obj->txt);
                obj->scale.x = obj->scale.y*0.5f;
            };
        }
        if(anims_len)  fread(scene.anims, sizeof(anim) * anims_len, 1, saved_scene);
        if(events_len) fread(selected_room->events,sizeof(state_event) * events_len, 1, saved_scene);
        selected_room->events_len = events_len;
        uint32_t len_back = 0;
        int c = 0;
        for(int f = 0; f < back_lenght; ++f){
            back_obj bo = scene.background[f];
            printf("%d|%d\n", bo.is_back, c);
            if(!bo.is_back && c != 1){
                len_back = f;
                c = 1;
            }
            object *obj = selected_room->ierarchy[c].objects + f - len_back;
            *obj = (object){
                (anim){},
                bo.pos,
                bo.scale,
                bo.rotation,
                0,
                bo.img_code*8,
                0,0,
                {},
                0
            };
        }
        if(!len_back && back_lenght && scene.background[0].is_back) len_back = back_lenght;

        selected_room->ierarchy[0].lenght = len_back;
        selected_room->ierarchy[1].lenght = back_lenght-len_back;
        int ai_ = 0;
        int s = 2;
        uint32_t len_rigid = 0;
        for(int f = 0; f < fore_lenght; ++f){
            platform plt = scene.foreground[f];
            if(plt.is_static && s != 4){
                len_rigid = f;
                s = 4;
            }
            object *obj = &selected_room->ierarchy[s].objects[f - len_rigid];
            *obj = (object){
                (anim){},
                plt.pos,
                plt.scale,
                plt.rot,
                anims_len <= f && scene.anims[ai_].owner == f,
                plt.img_code*8,
                plt.tag,
                plt.rigid,
                {},
                0
            };
            printf("%f|%f\n", plt.pos.x, plt.pos.y);
            if(anims_len <= f && scene.anims[ai_].owner == f){
                obj->animation = scene.anims[ai_];
                ++ai_;
            }
        }

        if(!len_rigid && fore_lenght && !scene.foreground[0].is_static) len_rigid = fore_lenght;
        //printf("%d|%d\n", len_rigid, fore_lenght-len_rigid);
        selected_room->ierarchy[2].lenght = len_rigid;
        selected_room->ierarchy[4].lenght = fore_lenght-len_rigid;
    }
    fclose(saved_scene);
    selected_room = rooms;
    selected.room = 0;
}

int cmp(const void *left, const void *right){
    const back_obj *a = (const back_obj *)left;
    const back_obj *b = (const back_obj *)right;
    if(b->pos.x > a->pos.x){
        return -1;
    }
    else if(a->pos.x > b->pos.x){
        return 1;
    }else{
        return 0;
    }
}

int cmp2(const void *left, const void *right){
    const platform *a = (const platform *)left;
    const platform *b = (const platform *)right;
    if(b->pos.x > a->pos.x){
        return -1;
    }
    else if(a->pos.x > b->pos.x){
        return 1;
    }else{
        return 0;
    }
}

int cmp3(const void *left, const void *right){
    const anim *a = (const anim *)left;
    const anim *b = (const anim *)right;
    if(b->owner > a->owner){
        return -1;
    }
    else if(a->owner > b->owner){
        return 1;
    }else{
        return 0;
    }
}

int cmp4(const void *left, const void *right){
    const text *a = (const text *)left;
    const text *b = (const text *)right;
    if(b->pos.x > a->pos.x){
        return -1;
    }
    else if(a->pos.x > b->pos.x){
        return 1;
    }else{
        return 0;
    }
}

object *near_body(float radius, vec2 pos){
    radius *= radius;
    float best_distance = 999999;
    object *bo = 0;
    for(unsigned int p = 0; p < selected_room->selected_layer->lenght; ++p){
        object *b = selected_room->selected_layer->objects + p;
        float dx = pos.x - b->pos.x;
        float dy = pos.y - b->pos.y;
        float distance = dx * dx + dy * dy;
        if(distance < radius && distance < best_distance){
            best_distance = distance;
            bo = b;
        }
    }
    return bo;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_SPACE:{
                double pxc, pyc;
                glfwGetCursorPos(window, &pxc, &pyc);
                pxc /= 640.0f;
                pyc /= 640.0f;
                pyc = 1 - pyc;
                pxc /= g_scale;
                pyc /= g_scale;
                if(selected_room->selected_layer->lenght + 1 > selected_room->ierarchy[selected.layer].ram_size){
                    selected_room->selected_layer->ram_size += 20;
                    selected_room->selected_layer->objects = (object*)realloc(selected_room->selected_layer->objects,
                        sizeof(object) * selected_room->selected_layer->ram_size);
                    memset(selected_room->selected_layer->objects+selected_room->selected_layer->ram_size-20, '\0', sizeof(object)*20);
                }
                object *obj_p = selected_room->selected_layer->objects + selected_room->selected_layer->lenght;
                obj_p->rotation = spawned_rot;
                vec2 scale = VEC2(markdown[selected_markdown+2]-markdown[selected_markdown], markdown[selected_markdown+1] - markdown[selected_markdown+7]);
                //vec2_Normalize(&scale);
                float coo = scale.x/scale.y-spawned_scale.x/spawned_scale.y;
                coo = coo < 0 ? -coo : coo;
                if(coo < 0.01f)
                    scale = spawned_scale;
                else scale = vec2_Multi_F(scale, 4.0f);
                obj_p->scale = scale;
                //obj_p->pos = VEC2(cam_pos.x + pxc, cam_pos.y + pyc);
                vec2 p = VEC2(pxc + cam_pos.x, pyc + cam_pos.y);
                if(selected.layer == 4){
                    object *nearest = near_body(scale.x*1.5f, p);
                    if(nearest != 0){
                        vec2 newp = nearest->pos;
                        if(oabs(p.x - newp.x) > oabs(p.y - newp.y)){
                            int right = p.x > nearest->pos.x;
                            if(right) newp.x+=(scale.x + nearest->scale.x)*0.495f;
                            else      newp.x-=(scale.x + nearest->scale.x)*0.495f;
                        }else{
                            int up = p.y > nearest->pos.y;
                            if(up) newp.y+=(scale.y + nearest->scale.y)*0.495f;
                            else      newp.y-=(scale.y + nearest->scale.y)*0.495f;
                        }
                        p = newp;
                    }
                }
                obj_p->is_txt = 0;
                //p.x = roundf(p.x * 10) / 10;
                //p.y = roundf(p.y * 10) / 10;
                obj_p->pos = p;
                obj_p->mc = selected_markdown;
                obj_p->is_animated = 0;
                //if(am.active) obj_p->animation = am;
                obj_p->tag = TAG_GROUND;
                obj_p->rigid = 1;
                selected_room->selected_layer->selected_object = obj_p;
                selected.obj = selected_room->selected_layer->lenght++;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_V:{
                if(selected.obj != -1){
                    object *obj = selected_room->selected_layer->selected_object;
                    if(obj->is_txt){
                        obj->is_txt = 0;
                    }else{
                        obj->scale.y = 0.2f;
                        obj->scale.x = 0.1f;
                        char buffer[32] = {};
                        puts("input:");
                        scanf("%s", buffer);
                        puts(buffer);
                        for(uint32_t i = 0; i < 32; ++i){
                            if(buffer[i] == '\0'){
                                obj->is_txt = i;
                                for(uint32_t m = 0; m < i; ++m){
                                    obj->txt[m] = buffer[m];
                                }
                                break;
                            }
                        }
                    }
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_Z:{
                if(selected.obj > -1){
                    selected_room->pos_move = !selected_room->pos_move;
                    selected_room->inactt_obj = selected.obj;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_W:{
                cam_pos.y += 0.1f/g_scale;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_A:{
                cam_pos.x -= 0.1f/g_scale;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_S:{
                cam_pos.y -= 0.1f/g_scale;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_D:{
                cam_pos.x += 0.1f/g_scale;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_I:{
                g_scale *= 0.9f;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_U:{
                g_scale *= 1.1f;
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_LEFT_BRACKET:{
                if(selected_room->selected_layer->objects_pos>0){
                    --selected_room->selected_layer->objects_pos;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_RIGHT_BRACKET:{
                if(selected_room->selected_layer->objects_pos<selected_room->selected_layer->lenght)
                    ++selected_room->selected_layer->objects_pos;
                    needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_N:{
                if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated)
                    selected_room->selected_layer->selected_object->animation.time -= 10;
                break;
            }
            case GLFW_KEY_M:{
                if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated)
                    selected_room->selected_layer->selected_object->animation.time += 10;
                break;
            }
            case GLFW_KEY_F:{
                selected_room = rooms + rooms_len;
                //*selected_room = *rooms;
                selected_room->selected_layer = selected_room->ierarchy;
                selected.room = rooms_len;
                selected.obj = -1;
                selected.layer = 0;
                ++rooms_len;
                for(int f = 0; f < 5; ++f){
                    selected_room->ierarchy[f].objects = (object*)malloc(sizeof(object) * 20);
                    selected_room->ierarchy[f].lenght = 0;
                    selected_room->ierarchy[f].ram_size = 20;
                }
                selected_room->coords = VEC2(-10, 10);
                needRedraw = GL_TRUE;
                break;
            }
            case GLFW_KEY_G:{
                if(selected.obj > -1){
                    if(selected_room->selected_layer->selected_object->is_animated = !selected_room->selected_layer->selected_object->is_animated){
                        selected_room->selected_layer->selected_object->animation = (anim){.time=30};
                        am = (anim){.time=30};
                    }else{
                        selected_room->selected_layer->selected_object->is_animated = 0;
                        //am.active = 0;
                    }
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_X:{
                if(eventer_index != -1 && selected.obj != -1){
                    selected_room->events[selected_room->events_len++] = (state_event){
                        eventer_index,
                        selected.obj
                    };
                    puts("event generated");
                    eventer_index = -1;
                }else{
                    eventer_index = selected.obj;
                }
                break;
            }
            case GLFW_KEY_C:{
                if(selected_room->events_len>0) --selected_room->events_len;
                puts("event deleted");
                break;
            }
            case GLFW_KEY_COMMA:{
                if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated){
                    am.dt_rot = selected_room->selected_layer->selected_object->animation.dt_rot+0.05f;
                    selected_room->selected_layer->selected_object->animation.dt_rot = am.dt_rot;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_PERIOD:{
                if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated){
                    am.dt_rot = selected_room->selected_layer->selected_object->animation.dt_rot-0.05f;
                    selected_room->selected_layer->selected_object->animation.dt_rot = am.dt_rot;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_APOSTROPHE:{
                if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated){
                    glBindTexture(GL_TEXTURE_2D, user_texture);
                    object *o_ptr = selected_room->selected_layer->selected_object;
                    float save = o_ptr->rotation;
                    vec2 savep = o_ptr->pos;
                    unsigned int timerem = 0;
                    unsigned int mu_tex = 0;
                    vec2 pos_dt;
                    for(unsigned int d = 0; d < 120; ++d){
                        if(!timerem){
                            pos_dt = vec2_Multi_F(o_ptr->animation.pos, ((mu_tex = !mu_tex)?1.0f:-1.0f)/o_ptr->animation.time);
                            timerem = o_ptr->animation.time;
                        }
                        o_ptr->pos = vec2_Plus(o_ptr->pos, pos_dt);
                        o_ptr->rotation += o_ptr->animation.dt_rot;
                        glClear(GL_COLOR_BUFFER_BIT);
                        SETRECT(markdown,o_ptr->mc);
                        mat2 rotm;
                        mat2_Set(&rotm, RAD(o_ptr->rotation));
                        vec2 verts[4]={
                            VEC2(-o_ptr->scale.x/2.0f, -o_ptr->scale.y/2.0f),
                            VEC2(o_ptr->scale.x/2.0f, -o_ptr->scale.y/2.0f),
                            VEC2(o_ptr->scale.x/2.0f, o_ptr->scale.y/2.0f),
                            VEC2(-o_ptr->scale.x/2.0f, o_ptr->scale.y/2.0f),
                        };
                        for(int h = 0; h < 4; ++h){
                            vec2 v = vec2_Plus(vec2_Minus(o_ptr->pos, cam_pos), mat_Mult_Vec(rotm, verts[h]));
                            rectCoord[h*2] = v.x*g_scale;
                            rectCoord[h*2+1] = v.y*g_scale;
                        }
                        glDrawArrays(GL_QUADS, 0, 4);
                        --timerem;
                        glfwSwapBuffers(window);
                    }
                    o_ptr->rotation = save;
                    o_ptr->pos = savep;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_SLASH:{
                if(selected_room->selected_layer->selected_object->is_animated){
                    double pxc, pyc;
                    glfwGetCursorPos(window, &pxc, &pyc);
                    pxc /= 640.0f;
                    pyc /= 640.0f;
                    pyc = 1 - pyc;
                    pxc /= g_scale;
                    pyc /= g_scale;
                    vec2 rel = VEC2(pxc+cam_pos.x, pyc+cam_pos.y);
                    am.pos = vec2_Minus(rel, selected_room->selected_layer->selected_object->pos);
                    selected_room->selected_layer->selected_object->animation.pos = am.pos;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_UP:{
                if(selected.obj > -1){
                    spawned_rot = selected_room->selected_layer->selected_object->rotation + 10.0f;
                    if(spawned_rot == 180.0f) spawned_rot = -180.0f;
                    selected_room->selected_layer->selected_object->rotation = spawned_rot;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_DOWN:{
                if(selected.obj > -1){
                    spawned_rot = selected_room->selected_layer->selected_object->rotation - 10.0f;
                    if(spawned_rot == -180.0f) spawned_rot = 180.0f;
                    selected_room->selected_layer->selected_object->rotation = spawned_rot;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_LEFT:{
                if(selected.obj > -1){
                    float coo = selected_room->selected_layer->selected_object->scale.x / selected_room->selected_layer->selected_object->scale.y;
                    spawned_scale = vec2_Minus(selected_room->selected_layer->selected_object->scale, VEC2(0.02f*coo, 0.02f));
                    selected_room->selected_layer->selected_object->scale = spawned_scale;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_RIGHT:{
                if(selected.obj > -1){
                    float coo = selected_room->selected_layer->selected_object->scale.x / selected_room->selected_layer->selected_object->scale.y;
                    spawned_scale = vec2_Plus(selected_room->selected_layer->selected_object->scale, VEC2(0.02f*coo, 0.02f));
                    selected_room->selected_layer->selected_object->scale = spawned_scale;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_DELETE:{
                if(selected.obj > -1){
                    for(int m = 0; m < selected_room->events_len; ++m){
                        if(selected_room->events[m].eventer_index == selected.obj || selected_room->events[m].passive_index == selected.obj){
                            for(unsigned int i = m+1; i < selected_room->events_len; ++i)
                                selected_room->events[i-1] = selected_room->events[i];
                            --selected_room->events_len;
                        }
                    }
                    for(unsigned int i = selected.obj+1; i < selected_room->selected_layer->lenght; ++i)
                        selected_room->selected_layer->objects[i-1] = selected_room->selected_layer->objects[i];
                    --selected_room->selected_layer->lenght;
                    selected.obj = -1;
                    needRedraw = GL_TRUE;
                }
                break;
            }
            case GLFW_KEY_1:{
                puts((selected_room->selected_layer->selected_object->rigid = !selected_room->selected_layer->selected_object->rigid) ? "rigid" : "trigger");
                break;
            }
            case GLFW_KEY_P:{
                FILE *saved_scene = fopen("scene.scn", "wb");
                unsigned short sizef = 1;
                fseek(saved_scene, 2, SEEK_SET);
                unsigned char len_char = (unsigned char)rooms_len;
                fwrite(&len_char, 1, 1, saved_scene);
                for(unsigned int r = 0; r < rooms_len; ++r){
                    selected_room = rooms + r;
                    unsigned char fore_lenght = 0, back_lenght = 0, anims_len = 0, text_len = 0;
                    uint32_t text_size = 0;
                    vec2 start = {999999, 999999};
                    vec2 finish ={-999999, -999999};
                    for(int g = 0; g < 5; ++g){
                        unsigned int is_fore = g > 1;
                        if(g == 3) is_fore = 2;
                        for(int f = 0; f < selected_room->ierarchy[g].lenght; ++f){
                            object *obj = selected_room->ierarchy[g].objects + f;
                            if(is_fore == 1){
                                if(obj->tag == TAGS_COUNT-1) obj->tag = TAG_START;
                                if(obj->tag == TAGS_COUNT)obj->tag = TAG_FINISH;
                                if(obj->tag == TAG_START) start = obj->pos;
                                if(obj->tag == TAG_FINISH)finish = obj->pos;

                                scene.foreground[fore_lenght] = (platform){
                                    obj->pos,
                                    obj->scale,
                                    obj->rotation,
                                    obj->rigid,
                                    obj->tag,
                                    g == 4,
                                    obj->mc/8
                                };
                                if(obj->is_animated){
                                    puts("anim");
                                    anim a = obj->animation;
                                    scene.anims[anims_len] = a;
                                    scene.owners[anims_len] = scene.foreground[fore_lenght];
                                    ++anims_len;
                                }
                                ++fore_lenght;
                            }else if(is_fore == 0){
                                printf("is back: %d\n", g == 0);
                                scene.background[back_lenght] = (back_obj){
                                    obj->pos,
                                    obj->scale,
                                    obj->rotation,
                                    g == 0,
                                    obj->mc/8
                                };
                                ++back_lenght;
                            }else if(is_fore == 2){
                                if(obj->is_txt){
                                    text *tx = scene.txts + f;
                                    text_size += 8 + 4 + 1 + obj->is_txt;
                                    tx->scale = obj->scale.y;
                                    tx->pos = obj->pos;
                                    tx->txt_len = obj->is_txt;
                                    tx->txt = (char*)malloc(obj->is_txt);
                                    for(int l = 0; l < obj->is_txt; ++l){
                                        tx->txt[l] = obj->txt[l];
                                    }
                                    ++text_len;
                                    printf("%d\n", obj->is_txt);
                                }
                            }
                        }
                        if     (g==0)qsort(scene.background, selected_room->ierarchy[0].lenght, sizeof(back_obj), cmp);
                        else if(g==1)qsort(scene.background+selected_room->ierarchy[0].lenght, selected_room->ierarchy[1].lenght, sizeof(back_obj), cmp);
                        else if(g==2)qsort(scene.foreground, selected_room->ierarchy[2].lenght, sizeof(platform), cmp2);
                        else if(g==3)qsort(scene.txts, selected_room->ierarchy[3].lenght, text_size, cmp4);
                        else if(g==4)qsort(scene.foreground+selected_room->ierarchy[2].lenght, selected_room->ierarchy[4].lenght, sizeof(platform), cmp2);

                        //if(is_fore)fore_lenght+=selected_room->ierarchy[g].lenght;
                        //else back_lenght+=selected_room->ierarchy[0].lenght;
                    }
                    for(int i = 0; i < anims_len; ++i){
                        unsigned char *a_ = (unsigned char*)&scene.owners[i];
                        for(int j = 0; j < fore_lenght; ++j){
                            unsigned char *a1 = (unsigned char*)&scene.foreground[j];
                            unsigned char is = 1;
                            for(int k = 0; k < sizeof(platform); ++k){
                                if(a_[k] != a1[k]){
                                    is = 0;
                                    break;
                                }
                            }
                            if(is){
                                scene.anims[i].owner = j;
                            }
                        }

                    }

                    sizef += 5 + sizeof(vec2)*2 + sizeof(back_obj)*back_lenght + sizeof(platform)*fore_lenght + text_size + sizeof(anim)*anims_len + sizeof(state_event)*selected_room->events_len;
                    printf("%d, %d\n", sizeof(back_obj), sizeof(back_obj)*back_lenght);
                    printf("%d, %d\n", sizeof(platform), sizeof(platform)*fore_lenght);
                    printf("%d\n", text_size);
                    printf("%d\n", sizeof(anim)*anims_len);
                    printf("%d\n", sizeof(state_event)*selected_room->events_len);
                    unsigned char conf[5] = {back_lenght, fore_lenght, anims_len, selected_room->events_len, text_len};
                    fwrite(conf, 5, 1, saved_scene);
                    vec2 poses[2] = {start, finish};
                    fwrite(poses, sizeof(vec2) * 2, 1, saved_scene);
                    if(back_lenght) fwrite(scene.background, sizeof(back_obj) * back_lenght, 1, saved_scene);
                    if(fore_lenght) fwrite(scene.foreground, sizeof(platform) * fore_lenght, 1, saved_scene);
                    if(text_len){
                        for(int l = 0; l < text_len; ++l){
                            text *tx = scene.txts + l;
                            printf("%d|%d\n", tx->txt_len, l);
                            fwrite(&tx->pos, sizeof(vec2),1, saved_scene);
                            fwrite(&tx->scale,sizeof(float),1,saved_scene);
                            fwrite(&tx->txt_len,sizeof(uint8_t),1,saved_scene);
                            fwrite(tx->txt,tx->txt_len,1,saved_scene);
                            free(tx->txt);
                        }
                    }
                    if(anims_len){
                        printf("%d\n", anims_len);
                        qsort(scene.anims, anims_len, sizeof(anim), cmp3);
                        fwrite(scene.anims, sizeof(anim) * anims_len, 1, saved_scene);
                    }
                    if(selected_room->events_len) fwrite(selected_room->events, sizeof(state_event) * selected_room->events_len, 1, saved_scene);
                }
                fseek(saved_scene, 0, SEEK_SET);
                fwrite(&sizef, 2, 1, saved_scene);
                printf("size: %d\n", sizef);
                fclose(saved_scene);
                break;
            }
            case GLFW_KEY_ESCAPE:{
                break;
            }
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        double pxc, pyc;
        glfwGetCursorPos(window, &pxc, &pyc);
        pxc /= 640.0f;
        pyc /= 640.0f;
        pyc = 1 - pyc;
        if(pxc > -1 && pxc < 2 && pyc > -1 && pyc < 1){
            if(selected_room->pos_move){
                pxc /= g_scale;
                pyc /= g_scale;
                object *objP = selected_room->selected_layer->objects + selected_room->inactt_obj;
                objP->pos = VEC2(pxc + cam_pos.x, pyc + cam_pos.y);
                selected_room->pos_move = 0;
                needRedraw = GL_TRUE;
            }else if(pxc < 0.2f){
                unsigned int ind = 19 - (unsigned int)((pyc + 1.0f) / 0.1f) + selected_room->selected_layer->objects_pos;
                if(ind < selected_room->selected_layer->lenght){
                    selected.obj = selected.obj != ind ? ind : -1;
                    selected_room->selected_layer->selected_object = selected_room->selected_layer->objects + selected.obj;
                    needRedraw = GL_TRUE;
                }
            }else if(pxc > 1.95f){
                if(selected.obj != -1){
                    int ind = 20 - ((int)(pyc * 20));
                    selected_room->selected_layer->selected_object->tag = ind;
                    printf("selected tag: %d|%d\n", ind, selected.obj);
                    needRedraw = GL_TRUE;
                }else{
                    int ind = 39 - ((int)(pyc * 40));
                    if(ind > -1 && ind < MK_LEN)
                        selected_markdown = ind * 8;
                }
            }else if(pyc > 0.9f && pxc < 0.7f){
                int ind = (int)(pxc*10)-2;
                if(ind < 5){
                    selected_room->selected_layer->sel_obj = selected.obj;
                    //selected_room->selected_lay_code = selected.layer;
                    selected_room->selected_layer->selected_object = selected_room->selected_layer->objects + selected.obj;
                    selected.layer = ind;
                    selected.obj = selected_room->ierarchy[ind].sel_obj;
                    selected_room->selected_layer = selected_room->ierarchy + ind;
                    selected_room->selected_layer->sel_obj = selected.obj;
                    selected_room->selected_layer->selected_object = selected_room->selected_layer->objects + selected.obj;
                    needRedraw = GL_TRUE;
                }
            }else if(pyc < 0.1f && pxc < 1.2f){
                int ind = (int)(pxc*10)-2;
                if(ind < rooms_len){
                    selected.room = ind;
                    selected_room = rooms + ind;
                    //selected.obj = selected_room->selected_layer->sel_obj;
                    selected.obj = -1;
                    selected_room->selected_layer = selected_room->ierarchy;
                    selected_room->selected_layer->selected_object = selected_room->selected_layer->objects;
                    needRedraw = GL_TRUE;
                }
            }else{
                pxc += cam_pos.x*g_scale;
                pyc += cam_pos.y*g_scale;
                for(int f = 4; f > -1; --f){
                    int ex = 0;
                    for(int u = selected_room->ierarchy[f].lenght-1; u > -1; --u){
                        object *ine = selected_room->ierarchy[f].objects + u;
                        float pc0 = ine->pos.x*g_scale;
                        float pc1 = ine->pos.y*g_scale;
                        float s_v0 = ine->scale.x/2.0f*g_scale;
                        float s_v1 = ine->scale.y/2.0f*g_scale;
                        if((pxc >= pc0 - s_v0) && (pyc >= pc1 - s_v1) && (pxc <= pc0 + s_v0) &&
                            (pyc <= pc1 + s_v1)){
                            selected.obj = u;
                            selected.layer = f;
                            selected_room->selected_layer = selected_room->ierarchy + f;
                            selected_room->selected_layer->selected_object = selected_room->selected_layer->objects + u;
                            selected_room->is_layer_move = 0;
                            selected_room->pos_move = 0;
                            needRedraw = GL_TRUE;
                            ex = 1;
                            break;
                        }
                    }
                    if(ex)break;
                }
            }
        }
    }
}

int main(int argc, char **argv){
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 640, "mapc", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glClearColor(0.0, 0.0, 0.5, 0.0);
    unsigned char *data = decode("/home/oleg/Main/mapc-main/plodtex.ugg", 128);
    if(data == NULL) return 1;
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, rectCoord);
    glTexCoordPointer(2, GL_FLOAT, 0, rectTex);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);
    unsigned char *userdata = decode("/home/oleg/Main/Inkscape/gametex.ugg", USER_IMAGE_SIZE_INT);
    if(userdata == NULL) return 1;
    glGenTextures(1, &user_texture);
    glBindTexture(GL_TEXTURE_2D, user_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, USER_IMAGE_SIZE_INT, USER_IMAGE_SIZE_INT, 0, GL_RGBA, GL_UNSIGNED_BYTE, userdata);
    free(userdata);
    glOrtho(0.0f, 2.0f, -0.0f, 1.0f, 0.0f, 1.0f);
    cam_pos = VEC2(-1.0f, -0.5f);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    rooms = (room*)malloc(sizeof(room)*4);
    memset(rooms, '\0', sizeof(room)*4);
    selected_room = rooms;
    selected_room->selected_layer = selected_room->ierarchy;
    //selected_room->selected_layer->selected_object = selected_room->selected_layer->objects + selected.obj;
    for(int f = 0; f < 5; ++f){
        selected_room->ierarchy[f].objects = (object*)malloc(sizeof(object) * 20);
        memset(selected_room->ierarchy[f].objects, '\0', sizeof(object)*20);
        selected_room->ierarchy[f].lenght = 0;
        selected_room->ierarchy[f].ram_size = 20;
        selected_room->ierarchy[f].sel_obj = -1;
    }
    for(int i = 0; i < 4; ++i){
        rooms[i].events_len = 0;
        rooms[i].pos_move = 0;
    }
    load_scene();
    redraw();
    while(!glfwWindowShouldClose(window)){
        glfwWaitEvents();
        if(needRedraw){
            redraw();
            needRedraw = GL_FALSE;
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    for(int h = 0; h < rooms_len; ++h){
        selected_room = rooms + h;
        for(int f = 0; f < 5; ++f){
            free((selected_room->ierarchy + f)->objects);
        }
    }
    free(rooms);
   glfwTerminate();
   puts("exit");
  return 0;
}
