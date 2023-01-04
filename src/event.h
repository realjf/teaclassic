#ifndef _EVENT_H_
#define _EVENT_H_

#include "script/public/script.h"

#include <SDL_events.h>
#include <stdbool.h>

enum eventtype {
    /*
     * +-----------------+-----------------------------------------------+
     * | Range           | Use                                           |
     * +-----------------+-----------------------------------------------+
     * | 0x0-0xffff      | SDL events                                    |
     * +-----------------+-----------------------------------------------+
     * | 0x10000-0x1ffff | Engine-generated events                       |
     * +-----------------+-----------------------------------------------+
     * | 0x20000-0x2ffff | Script-generated events                       |
     * +-----------------+-----------------------------------------------+
     *
     * The very first event serviced during a tick is a single EVENT_UPDATE_START one.
     * The very last event serviced during a tick is a single EVENT_UPDATE_END one.
     */
    EVENT_UPDATE_START = SDL_LASTEVENT + 1,
    EVENT_UPDATE_END,
    EVENT_UPDATE_UI,
    EVENT_RENDER_3D_PRE,
    EVENT_RENDER_3D_POST,
    EVENT_RENDER_UI,
    EVENT_RENDER_FINISH,
    EVENT_SELECTED_TILE_CHANGED,
    EVENT_NEW_GAME,
    EVENT_UNIT_SELECTION_CHANGED,
    EVENT_60HZ_TICK,
    EVENT_30HZ_TICK,
    EVENT_20HZ_TICK,
    EVENT_15HZ_TICK,
    EVENT_10HZ_TICK,
    EVENT_1HZ_TICK,
    EVENT_ANIM_FINISHED,
    EVENT_ANIM_CYCLE_FINISHED,
    EVENT_MOVE_ISSUED,
    EVENT_MOTION_START,
    EVENT_MOTION_END,
    EVENT_ATTACK_START,
    EVENT_ENTITY_DEATH,
    EVENT_ATTACK_END,
    EVENT_GAME_SIMSTATE_CHANGED,
    EVENT_SESSION_LOADED,
    EVENT_SESSION_POPPED,
    EVENT_SESSION_FAIL_LOAD,
    EVENT_SCRIPT_TASK_EXCEPTION,
    EVENT_SCRIPT_TASK_FINISHED,
    EVENT_BUILD_BEGIN,
    EVENT_BUILD_END,
    EVENT_BUILD_FAIL_FOUND,
    EVENT_BUILD_TARGET_ACQUIRED,
    EVENT_BUILDING_FOUNDED,
    EVENT_BUILDING_COMPLETED,
    EVENT_BUILDING_CONSTRUCTED,
    EVENT_ENTITY_DIED,
    EVENT_ENTITY_STOP,
    EVENT_HARVEST_BEGIN,
    EVENT_HARVEST_END,
    EVENT_HARVEST_TARGET_ACQUIRED,
    EVENT_TRANSPORT_TARGET_ACQUIRED,
    EVENT_STORAGE_TARGET_ACQUIRED,
    EVENT_STORAGE_SITE_AMOUNT_CHANGED,
    EVENT_RESOURCE_DROPPED_OFF,
    EVENT_RESOURCE_PICKED_UP,
    EVENT_RESOURCE_EXHAUSTED,
    EVENT_RESOURCE_AMOUNT_CHANGED,
    EVENT_ENTERED_REGION,
    EVENT_EXITED_REGION,
    EVENT_UPDATE_FACTION,
    EVENT_PROJECTILE_DISAPPEAR,
    EVENT_PROJECTILE_HIT,
    EVENT_ENTITY_DISAPPEARED,
    EVENT_ORDER_ISSUED,

    EVENT_ENGINE_LAST = 0x1ffff,
};

enum event_source {
    ES_ENGINE,
    ES_SCRIPT,
};

typedef void (*handler_t)(void *, void *);

struct script_handler {
    enum eventtype event;
    uint32_t id;
    int simmask;
    script_opaque_t handler;
    script_opaque_t arg;
};

/*###########################################################################*/
/* EVENT GENERAL                                                             */
/*###########################################################################*/

bool E_Init(void);
void E_ServiceQueue(void);
void E_Shutdown(void);
void E_DeleteScriptHandlers(void);
size_t E_GetScriptHandlers(size_t max_out, struct script_handler *out);
void E_ClearPendingEvents(void);
void E_FlushEventQueue(void);
const char *E_EngineEventString(enum eventtype event);
bool E_EventsQueued(void);

/*###########################################################################*/
/* EVENT GLOBAL                                                              */
/*###########################################################################*/

void E_Global_Notify(enum eventtype event, void *event_arg, enum event_source);
void E_Global_NotifyImmediate(enum eventtype event, void *event_arg, enum event_source);

bool E_Global_Register(enum eventtype event, handler_t handler, void *user_arg,
                       int simmask);
bool E_Global_Unregister(enum eventtype event, handler_t handler);

bool E_Global_ScriptRegister(enum eventtype event, script_opaque_t handler,
                             script_opaque_t user_arg, int simmask);
bool E_Global_ScriptUnregister(enum eventtype event, script_opaque_t handler);

/*###########################################################################*/
/* EVENT ENTITY                                                              */
/*###########################################################################*/

bool E_Entity_Register(enum eventtype event, uint32_t ent_uid, handler_t handler,
                       void *user_arg, int simmask);
bool E_Entity_Unregister(enum eventtype event, uint32_t ent_uid, handler_t handler);

bool E_Entity_ScriptRegister(enum eventtype event, uint32_t ent_uid,
                             script_opaque_t handler, script_opaque_t user_arg, int simmask);
bool E_Entity_ScriptUnregister(enum eventtype event, uint32_t ent_uid,
                               script_opaque_t handler);
void E_Entity_Notify(enum eventtype, uint32_t ent_uid, void *event_arg, enum event_source);
void E_Entity_NotifyImmediate(enum eventtype event, uint32_t ent_uid, void *event_arg,
                              enum event_source source);

#endif /* _EVENT_H_ */
