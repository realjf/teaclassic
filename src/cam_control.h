#ifndef _CAM_CONTROL_H_
#define _CAM_CONTROL_H_

#include "camera.h"

#include <SDL.h>

/*
 * Installation will set the specified camera as the currently active camera,
 * from whose point of view the world will be rendered.
 * The 'FPS' and 'RTS' modes control how mouse and keyboard events are used to
 * transform the active camera.  The 'Free' mode will simply render from the
 * camera's PoV.
 */
void CamControl_FPS_Install(struct camera *cam);
void CamControl_RTS_Install(struct camera *cam);
void CamControl_Free_Install(struct camera *cam);
void CamControl_UninstallActive(void);

#endif /* _CAM_CONTROL_H_ */
