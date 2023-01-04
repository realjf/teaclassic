#include "../camera.h"
#include "../event.h"
#include "../game/public/game.h"
#include "../lib/public/attr.h"
#include "../lib/public/khash.h"
#include "../lib/public/nk_file_browser.h"
#include "../lib/public/tc_string.h"
#include "../main.h"
#include "../render/public/render.h"
#include "../render/public/render_ctrl.h"
#include "../settings.h"
#include "al_assert.h"
#include "al_effect.h"
#include "public/audio.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <SDL.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define EPSILON (1.0f / 1024)

#define CHK_TRUE_RET(_pred)                                                    \
  do {                                                                         \
    if (!(_pred))                                                              \
      return false;                                                            \
  } while (0)

struct al_buffer {
  ALuint buffer;
  ALenum format;
};

KHASH_MAP_INIT_STR(buffer, struct al_buffer)

/*****************************************************************************/
/* STATIC VARIABLES                                                          */
/*****************************************************************************/

static ALCdevice *s_device = NULL;
static ALCcontext *s_context = NULL;

static khash_t(buffer) * s_music;
static khash_t(buffer) * s_effects;
static ALuint s_music_source;
static ALuint s_foreground_sources[AUDIO_NUM_FG_CHANNELS];

static bool s_mute_on_focus_loss = false;
static ALfloat s_master_volume = 0.5f;
static ALfloat s_music_volume = 0.5f;
static enum playback_mode s_music_mode = MUSIC_MODE_PLAYLIST;

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

static bool audio_load_wav(const char *path, struct al_buffer *out) {
  struct SDL_AudioSpec spec;
  Uint8 *audio_buff;
  Uint32 audio_len;

  if (!SDL_LoadWAV(path, &spec, &audio_buff, &audio_len))
    return false;

  ALenum format = -1;
  switch (spec.channels) {
  case 1:
    switch (spec.format) {
    case AUDIO_U8:
    case AUDIO_S8:
      format = AL_FORMAT_MONO8;
      break;
    default:
      format = AL_FORMAT_MONO16;
      break;
    }
    break;
  case 2:
    switch (spec.format) {
    case AUDIO_U8:
    case AUDIO_S8:
      format = AL_FORMAT_STEREO8;
      break;
    default:
      format = AL_FORMAT_STEREO16;
      break;
    }
    break;
  default:
    assert(0);
  }
  assert(format >= 0);

  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, audio_buff, audio_len, spec.freq);
  AL_ASSERT_OK();

  SDL_FreeWAV(audio_buff);

  out->buffer = buffer;
  out->format = format;
  return true;
}

static void audio_free_buffer(struct al_buffer *buff) {
  alDeleteBuffers(1, &buff->buffer);
  AL_ASSERT_OK();
}

static void audio_create_global_source(ALuint *src, ALfloat volume) {
  alGenSources(1, src);
  alSourcef(*src, AL_PITCH, 1);
  alSourcef(*src, AL_GAIN, volume);
  alSource3f(*src, AL_POSITION, 0, 0, 0);
  alSource3f(*src, AL_VELOCITY, 0, 0, 0);
  alSourcei(*src, AL_LOOPING, AL_FALSE);
  alSourcei(*src, AL_BUFFER, 0);
  alSourcei(*src, AL_SOURCE_RELATIVE, AL_TRUE);
  alSourcef(*src, AL_ROLLOFF_FACTOR, 0.0);
  AL_ASSERT_OK();
}

static void audio_index_directory(const char *prefix, const char *dir,
                                  khash_t(buffer) * table) {
  char absdir[NK_MAX_PATH_LEN];
  tc_snprintf(absdir, sizeof(absdir), "%s/%s", g_basepath, dir);

  size_t nfiles = 0;
  struct file *files = nk_file_list(absdir, &nfiles);

  for (int i = 0; i < nfiles; i++) {

    if (files[i].is_dir) {

      if (!strcmp(files[i].name, ".") || !strcmp(files[i].name, ".."))
        continue;

      char dirpath[NK_MAX_PATH_LEN];
      tc_snprintf(dirpath, sizeof(dirpath), "%s/%s", dir, files[i].name,
                  sizeof(dirpath));

      char newprefix[NK_MAX_PATH_LEN] = "";
      if (prefix && strlen(prefix) > 0) {
        tc_strlcat(newprefix, prefix, sizeof(newprefix));
        tc_strlcat(newprefix, "/", sizeof(newprefix));
      }
      tc_strlcat(newprefix, files[i].name, sizeof(newprefix));

      audio_index_directory(newprefix, dirpath, table);
      continue;
    }

    if (!tc_endswith(files[i].name, ".wav"))
      continue;

    char path[NK_MAX_PATH_LEN];
    tc_snprintf(path, sizeof(path), "%s/%s", absdir, files[i].name);

    struct al_buffer audio;
    if (!audio_load_wav(path, &audio))
      continue;

    char name[NK_MAX_PATH_LEN] = "";
    if (prefix && strlen(prefix) > 0) {
      tc_strlcat(name, prefix, sizeof(name));
      tc_strlcat(name, "/", sizeof(name));
    }
    tc_strlcat(name, files[i].name, sizeof(name));
    name[strlen(name) - strlen(".wav")] = '\0';

    const char *key = tc_strdup(name);
    if (!key) {
      audio_free_buffer(&audio);
      continue;
    }

    int status;
    khiter_t k = kh_put(buffer, table, key, &status);
    if (status == -1) {
      free((void *)key);
      audio_free_buffer(&audio);
      continue;
    }
    kh_value(table, k) = audio;
  }

  free(files);
}

static bool audio_volume_validate(const struct sval *val) {
  if (val->type != ST_TYPE_FLOAT)
    return false;
  if (val->as_float < 0.0f || val->as_float > 1.0f)
    return false;
  return true;
}

static void audio_master_volume_commit(const struct sval *val) {
  s_master_volume = val->as_float;
  alListenerf(AL_GAIN, s_master_volume);
  AL_ASSERT_OK();
}

static void audio_music_volume_commit(const struct sval *val) {
  s_music_volume = val->as_float;
  alSourcef(s_music_source, AL_GAIN, s_music_volume);
  AL_ASSERT_OK();
}

static bool audio_bool_validate(const struct sval *val) {
  return (val->type == ST_TYPE_BOOL);
}

static void audio_mute_focus_commit(const struct sval *val) {
  s_mute_on_focus_loss = val->as_bool;
}

static bool audio_music_mode_validate(const struct sval *val) {
  if (val->type != ST_TYPE_INT)
    return false;
  if (val->as_int < 0 || val->as_int > MUSIC_MODE_SHUFFLE)
    return false;
  return true;
}

static void audio_music_mode_commit(const struct sval *val) {
  s_music_mode = val->as_int;
}

static void audio_create_settings(void) {
  ss_e status;
  (void)status;

  status = Settings_Create((struct setting){
      .name = "tc.audio.master_volume",
      .val = (struct sval){.type = ST_TYPE_FLOAT, .as_float = s_master_volume},
      .prio = 0,
      .validate = audio_volume_validate,
      .commit = audio_master_volume_commit,
  });
  assert(status == SS_OKAY);

  status = Settings_Create((struct setting){
      .name = "tc.audio.music_volume",
      .val = (struct sval){.type = ST_TYPE_FLOAT, .as_float = s_music_volume},
      .prio = 0,
      .validate = audio_volume_validate,
      .commit = audio_music_volume_commit,
  });
  assert(status == SS_OKAY);

  status = Settings_Create((struct setting){
      .name = "tc.audio.mute_on_focus_loss",
      .val =
          (struct sval){.type = ST_TYPE_BOOL, .as_bool = s_mute_on_focus_loss},
      .prio = 0,
      .validate = audio_bool_validate,
      .commit = audio_mute_focus_commit,
  });
  assert(status == SS_OKAY);

  status = Settings_Create((struct setting){
      .name = "tc.audio.music_playback_mode",
      .val = (struct sval){.type = ST_TYPE_INT, .as_bool = s_music_mode},
      .prio = 0,
      .validate = audio_music_mode_validate,
      .commit = audio_music_mode_commit,
  });
  assert(status == SS_OKAY);

  status = Settings_Create((struct setting){
      .name = "tc.debug.show_hearing_range",
      .val = (struct sval){.type = ST_TYPE_BOOL, .as_bool = false},
      .prio = 0,
      .validate = audio_bool_validate,
      .commit = NULL});
  assert(status == SS_OKAY);
}

static void audio_window_event(void *user, void *arg) {
  if (!s_mute_on_focus_loss)
    return;

  SDL_WindowEvent *event = arg;
  switch (event->event) {
  case SDL_WINDOWEVENT_FOCUS_LOST:
    alListenerf(AL_GAIN, 0.0f);
    break;
  case SDL_WINDOWEVENT_FOCUS_GAINED:
    alListenerf(AL_GAIN, s_master_volume);
    break;
  }
}

static const char *audio_music_name(ALuint buffer) {
  const char *name;
  struct al_buffer curr;

  kh_foreach(s_music, name, curr, {
    if (curr.buffer == buffer)
      return name;
  });
  return NULL;
}

static bool audio_name_music(const char *name, ALint *out) {
  khiter_t k = kh_get(buffer, s_music, name);
  if (k == kh_end(s_music))
    return false;
  *out = kh_value(s_music, k).buffer;
  return true;
}

static void audio_next_music_track(void) {
  const char *tracks[kh_size(s_music)];
  size_t ntracks = Audio_GetAllMusic(ARR_SIZE(tracks), tracks);

  ALint play_buffer = 0;
  alGetSourcei(s_music_source, AL_BUFFER, &play_buffer);

  const char *curr = audio_music_name(play_buffer);
  const char *next = curr;

  int curr_idx = -1;
  for (int i = 0; i < ntracks; i++) {
    if (!strcmp(curr, tracks[i])) {
      curr_idx = i;
      break;
    }
  }
  assert(curr);
  assert(curr_idx >= 0 && curr_idx < ntracks);

  switch (s_music_mode) {
  case MUSIC_MODE_LOOP:
    break;
  case MUSIC_MODE_PLAYLIST: {
    int next_idx = (curr_idx + 1) % ntracks;
    next = tracks[next_idx];
    break;
  }
  case MUSIC_MODE_SHUFFLE: {
    if (ntracks > 0) {
      tracks[curr_idx] = tracks[--ntracks];
      int next_idx = rand() % ntracks;
      next = tracks[next_idx];
    }
    break;
  }
  default:
    assert(0);
  }

  Audio_PlayMusic(next);
}

static void audio_update_listener(void) {
  vec3_t cam_pos = Camera_GetPos(G_GetActiveCamera());
  vec3_t listener_pos = cam_pos;

  if (G_MapLoaded()) {
    bool hit =
        M_Raycast_CameraIntersecCoord(G_GetActiveCamera(), &listener_pos);
    if (hit) {
      /* nudge the hearing center point such that it's more
       * centered within the viewport */
      vec3_t cam_dir = Camera_GetDir(G_GetActiveCamera());
      cam_dir.y = 0.0f;
      if (TCM_Vec3_Len(&cam_dir) > EPSILON)
        TCM_Vec3_Normal(&cam_dir, &cam_dir);
      TCM_Vec3_Scale(&cam_dir, 45.0f, &cam_dir);
      TCM_Vec3_Add(&listener_pos, &cam_dir, &listener_pos);
    }
  }
  alListener3f(AL_POSITION, listener_pos.x, listener_pos.y, listener_pos.z);
  AL_ASSERT_OK();
}

static void audio_on_update(void *user, void *event) {
  ALint src_state;
  alGetSourcei(s_music_source, AL_SOURCE_STATE, &src_state);

  if (src_state == AL_STOPPED) {
    audio_next_music_track();
  }
  audio_update_listener();
}

static int compare_strings(const void *a, const void *b) {
  const char *stra = *(const char **)a;
  const char *strb = *(const char **)b;
  return strcmp(stra, strb);
}

static void on_render_3d(void *user, void *event) {
  struct sval setting;
  ss_e status;
  (void)status;

  status = Settings_Get("tc.debug.show_hearing_range", &setting);
  if (!setting.as_bool)
    return;

  if (!G_MapLoaded())
    return;

  vec2_t pos = Audio_ListenerPosXZ();
  const float radius = HEARING_RANGE;
  const float width = 0.5f;
  vec3_t red = (vec3_t){1.0f, 0.0f, 0.0f};

  R_PushCmd((struct rcmd){
      .func = R_GL_DrawSelectionCircle,
      .nargs = 5,
      .args =
          {
              R_PushArg(&pos, sizeof(pos)),
              R_PushArg(&radius, sizeof(radius)),
              R_PushArg(&width, sizeof(width)),
              R_PushArg(&red, sizeof(red)),
              (void *)G_GetPrevTickMap(),
          },
  });
}

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

bool Audio_Init(void) {
  if (NULL == (s_device = alcOpenDevice(NULL)))
    goto fail_open;

  if (NULL == (s_context = alcCreateContext(s_device, NULL)))
    goto fail_context;
  alcMakeContextCurrent(s_context);

  if (NULL == (s_music = kh_init(buffer)))
    goto fail_music_table;

  if (NULL == (s_effects = kh_init(buffer)))
    goto fail_effects_table;

  audio_create_global_source(&s_music_source, s_music_volume);
  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {
    audio_create_global_source(&s_foreground_sources[i], Audio_EffectVolume());
  }
  alListenerf(AL_GAIN, s_master_volume);

  if (!Audio_Effect_Init())
    goto fail_effects;

  audio_index_directory(NULL, "assets/music", s_music);
  audio_index_directory(NULL, "assets/sounds", s_effects);

  audio_create_settings();

  E_Global_Register(SDL_WINDOWEVENT, audio_window_event, NULL, G_ALL);
  E_Global_Register(EVENT_UPDATE_START, audio_on_update, NULL, G_ALL);
  E_Global_Register(EVENT_RENDER_3D_POST, on_render_3d, NULL, G_ALL);
  return true;

fail_effects:
  alDeleteSources(1, &s_music_source);
  alDeleteSources(AUDIO_NUM_FG_CHANNELS, s_foreground_sources);
  kh_destroy(buffer, s_effects);
fail_effects_table:
  kh_destroy(buffer, s_music);
fail_music_table:
  alcMakeContextCurrent(NULL);
  alcDestroyContext(s_context);
fail_context:
  alcCloseDevice(s_device);
fail_open:
  return false;
}

void Audio_Shutdown(void) {
  const char *name;
  struct al_buffer curr;

  alSourceStop(s_music_source);
  alDeleteSources(1, &s_music_source);

  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {
    alSourceStop(s_foreground_sources[i]);
    alDeleteSources(1, &s_foreground_sources[i]);
  }

  Audio_Effect_Shutdown();

  kh_foreach(s_music, name, curr, {
    free((void *)name);
    audio_free_buffer(&curr);
  });
  kh_destroy(buffer, s_music);

  kh_foreach(s_effects, name, curr, {
    free((void *)name);
    audio_free_buffer(&curr);
  });
  kh_destroy(buffer, s_effects);

  E_Global_Unregister(SDL_WINDOWEVENT, audio_window_event);
  E_Global_Unregister(EVENT_UPDATE_START, audio_on_update);
  E_Global_Unregister(EVENT_RENDER_3D_POST, on_render_3d);

  alcMakeContextCurrent(NULL);
  alcDestroyContext(s_context);
  alcCloseDevice(s_device);
}

bool Audio_PlayMusic(const char *name) {
  ALint src_state;
  alGetSourcei(s_music_source, AL_SOURCE_STATE, &src_state);

  if (name == NULL) {
    alSourceStop(s_music_source);
    alSourcei(s_music_source, AL_BUFFER, 0);
    return true;
  }

  ALint play_buffer = 0;
  if (!audio_name_music(name, &play_buffer))
    return false;

  alSourceStop(s_music_source);
  alSourcei(s_music_source, AL_BUFFER, play_buffer);
  alSourcePlay(s_music_source);

  AL_ASSERT_OK();
  return true;
}

void Audio_PlayMusicFirst(void) {
  const char *tracks[1];
  size_t ntracks = Audio_GetAllMusic(ARR_SIZE(tracks), tracks);
  if (ntracks > 0) {
    Audio_PlayMusic(tracks[0]);
  }
}

bool Audio_PlayForegroundEffect(const char *name, bool interrupt, int channel) {
  if (channel >= AUDIO_NUM_FG_CHANNELS)
    return false;

  ALint src_state;
  alGetSourcei(s_foreground_sources[channel], AL_SOURCE_STATE, &src_state);

  if (src_state == AL_PLAYING && !interrupt)
    return true;

  if (name == NULL) {
    alSourceStop(s_foreground_sources[channel]);
    alSourcei(s_foreground_sources[channel], AL_BUFFER, 0);
    return true;
  }

  ALint play_buffer = 0;
  if (!Audio_GetEffectBuffer(name, &play_buffer))
    return false;

  alSourceStop(s_foreground_sources[channel]);
  alSourcei(s_foreground_sources[channel], AL_BUFFER, play_buffer);
  alSourcePlay(s_foreground_sources[channel]);

  AL_ASSERT_OK();
  return true;
}

size_t Audio_GetAllMusic(size_t maxout, const char *out[static maxout]) {
  const char *tracks[kh_size(s_music)];
  size_t ntracks = 0;

  const char *name;
  kh_foreach(s_music, name, (struct al_buffer){0},
             { tracks[ntracks++] = name; });
  qsort(tracks, ntracks, sizeof(const char *), compare_strings);

  size_t ret = MIN(ntracks, maxout);
  memcpy(out, tracks, ret * sizeof(const char *));
  return ret;
}

const char *Audio_CurrMusic(void) {
  ALint play_buffer = 0;
  alGetSourcei(s_music_source, AL_BUFFER, &play_buffer);
  return audio_music_name(play_buffer);
}

const char *Audio_ErrString(ALenum err) {
  const char *ret = NULL;
  switch (err) {
  case ALC_INVALID_VALUE:
    ret = "ALC_INVALID_VALUE";
    break;
  case ALC_INVALID_DEVICE:
    ret = "ALC_INVALID_DEVICE";
    break;
  case ALC_INVALID_CONTEXT:
    ret = "ALC_INVALID_CONTEXT";
    break;
  case ALC_INVALID_ENUM:
    ret = "ALC_INVALID_ENUM";
    break;
  case ALC_OUT_OF_MEMORY:
    ret = "ALC_OUT_OF_MEMORY";
    break;
  default:
    assert(0);
  }
  return ret;
}

bool Audio_GetEffectBuffer(const char *name, ALint *out) {
  khiter_t k = kh_get(buffer, s_effects, name);
  if (k == kh_end(s_effects))
    return false;
  *out = kh_value(s_effects, k).buffer;
  return true;
}

const char *Audio_GetEffectName(ALuint buffer) {
  const char *name;
  struct al_buffer curr;

  kh_foreach(s_effects, name, curr, {
    if (curr.buffer == buffer)
      return name;
  });
  return NULL;
}

vec2_t Audio_ListenerPosXZ(void) {
  vec2_t pos = {0};
  alGetListener3f(AL_POSITION, &pos.x, &(ALfloat){0}, &pos.z);
  return pos;
}

float Audio_BufferDuration(ALuint buffer) {
  ALint nbytes;
  ALint channels;
  ALint bits;
  ALint freq;

  alGetBufferi(buffer, AL_SIZE, &nbytes);
  alGetBufferi(buffer, AL_CHANNELS, &channels);
  alGetBufferi(buffer, AL_BITS, &bits);
  alGetBufferi(buffer, AL_FREQUENCY, &freq);

  size_t nsamples = nbytes * 8 / (channels * bits);
  return nsamples / ((float)freq);
}

void Audio_SetForegroundEffectVolume(float gain) {
  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {
    alSourcef(s_foreground_sources[i], AL_GAIN, gain);
  }
}

void Audio_Pause(void) {
  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {
    ALint fg_state;
    alGetSourcei(s_foreground_sources[i], AL_SOURCE_STATE, &fg_state);

    if (fg_state == AL_PLAYING) {
      alSourcePause(s_foreground_sources[i]);
    }
  }
  Audio_EffectPause();
}

void Audio_Resume(uint32_t dt) {
  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {
    ALint fg_state;
    alGetSourcei(s_foreground_sources[i], AL_SOURCE_STATE, &fg_state);

    if (fg_state == AL_PAUSED) {
      alSourcePlay(s_foreground_sources[i]);
    }
  }
  Audio_EffectResume(dt);
}

void Audio_ClearState(void) {
  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {
    alSourceStop(s_foreground_sources[i]);
    alSourcei(s_foreground_sources[i], AL_BUFFER, 0);
  }
  /* The music keeps playing even accross sessions */

  Audio_EffectClearState();
}

bool Audio_SaveState(struct SDL_RWops *stream) {
  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {

    ALint fg_buffer;
    alGetSourcei(s_foreground_sources[i], AL_BUFFER, &fg_buffer);
    struct attr fg_has_buffer_attr =
        (struct attr){.type = TYPE_BOOL, .val.as_bool = (fg_buffer != 0)};
    CHK_TRUE_RET(Attr_Write(stream, &fg_has_buffer_attr, "fg_has_buffer"));

    if (fg_has_buffer_attr.val.as_bool) {

      const char *name = Audio_GetEffectName(fg_buffer);
      assert(name);

      struct attr fg_clip_attr = (struct attr){
          .type = TYPE_STRING,
      };
      tc_snprintf(fg_clip_attr.val.as_string,
                  sizeof(fg_clip_attr.val.as_string), name);
      CHK_TRUE_RET(Attr_Write(stream, &fg_clip_attr, "fg_clip"));

      ALint fg_offset;
      alGetSourcei(s_foreground_sources[i], AL_SAMPLE_OFFSET, &fg_offset);
      struct attr fg_offset_attr =
          (struct attr){.type = TYPE_INT, .val.as_int = fg_offset};
      CHK_TRUE_RET(Attr_Write(stream, &fg_offset_attr, "fg_offset"));

      ALint fg_state;
      alGetSourcei(s_foreground_sources[i], AL_SOURCE_STATE, &fg_state);
      struct attr fg_state_attr =
          (struct attr){.type = TYPE_INT, .val.as_int = fg_state};
      CHK_TRUE_RET(Attr_Write(stream, &fg_state_attr, "fg_state"));
    }
  }

  vec3_t pos = {0};
  alGetListener3f(AL_POSITION, &pos.x, &pos.y, &pos.z);
  struct attr listener_pos_attr =
      (struct attr){.type = TYPE_VEC3, .val.as_vec3 = pos};
  CHK_TRUE_RET(Attr_Write(stream, &listener_pos_attr, "listener_pos"));

  if (!Audio_EffectSaveState(stream))
    return false;

  return true;
}

bool Audio_LoadState(struct SDL_RWops *stream) {
  struct attr attr;

  for (int i = 0; i < AUDIO_NUM_FG_CHANNELS; i++) {

    CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
    CHK_TRUE_RET(attr.type == TYPE_BOOL);
    bool fg_has_buffer = attr.val.as_bool;

    /* Take a more liberal approach with loading the audio
     * state, as it depens on resource files which may have
     * been modified, renamed, etc. In case of failing to
     * load something, just keep going.
     */
    if (fg_has_buffer) {

      CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
      CHK_TRUE_RET(attr.type == TYPE_STRING);
      char name[sizeof(attr.val.as_string)];
      tc_strlcpy(name, attr.val.as_string, sizeof(name));

      CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
      CHK_TRUE_RET(attr.type == TYPE_INT);
      ALint offset = attr.val.as_int;

      CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
      CHK_TRUE_RET(attr.type == TYPE_INT);
      ALint state = attr.val.as_int;

      ALint buffer;
      if (Audio_GetEffectBuffer(name, &buffer)) {

        alSourcei(s_foreground_sources[i], AL_BUFFER, buffer);
        alSourcei(s_foreground_sources[i], AL_SAMPLE_OFFSET, offset);
        if (alGetError() == AL_NO_ERROR) {
          switch (state) {
          case AL_INITIAL:
            break;
          case AL_PLAYING:
            alSourcePlay(s_foreground_sources[i]);
            break;
          case AL_PAUSED:
            alSourcePlay(s_foreground_sources[i]);
            alSourcePause(s_foreground_sources[i]);
            break;
          case AL_STOPPED:
            alSourcePlay(s_foreground_sources[i]);
            alSourceStop(s_foreground_sources[i]);
            break;
          }
          glGetError(); /* clear error state */
        }
      }
    }
  }

  CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
  CHK_TRUE_RET(attr.type == TYPE_VEC3);
  vec3_t pos = attr.val.as_vec3;
  alListener3f(AL_POSITION, pos.x, pos.y, pos.z);

  if (!Audio_EffectLoadState(stream))
    return false;

  return true;
}
