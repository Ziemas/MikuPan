#ifndef IOPDRV_H_
#define IOPDRV_H_

#include "common.h"
#include "os/eeiop/eeiop.h"

#include <SDL3/SDL_audio.h>
#include <stdio.h>

struct cdvd_stat
{
    FILE *fp;
    u32 cmd_count;
    u32 cmd_wp;
    u32 cmd_rp;
};

struct cdvd_req
{
    int cmd_no;
    int file_no;
    int sector;
    int size;
    int field_10;
    int id;
    uintptr_t dst_addr;
    char se_type;
    u8 destination;
    char field_1E;
};

struct cdvd_trans
{
};

extern IOP_STAT iop_stat;
extern SDL_AudioDeviceID audio_dev;

void *IopDrvFunc(int fno, void *data, unsigned int length);

void IAdpcmInit(int mode);
void IAdpcmCmd(IOP_COMMAND *cmd);

void ICdvdInit(int mode);
void ICdvdCmd(IOP_COMMAND *cmd);

u32 SeGetSndBufTop(int type);
void ISeCmd(IOP_COMMAND *cmd);

#endif// IOPDRV_H_
