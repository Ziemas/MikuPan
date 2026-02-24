#ifndef INGAME_MENU_IG_RANK_H
#define INGAME_MENU_IG_RANK_H

#include "typedefs.h"
#include "ig_types.h"
#include "os/system.h"


typedef struct { // 0x5
    /* 0x0 */ u_char mode;
    /* 0x1 */ u_char csr_bak;
    /* 0x2 */ u_char star;
    /* 0x3 */ u_char cnt;
    /* 0x4 */ char exist_s_rank;
} MENU_RANK;

extern SAVE_RANK save_rank;

void NewgameMenuRankInit();
void LoadgameMenuRankInit();
void StartRankModeInit();
void IngameMenuRank();
void IngameMenuRankDisp();
void RankingChkMem(PICTURE_WRK new_pic);
int CheckSamePic(PICTURE_WRK *newp);

#endif // INGAME_MENU_IG_RANK_H
