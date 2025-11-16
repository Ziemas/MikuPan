#include "enums.h"
#include "iopdrv.h"

#include "common/logging_c.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wimplicit-function-declaration"

struct ADPCM_CMD
{
    int f_start;
    int f_size;
    int start_flm;
    short fin_flm;
    short file_no;
    short voll;
    short field_12;
    short volr;
    short pan;
    short pitch;
    char action;
    char field_1B;
    char channel;
    char field_1D;
};

struct ADPCM_CMD now_cmd;
struct ADPCM_CMD cmd_buf[8];

enum
{
    IA_PLAY = 1,
    IA_PRELOAD = 2,
    IA_STOP = 3,
    IA_PAUSE = 4,
    IA_RESTART = 5,
};

void IAdpcmCmdSlide()
{
    int i;

    now_cmd = cmd_buf[0];
    for (i = 0; i < 7; i++)
    {
        cmd_buf[i] = cmd_buf[i + 1];
    }

    cmd_buf[7].action = 0;
}

int IAdpcmCmdPlay()
{
    return 0;
}

int IAdpcmCmdPreLoad()
{
    return 0;
}

int IAdpcmCmdStop()
{
    return 0;
}

int IAdpcmCmdPause()
{
    return 0;
}

int IAdpcmCmdRestart()
{
    //?? seems like it might do nothing
    return 0;
}

void IAdpcmMain2()
{
    if (!now_cmd.action)
    {
        IAdpcmCmdSlide();
    }
    else
    {
        switch (now_cmd.action)
        {
            case 1:
                IAdpcmCmdPlay();
                break;
            case 2:
                IAdpcmCmdPreLoad();
                break;
            case 3:
                IAdpcmCmdStop();
                break;
            case 4:
                IAdpcmCmdPause();
                break;
            case 5:
                IAdpcmCmdRestart();
                break;
        }
    }
}

void IAdpcmInit(int mode)
{
}

void IAdpcmAddCmd(IOP_COMMAND *cmd)
{
    struct ADPCM_CMD adpcm = {};
    int i;

    adpcm.action = 0;
    switch (cmd->cmd_no)
    {
        case IC_ADPCM_PLAY:
            adpcm.action = IA_PLAY;
            adpcm.fin_flm = cmd->data6;
            adpcm.f_start = cmd->data2;
            adpcm.f_size = cmd->data3;
            adpcm.start_flm = cmd->data7;
            adpcm.file_no = cmd->data1;
            adpcm.voll = cmd->data5;
            adpcm.volr = adpcm.voll;
            adpcm.pan = cmd->data5 >> 16;
            adpcm.pitch = cmd->data6 >> 16;
            adpcm.field_1B = cmd->data4 & 2;
            adpcm.channel = cmd->data4 & 1;
            if (adpcm.fin_flm)
            {
                adpcm.field_1D = 1;
                adpcm.voll = 0;
            }
            else
            {
                adpcm.field_1D = 0;
            }
            break;
        case IC_ADPCM_PRELOAD:
            adpcm.action = IA_PRELOAD;
            adpcm.file_no = cmd->data1;
            adpcm.f_start = cmd->data2;
            adpcm.f_size = cmd->data3;
            adpcm.start_flm = cmd->data7;
            adpcm.channel = cmd->data4 & 1;
            break;
        case IC_ADPCM_LOADEND_PLAY:
            adpcm.action = IA_PLAY;
            adpcm.fin_flm = cmd->data6;
            adpcm.start_flm = cmd->data7;
            adpcm.file_no = cmd->data1;
            adpcm.voll = cmd->data5;
            adpcm.volr = adpcm.voll;
            adpcm.pan = cmd->data5 >> 16;
            adpcm.pitch = cmd->data6 >> 16;
            adpcm.field_1B = cmd->data4 & 2;
            adpcm.channel = cmd->data4 & 1;
            if (adpcm.fin_flm)
            {
                adpcm.field_1D = 1;
                adpcm.voll = 0;
            }
            else
            {
                adpcm.field_1D = 0;
            }
            break;
        case IC_ADPCM_STOP:
            adpcm.action = IA_STOP;
            adpcm.fin_flm = cmd->data6;
            adpcm.file_no = cmd->data1;
            adpcm.channel = cmd->data4 & 1;
            break;
        case IC_ADPCM_PAUSE:
            adpcm.action = IA_PAUSE;
            adpcm.fin_flm = cmd->data6;
            adpcm.file_no = cmd->data1;
            adpcm.channel = cmd->data4 & 1;
            break;
        case IC_ADPCM_RESTART:
            adpcm.action = IA_RESTART;
            adpcm.fin_flm = cmd->data6;
            adpcm.file_no = cmd->data1;
            adpcm.channel = cmd->data4 & 1;
            break;
        default:
            break;
    }

    // Insert into cmd buffer
    if (adpcm.action)
    {
        for (i = 0; i < 8; i++)
        {
            if (cmd_buf[i].action == 0)
            {
                info_log("adpcm: inserted command %d", adpcm.action);
                cmd_buf[i] = adpcm;
                break;
            }
        }
    }
}

void IAdpcmCmd(IOP_COMMAND *cmd)
{
    switch (cmd->cmd_no)
    {
        case IC_ADPCM_PLAY:
        case IC_ADPCM_PRELOAD:
        case IC_ADPCM_LOADEND_PLAY:
        case IC_ADPCM_STOP:
        case IC_ADPCM_PAUSE:
        case IC_ADPCM_RESTART:
            IAdpcmAddCmd(cmd);
            break;
        default:
            info_log("Error: ADPCM Command %d not yet implemented!",
                     cmd->cmd_no);
            break;
    }
}

#pragma GCC diagnostic pop
