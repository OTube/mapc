#include "main.h"

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
void redraw(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, user_texture);

  for(int f = 0; f < 4; ++f){
    layer *lay = selected_room->ierarchy + f;
      for(int e = 0; e < lay->lenght; ++e){
        object *o_ptr = lay->objects + e;
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
   }
  }
  if(selected.obj < 0){
      for(int e = 0; e < markdown_len; ++e){
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
    for(int u = 0; u < 4; ++u){
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
        for(int g = 0; g < TAG_FINISH; ++g){
            if(selected_room->selected_layer->selected_object->tag-1 == g){
                SETRECT(plod_markdown,PLOD_BUTTON);
            }else{
                SETRECT(plod_markdown,PLOD_BUTTONP);
            }
            SETCOORD(1.9f, 0.9f-g/10.0f, BSIZEX, BSIZEY);
            glDrawArrays(GL_QUADS, 0, 4);
        }
    }
  glXSwapBuffers(dpy, win);
}

void load_scene(void){
    FILE *saved_scene = fopen("/home/oleg/Main/cbps/CardCreator/scene.scn", "rb");
    if(!saved_scene) return;
    unsigned short sizef;
    fread(&sizef, 2, 1, saved_scene);
    unsigned char len_char;
    fread(&len_char, 1, 1, saved_scene);
    rooms_len = len_char;

    for(unsigned int r = 0; r < rooms_len; ++r){
        selected_room = rooms + r;
        if(r > 0){
            for(int f = 0; f < 4; ++f){
                selected_room->ierarchy[f].objects = (object*)malloc(sizeof(object) * 20);
                selected_room->ierarchy[f].lenght = 0;
                selected_room->ierarchy[f].ram_size = 20;
                selected_room->ierarchy[f].sel_obj = -1;
            }
        }
        unsigned char fore_lenght = 0, back_lenght = 0, anims_len = 0;
        unsigned char conf[3];
        fread(conf, 3, 1, saved_scene);
        back_lenght = conf[0];
        fore_lenght = conf[1];
        anims_len = conf[2];
        unsigned char dummy[16];
        fread(dummy, 16, 1, saved_scene);
        if(back_lenght)fread(scene.background, sizeof(back_obj) * back_lenght, 1, saved_scene);
        if(fore_lenght)fread(scene.foreground, sizeof(platform) * fore_lenght, 1, saved_scene);
        if(anims_len)  fread(scene.anims, sizeof(anim) * anims_len, 1, saved_scene);
        //fread(&is_, 1, 1, saved_scene);
        uint len_back = 0;
        int c = 0;
        for(int f = 0; f < back_lenght; ++f){
            back_obj bo = scene.background[f];
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
                0,0
            };
        }
        selected_room->ierarchy[0].lenght = len_back;
        selected_room->ierarchy[1].lenght = back_lenght-len_back;
        int ai_ = 0;
        int s = 2;
        uint len_rigid = 0;
        for(int f = 0; f < fore_lenght; ++f){
            platform plt = scene.foreground[f];
            if(plt.is_static && s != 3){
                len_rigid = f;
                s = 3;
            }
            object *obj = selected_room->ierarchy[s].objects + f - len_rigid;
            *obj = (object){
                (anim){},
                plt.pos,
                plt.scale,
                plt.rot,
                scene.anims[ai_].owner == f,
                plt.img_code*8,
                plt.tag,
                plt.rigid
            };
            if(scene.anims[ai_].owner == f){
                obj->animation = scene.anims[ai_];
                ++ai_;
            }
        }
        //printf("%d|%d\n", len_rigid, fore_lenght-len_rigid);
        selected_room->ierarchy[2].lenght = len_rigid;
        selected_room->ierarchy[3].lenght = fore_lenght-len_rigid;
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

int main(int argc, char **argv){
    XVisualInfo         *vi;
    Colormap             cmap;
    XSetWindowAttributes swa;
    GLXContext           cx;
    XEvent               event;
    GLboolean            needRedraw = GL_FALSE;
    int                  dummy;

    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) return 1;

    if(!glXQueryExtension(dpy, &dummy, &dummy)) return 1;

    vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
    if (vi == NULL) return 1;
    if(vi->class != TrueColor) return 1;

    cx = glXCreateContext(dpy, vi, None, GL_TRUE);
    if (cx == NULL) return 1;

    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = KeyPressMask    | ExposureMask
                 | ButtonPressMask | StructureNotifyMask;
    win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0,
                      1280, 640, 0, vi->depth, InputOutput, vi->visual,
                      CWBorderPixel | CWColormap | CWEventMask, &swa);
    XSetStandardProperties(dpy, win, "main", "main", None,
                         argv, argc, NULL);

    glXMakeCurrent(dpy, win, cx);

    XMapWindow(dpy, win);

    glClearColor(0.0, 0.0, 0.5, 0.0);

    unsigned char *data;
    FILE *fl = fopen("/home/oleg/Desktop/plodtex.utx", "rb");
    if(fl == NULL) return 1;
    data = (unsigned char*)malloc(128 * 128 * 4 * sizeof(unsigned char));
    fread(data, 128 * 128 * 4, 1, fl);
    fclose(fl);
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
    unsigned char *userdata = decode("/home/oleg/Desktop/gametex.ugg", USER_IMAGE_SIZE_INT, 1739650); //(unsigned char*)malloc(USER_IMAGE_SIZE_INT * USER_IMAGE_SIZE_INT * 4 * sizeof(unsigned char));
    if(userdata == NULL) return 1;
    glGenTextures(1, &user_texture);
    glBindTexture(GL_TEXTURE_2D, user_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, USER_IMAGE_SIZE_INT, USER_IMAGE_SIZE_INT, 0, GL_RGBA, GL_UNSIGNED_BYTE, userdata);
    free(userdata);
    cam_pos = VEC2(-1.0f, -0.5f);
    selected_room = rooms + selected.room;
    selected_room->selected_layer = selected_room->ierarchy;
    //selected_room->selected_layer->selected_object = selected_room->selected_layer->objects + selected.obj;
    for(int f = 0; f < 4; ++f){
        selected_room->ierarchy[f].objects = (object*)malloc(sizeof(object) * 20);
        selected_room->ierarchy[f].lenght = 0;
        selected_room->ierarchy[f].ram_size = 20;
        selected_room->ierarchy[f].sel_obj = -1;
    }
    load_scene();
    while (1){
    do{
      XNextEvent(dpy, &event);
      switch (event.type){
        case KeyPress:{
            switch(event.xkey.keycode){
                case 65:{
                    float pxc = (event.xbutton.x - biasx) / sS;
                    float pyc = 1.0f - (event.xbutton.y - biasy) / sS;
                    pxc /= g_scale;
                    pyc /= g_scale;
                    if(selected_room->selected_layer->lenght + 1 > selected_room->ierarchy[selected.layer].ram_size){
                        selected_room->selected_layer->ram_size += 20;
                        selected_room->selected_layer->objects = (object*)realloc(selected_room->selected_layer->objects,
                            sizeof(object) * selected_room->selected_layer->ram_size);
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
                    obj_p->pos = VEC2(pxc + cam_pos.x, pyc + cam_pos.y);
                    obj_p->mc = selected_markdown;
                    //if(am.active) obj_p->animation = am;
                    obj_p->tag = TAG_GROUND;
                    obj_p->rigid = 1;
                    selected_room->selected_layer->selected_object = obj_p;
                    selected.obj = selected_room->selected_layer->lenght++;
                    needRedraw = GL_TRUE;
                    break;
                }
                case 52:{
                    if(selected.obj > -1){
                        selected_room->pos_move = !selected_room->pos_move;
                        selected_room->inactt_obj = selected.obj;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 25:{
                    cam_pos.y += 0.1f/g_scale;
                    needRedraw = GL_TRUE;
                    break;
                }
                case 38:{
                    cam_pos.x -= 0.1f/g_scale;
                    needRedraw = GL_TRUE;
                    break;
                }
                case 39:{
                    cam_pos.y -= 0.1f/g_scale;
                    needRedraw = GL_TRUE;
                    break;
                }
                case 40:{
                    cam_pos.x += 0.1f/g_scale;
                    needRedraw = GL_TRUE;
                    break;
                }
                /*case 27:{
                    glColor3f(0,0,0);
                    glClearColor(0.97,0.32,0,0);
                    needRedraw = GL_TRUE;
                    break;
                }*/
                case 30:{
                    g_scale *= 0.9f;
                    needRedraw = GL_TRUE;
                    break;
                }
                case 31:{
                    g_scale *= 1.1f;
                    needRedraw = GL_TRUE;
                    break;
                }
                case 34:{
                    if(selected_room->selected_layer->objects_pos>0){
                        --selected_room->selected_layer->objects_pos;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 35:{
                    if(selected_room->selected_layer->objects_pos<selected_room->selected_layer->lenght)
                        ++selected_room->selected_layer->objects_pos;
                        needRedraw = GL_TRUE;
                    break;
                }
                case 57:{
                    if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated)
                        selected_room->selected_layer->selected_object->animation.time -= 10;
                    break;
                }
                case 58:{
                    if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated)
                        selected_room->selected_layer->selected_object->animation.time += 10;
                    break;
                }
                case 41:{
                    selected_room = rooms + rooms_len;
                    //*selected_room = *rooms;
                    selected_room->selected_layer = selected_room->ierarchy;
                    selected.room = rooms_len;
                    selected.obj = -1;
                    selected.layer = 0;
                    ++rooms_len;
                    for(int f = 0; f < 3; ++f){
                        selected_room->ierarchy[f].objects = (object*)malloc(sizeof(object) * 20);
                        selected_room->ierarchy[f].lenght = 0;
                        selected_room->ierarchy[f].ram_size = 20;
                    }
                    selected_room->coords = VEC2(-10, 10);
                    needRedraw = GL_TRUE;
                    break;
                }
                case 42:{
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
                case 59:{
                    if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated){
                        am.dt_rot = selected_room->selected_layer->selected_object->animation.dt_rot+0.05f;
                        selected_room->selected_layer->selected_object->animation.dt_rot = am.dt_rot;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 60:{
                    if(selected.obj > -1 && selected_room->selected_layer->selected_object->is_animated){
                        am.dt_rot = selected_room->selected_layer->selected_object->animation.dt_rot-0.05f;
                        selected_room->selected_layer->selected_object->animation.dt_rot = am.dt_rot;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 48:{
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
                            glXSwapBuffers(dpy, win);
                        }
                        o_ptr->rotation = save;
                        o_ptr->pos = savep;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 61:{
                    if(selected_room->selected_layer->selected_object->is_animated){
                        float pxc = (event.xbutton.x - biasx) / sS;
                        float pyc = 1.0f - (event.xbutton.y - biasy) / sS;
                        //vec2 rel = vec2_Plus(VEC2(pxc, pyc), cam_pos);
                        pxc /= g_scale;
                        pyc /= g_scale;
                        vec2 rel = VEC2(pxc+cam_pos.x, pyc+cam_pos.y);
                        am.pos = vec2_Minus(rel, selected_room->selected_layer->selected_object->pos);
                        selected_room->selected_layer->selected_object->animation.pos = am.pos;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 113:{
                    if(selected.obj > -1){
                        spawned_rot = selected_room->selected_layer->selected_object->rotation + 10.0f;
                        if(spawned_rot == 180.0f) spawned_rot = -180.0f;
                        selected_room->selected_layer->selected_object->rotation = spawned_rot;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 114:{
                    if(selected.obj > -1){
                        spawned_rot = selected_room->selected_layer->selected_object->rotation - 10.0f;
                        if(spawned_rot == -180.0f) spawned_rot = 180.0f;
                        selected_room->selected_layer->selected_object->rotation = spawned_rot;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 111:{
                    if(selected.obj > -1){
                        float coo = selected_room->selected_layer->selected_object->scale.x / selected_room->selected_layer->selected_object->scale.y;
                        spawned_scale = vec2_Minus(selected_room->selected_layer->selected_object->scale, VEC2(0.02f*coo, 0.02f));
                        selected_room->selected_layer->selected_object->scale = spawned_scale;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 116:{
                    if(selected.obj > -1){
                        float coo = selected_room->selected_layer->selected_object->scale.x / selected_room->selected_layer->selected_object->scale.y;
                        spawned_scale = vec2_Plus(selected_room->selected_layer->selected_object->scale, VEC2(0.02f*coo, 0.02f));
                        selected_room->selected_layer->selected_object->scale = spawned_scale;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 119:{
                    if(selected.obj > -1){
                        for(unsigned int i = selected.obj+1; i < selected_room->selected_layer->lenght; ++i)
                            selected_room->selected_layer->objects[i-1] = selected_room->selected_layer->objects[i];
                        --selected_room->selected_layer->lenght;
                        selected.obj = -1;
                        needRedraw = GL_TRUE;
                    }
                    break;
                }
                case 10:{
                    puts((selected_room->selected_layer->selected_object->rigid = !selected_room->selected_layer->selected_object->rigid) ? "rigid" : "trigger");
                    break;
                }
                case 33:{
                    FILE *saved_scene = fopen("/home/oleg/Main/cbps/CardCreator/scene.scn", "wb");
                    unsigned short sizef = 1;
                    fseek(saved_scene, 2, SEEK_SET);
                    unsigned char len_char = (unsigned char)rooms_len;
                    fwrite(&len_char, 1, 1, saved_scene);
                    for(unsigned int r = 0; r < rooms_len; ++r){
                        selected_room = rooms + r;
                        unsigned char fore_lenght = 0, back_lenght = 0, anims_len = 0;
                        vec2 start = {9999, 9999};
                        vec2 finish ={-9999, -9999};
                        for(int g = 0; g < 4; ++g){
                            unsigned char p1 = back_lenght, p2 = fore_lenght;
                            unsigned int is_fore = g > 1;
                            for(int f = 0; f < selected_room->ierarchy[g].lenght; ++f){
                                object *obj = selected_room->ierarchy[g].objects + f;
                                if(is_fore){
                                    if(obj->tag == TAG_START) start = obj->pos;
                                    if(obj->tag == TAG_FINISH) finish = obj->pos;

                                    scene.foreground[fore_lenght] = (platform){
                                        obj->pos,
                                        obj->scale,
                                        obj->rotation,
                                        obj->rigid,
                                        obj->tag,
                                        g == 3,
                                        obj->mc/8
                                    };
                                    if(obj->is_animated){
                                        anim a = obj->animation;
                                        scene.anims[anims_len] = a;
                                        scene.owners[anims_len] = scene.foreground[fore_lenght];
                                        ++anims_len;
                                    }
                                    ++fore_lenght;
                                }else{
                                    scene.background[back_lenght] = (back_obj){
                                        obj->pos,
                                        obj->scale,
                                        obj->rotation,
                                        g == 0,
                                        obj->mc/8
                                    };
                                    ++back_lenght;
                                }
                            }
                            if     (g==0)qsort(scene.background, selected_room->ierarchy[0].lenght, sizeof(back_obj), cmp);
                            else if(g==1)qsort(scene.background+p1, selected_room->ierarchy[1].lenght, sizeof(back_obj), cmp);
                            else if(g==2)qsort(scene.foreground, selected_room->ierarchy[2].lenght, sizeof(platform), cmp2);
                            else if(g==3)qsort(scene.foreground+p2, selected_room->ierarchy[3].lenght, sizeof(platform), cmp2);

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
                        sizef += 3 + sizeof(vec2)*2 + sizeof(back_obj)*back_lenght + sizeof(platform)*fore_lenght + sizeof(anim)*anims_len;
                        unsigned char conf[3] = {back_lenght, fore_lenght, anims_len};
                        fwrite(conf, sizeof(unsigned char) * 3, 1, saved_scene);
                        vec2 poses[2] = {start, finish};
                        fwrite(poses, sizeof(vec2) * 2, 1, saved_scene);
                        if(back_lenght) fwrite(scene.background, sizeof(back_obj) * back_lenght, 1, saved_scene);
                        if(fore_lenght) fwrite(scene.foreground, sizeof(platform) * fore_lenght, 1, saved_scene);
                        if(anims_len){
                            qsort(scene.anims, anims_len, sizeof(anim), cmp3);
                            fwrite(scene.anims, sizeof(anim) * anims_len, 1, saved_scene);
                        }
                    }
                    fseek(saved_scene, 0, SEEK_SET);
                    fwrite(&sizef, 2, 1, saved_scene);
                    fclose(saved_scene);
                    break;
                }
                case 9:{
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDeleteTextures(1, &texture);
                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    glDisable(GL_BLEND);
                    glDisable(GL_TEXTURE_2D);
                    for(int h = 0; h < 4; ++h)
                        for(int f = 0; f < 3; ++f)
                            free((rooms + h)->ierarchy[f].objects);
                    return 0;
                }
            }
          break;
        }
        case ButtonPress:{
            float pxc = (event.xbutton.x - biasx) / sS;
            float pyc = 1.0f - (event.xbutton.y - biasy) / sS;
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
                }else if(pxc > 1.9f){
                    if(selected.obj != -1){
                        int ind = 10 - ((int)(pyc * 10));
                        selected_room->selected_layer->selected_object->tag = ind;
                        printf("selected tag: %d|%d\n", ind, selected.obj);
                        needRedraw = GL_TRUE;
                    }else{
                        int ind = 39 - ((int)(pyc * 40));
                        if(ind > -1 && ind < markdown_len)
                            selected_markdown = ind * 8;
                    }
                }else if(pyc > 0.9f && pxc < 0.6f){
                    int ind = (int)(pxc*10)-2;
                    if(ind < 4){
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
                    for(int f = 3; f > -1; --f){
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
            break;
        }
        case ConfigureNotify:{
            glLoadIdentity();
            int wr = event.xconfigure.width;
            int hr = event.xconfigure.height;
            float coo = (float)wr / (float)hr;
            if(coo > 2){
                biasx = (wr - hr*2.0f) / 2.0f;
                glViewport((wr - (wr / coo * 2)) / 2, 0, wr / coo * 2, hr);
                glOrtho(0, wr / coo * 2 / hr, 0, 1, 0, 1);
                sS = (float)hr;
                biasy = 0;
            }else if(coo < 2){
                biasy = (hr - wr/2.0f) / 2.0f;
                glViewport(0, (hr - (hr / (2 / coo))) / 2, wr, hr / (2 / coo));
                glOrtho(0, 1, 0, hr / (2 / coo) / wr, 0, 1);
                sS = (float)wr/2;
                biasx = 0;
            }else{
                glViewport(0, 0, wr, hr);
                glOrtho(0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 1.0f);
                sS = (float)hr;
                biasx = 0;
                biasy = 0;
            }
        }
        case Expose:
          needRedraw = GL_TRUE;
          break;
      }
    } while(XPending(dpy));
    if(needRedraw){
        redraw();
        needRedraw = GL_FALSE;
    }
    }

  return 0;
}
