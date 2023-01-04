#ifndef _NK_FILE_BROWSER_H_
#define _NK_FILE_BROWSER_H_

#include "tc_nuklear.h"
#include <stdbool.h>

#define NK_MAX_PATH_LEN (512)

struct nk_fb_state {
    nk_flags flags;                  /* nk_panel_flags */
    char name[128];                  /* unique name for the instance */
    char directory[NK_MAX_PATH_LEN]; /* path of the current selected directory */
    char selected[NK_MAX_PATH_LEN];  /* empty string for no selection */
};

struct file {
    bool is_dir;
    char name[NK_MAX_PATH_LEN];
};

void nk_file_browser(struct nk_context *ctx, struct nk_fb_state *state);
struct file *nk_file_list(const char *dir, size_t *out_size);

#endif /* _NK_FILE_BROWSER_H_ */
