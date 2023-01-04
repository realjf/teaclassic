#include "public/attr.h"
#include "public/tc_string.h"
#include "../asset_load.h"

#include <string.h>
#include <assert.h>
#include <SDL.h>


#define CHK_TRUE(_pred, _label) do{ if(!(_pred)) goto _label; }while(0)

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

bool Attr_Parse(struct SDL_RWops *stream, struct attr *out, bool named)
{
    char line[MAX_LINE_LEN];
    READ_LINE(stream, line, fail);
    char *saveptr;
    char *token;

    if(named) {
        token = tc_strtok_r(line, " \t", &saveptr);
        CHK_TRUE(token, fail);

        strncpy(out->key, token, sizeof(out->key));
        out->key[sizeof(out->key)-1] = '\0';

        token = tc_strtok_r(NULL, " \t", &saveptr);
        CHK_TRUE(token, fail);
    }else{
        token = tc_strtok_r(line, " \t", &saveptr);
        CHK_TRUE(token, fail);
    }

    if(!strcmp(token, "string")) {

        out->type = TYPE_STRING;
        token = tc_strtok_r(NULL, "\n", &saveptr);
        CHK_TRUE(token, fail);
        tc_snprintf(out->val.as_string, sizeof(out->val.as_string), "%s", token);

    }else if(!strcmp(token, "quat")) {

        out->type = TYPE_QUAT;
        token = token + strlen(token) + 1;
        if(!sscanf(token, "%f %f %f %f",
            &out->val.as_quat.x, &out->val.as_quat.y, &out->val.as_quat.z, &out->val.as_quat.w))
            goto fail;

    }else if(!strcmp(token, "vec2")) {

        out->type = TYPE_VEC2;
        token = token + strlen(token) + 1;
        if(!sscanf(token, "%f %f",
            &out->val.as_vec2.x, &out->val.as_vec2.y))
            goto fail;

    }else if(!strcmp(token, "vec3")) {

        out->type = TYPE_VEC3;
        token = token + strlen(token) + 1;
        if(!sscanf(token, "%f %f %f",
            &out->val.as_vec3.x, &out->val.as_vec3.y, &out->val.as_vec3.z))
            goto fail;

    }else if(!strcmp(token, "bool")) {

        out->type = TYPE_BOOL;
        token = tc_strtok_r(NULL, " \t", &saveptr);
        CHK_TRUE(token, fail);
        int tmp;
        if(!sscanf(token, "%d", &tmp))
            goto fail;
        if(tmp != 0 && tmp != 1)
            goto fail;
        out->val.as_bool = tmp;

    }else if(!strcmp(token, "float")) {

        out->type = TYPE_FLOAT;
        token = tc_strtok_r(NULL, " \t", &saveptr);
        CHK_TRUE(token, fail);
        if(!sscanf(token, "%f", &out->val.as_float))
            goto fail;

    }else if(!strcmp(token, "int")) {

        out->type = TYPE_INT;
        token = tc_strtok_r(NULL, " \t", &saveptr);
        CHK_TRUE(token, fail);
        if(!sscanf(token, "%d", &out->val.as_int))
            goto fail;

    }else {
        goto fail;
    }

    return true;

fail:
    return false;
}

bool Attr_Write(struct SDL_RWops *stream, const struct attr *in, const char name[static 0])
{
    if(name) {
        CHK_TRUE(SDL_RWwrite(stream, name, strlen(name), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, " ", 1, 1), fail);
    }

    switch(in->type) {
    case TYPE_STRING:
        CHK_TRUE(SDL_RWwrite(stream, "string ", strlen("string "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, in->val.as_string, strlen(in->val.as_string), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    case TYPE_FLOAT: {
        char buff[64];
        tc_snprintf(buff, sizeof(buff), "%.6f", in->val.as_float);

        CHK_TRUE(SDL_RWwrite(stream, "float ", strlen("float "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, buff, strlen(buff), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    }
    case TYPE_INT: {
        char buff[64];
        tc_snprintf(buff, sizeof(buff), "%d", in->val.as_int);

        CHK_TRUE(SDL_RWwrite(stream, "int ", strlen("int "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, buff, strlen(buff), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    }
    case TYPE_VEC2: {
        char buff[64];
        tc_snprintf(buff, sizeof(buff), "%.6f %.6f",
            in->val.as_vec2.x, in->val.as_vec2.y);

        CHK_TRUE(SDL_RWwrite(stream, "vec2 ", strlen("vec2 "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, buff, strlen(buff), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    }
    case TYPE_VEC3: {
        char buff[64];
        tc_snprintf(buff, sizeof(buff), "%.6f %.6f %.6f",
            in->val.as_vec3.x, in->val.as_vec3.y, in->val.as_vec3.z);

        CHK_TRUE(SDL_RWwrite(stream, "vec3 ", strlen("vec3 "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, buff, strlen(buff), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    }
    case TYPE_QUAT: {
        char buff[64];
        tc_snprintf(buff, sizeof(buff), "%.6f %.6f %.6f %.6f",
            in->val.as_quat.x, in->val.as_quat.y, in->val.as_quat.z, in->val.as_quat.w);

        CHK_TRUE(SDL_RWwrite(stream, "quat ", strlen("quat "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, buff, strlen(buff), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    }
    case TYPE_BOOL: {
        char buff[64];
        tc_snprintf(buff, sizeof(buff), "%d", (int)in->val.as_bool);

        CHK_TRUE(SDL_RWwrite(stream, "bool ", strlen("bool "), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, buff, strlen(buff), 1), fail);
        CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
        break;
    }
    default: assert(0);
    }

    return true;

fail:
    return false;
}

