#include "iopdrv.h"

#include "common.h"
#include "common/logging_c.h"
#include "enums.h"
#include "os/eeiop/eeiop.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wimplicit-function-declaration"

IOP_STAT iop_stat;
u32 se_start_flag;
u32 se_stop_flag;
u16 iop_mv;
char mono;

SDL_AudioDeviceID audio_dev;
SDL_Thread *iop_thread;

int ICdvdMain();
int IAdpcmMain2();

int SDLCALL IopMain(void *data)
{
    while (1)
    {
        ICdvdMain();
        IAdpcmMain2();

        SDL_DelayNS(4167000);
    }

    return 0;
}

void IopInitDevice()
{
    SDL_AudioSpec spec;

    SDL_Init(SDL_INIT_AUDIO);

    spec.channels = 2;
    spec.format = SDL_AUDIO_S16;
    spec.freq = 48000;

    audio_dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (!audio_dev)
    {
        info_log("%s", SDL_GetError());
    }
    SDL_ResumeAudioDevice(audio_dev);

    //sceSdInit(0);
    ICdvdInit(0);
    //ISeInit(0);
    IAdpcmInit(0);

    iop_thread = SDL_CreateThread(IopMain, "IOP", NULL);
}

void *IopDrvFunc(int fno, void *data, unsigned int length)
{
    IOP_COMMAND *cmd = (IOP_COMMAND *) data;
    int total_cmd = length / sizeof(IOP_COMMAND);

    if (fno == 0)
    {
        info_log("IOP server init");
        iop_mv = 0;
        mono = 0;
        IopInitDevice();
        iop_mv = 0x3FFF;
        //sceSdSetParam(SD_P_MVOLL | 0, iop_mv);
        //sceSdSetParam(SD_P_MVOLR | 0, iop_mv);
        //sceSdSetParam(SD_P_MVOLL | 1, iop_mv);
        //sceSdSetParam(SD_P_MVOLR | 1, iop_mv);
        //IopInitMain();
    }
    else
    {
        for (int i = 0; i < total_cmd; i++, cmd++)
        {
            if (cmd->cmd_no >= IC_SE_INIT && cmd->cmd_no <= IC_SE_QUIT)
            {
                ISeCmd(cmd);
            }

            if (cmd->cmd_no >= IC_CDVD_INIT && cmd->cmd_no <= IC_CDVD_BREAK)
            {
                ICdvdCmd(cmd);
            }

            if (cmd->cmd_no >= IC_ADPCM_INIT && cmd->cmd_no <= IC_ADPCM_QUIT)
            {
                IAdpcmCmd(cmd);
            }
        }
    }

    // iCdvdTransSeEnd();
    return &iop_stat;
}

#pragma GCC diagnostic pop
