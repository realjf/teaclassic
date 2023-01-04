#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "tc_math.h"

#include <stdbool.h>
#include <stdint.h>

#define SETT_NAME_LEN 128
#define SETT_MAX_PRIO 2

struct sval {
    enum {
        ST_TYPE_STRING,
        ST_TYPE_FLOAT,
        ST_TYPE_INT,
        ST_TYPE_BOOL,
        ST_TYPE_VEC2,
    } type;
    union {
        char as_string[SETT_NAME_LEN];
        float as_float;
        int as_int;
        bool as_bool;
        vec2_t as_vec2;
    };
};

struct setting {
    char name[SETT_NAME_LEN];
    struct sval val;
    /* When reading the settings file, all settings with a lower priority number
     * will be read before settings with a higher priority number. This allows
     * creating some dependencies between settings. */
    int prio;

    /* Called before a new setting value is committed - if 'validate'
     * returns false, the update is aborted. Can be NULL. */
    bool (*validate)(const struct sval *new_val);
    /* Called when the value of a setting updated. This can be used
     * to actually apply engine settings (ex. changing the resolution).
     * Can be NULL. */
    void (*commit)(const struct sval *new_val);
};

typedef enum settings_status {
    SS_OKAY = 0,
    SS_NO_SETTING,
    SS_INVALID_VAL,
    SS_FILE_ACCESS,
    SS_FILE_PARSING,
    SS_BADALLOC,
} ss_e;

ss_e Settings_Init(void);
void Settings_Shutdown(void);

/* If a setting with this name already exists, its' value is preserved and
 * it is used to replace the provided default. */
ss_e Settings_Create(struct setting sett);
ss_e Settings_Delete(const char *name);

ss_e Settings_Get(const char *name, struct sval *out);
ss_e Settings_Set(const char *name, const struct sval *new_val);
ss_e Settings_SetNoValidate(const char *name, const struct sval *new_val);
/* The new value is not written to the settings file. Until it is overwritten
 * with a persistent value, the old value will be written. */
ss_e Settings_SetNoPersist(const char *name, const struct sval *new_val);

ss_e Settings_SaveToFile(void);
ss_e Settings_LoadFromFile(void);
const char *Settings_GetFile(void);

#endif /* _SETTINGS_H_ */
