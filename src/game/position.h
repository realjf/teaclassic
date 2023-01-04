#ifndef _POSITION_H_
#define _POSITION_H_

struct map;

bool G_Pos_Init(const struct map *map);
void G_Pos_Shutdown(void);
void G_Pos_Delete(uint32_t uid);

#endif /* _POSITION_H_ */
