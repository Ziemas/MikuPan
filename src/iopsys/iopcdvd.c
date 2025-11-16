#include "iopdrv.h"

#include "common.h"
#include "common/logging_c.h"
#include "common/memory_addresses.h"
#include "enums.h"

#include <SDL3/SDL_mutex.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wimplicit-function-declaration"

void ReadFileInArchive(int sector, int size, int64_t address);
unsigned char *LoadImgHdFile();

struct cdvd_stat cdvd_stat;
struct cdvd_req cdvd_req[32];
void *load_buf_table[2];
SDL_Mutex *cdvd_lock;

void ICdvdSetRetStat(u32 cid, char stat)
{
    if (cid < 32)
        iop_stat.cdvd.fstat[cid].stat = stat;
}

void ICdvdCopyToEE(struct cdvd_req *rq)
{
    info_log("read(%x, %x, %p)", rq->sector, rq->size, rq->dst_addr);
    ReadFileInArchive(rq->sector, rq->size, rq->dst_addr);
}

void ICdvdDoTransfer(struct cdvd_req *rq)
{
    switch (rq->destination)
    {
        case 0:// EE
            ICdvdCopyToEE(rq);
            break;
        case 1:// IOP
            info_log("CDVD transfer to IOP unimplemented");
            break;
        case 2:// SPU
            info_log("CDVD transfer to SPU unimplemented");
            break;
    }

    SDL_LockMutex(cdvd_lock);
    cdvd_stat.cmd_rp = (cdvd_stat.cmd_rp + 1) % 32;
    cdvd_stat.cmd_count--;
    ICdvdSetRetStat(rq->id, 0);
    SDL_UnlockMutex(cdvd_lock);
}

int ICdvdMain()
{
    struct cdvd_req rq;
    bool found_work = false;

    SDL_LockMutex(cdvd_lock);

    if (cdvd_stat.cmd_count)
    {
        rq = cdvd_req[cdvd_stat.cmd_rp];
        found_work = true;
    }

    SDL_UnlockMutex(cdvd_lock);

    if (found_work)
    {
        ICdvdDoTransfer(&rq);
    }

    return 0;
}

void ICdvdInitOnce()
{
    //memset(&cdvd_stat, 0, sizeof(cdvd_stat));
    //memset(cdvd_req, 0, sizeof(cdvd_req));
    memset(&iop_stat.cdvd, 0, sizeof(iop_stat.cdvd));
    //memset(cdvd_trans, 0, sizeof(cdvd_trans));
    load_buf_table[0] = malloc(0x64000);
    if (load_buf_table[0])
    {
        load_buf_table[1] = load_buf_table[0] + 0x32000;
        iop_stat.cdvd.ld_addr = load_buf_table[0];
    }

    cdvd_stat.fp = fopen("\\IMG.BD.BIN", "r");

    ImgHdAddress = LoadImgHdFile();

    cdvd_lock = SDL_CreateMutex();
}

void ICdvdInitSoftReset()
{
    //memset(&cdvd_stat, 0, sizeof(cdvd_stat));
    //memset(cdvd_req, 0, sizeof(cdvd_req));
    memset(&iop_stat.cdvd, 0, sizeof(iop_stat.cdvd));
}

void ICdvdInit(int mode)
{
    if (mode)
        ICdvdInitSoftReset();
    else
        ICdvdInitOnce();
}

void ICdvdAddCmd(IOP_COMMAND *cmd)
{
    struct cdvd_req rq;

    rq.cmd_no = cmd->cmd_no;
    rq.file_no = cmd->data1;
    rq.sector = cmd->data2;
    rq.size = cmd->data3;
    if (cmd->data5 < 4)
    {
        rq.destination = cmd->data5;
    }
    else
    {
        rq.destination = 0;
    }

    if (rq.destination == 2)
    {
        rq.dst_addr = SeGetSndBufTop(cmd->data4);
        rq.se_type = cmd->data4;
    }
    else
    {
        rq.dst_addr = cmd->data4;
    }
    rq.id = cmd->data6;

    if (cdvd_stat.cmd_count < 32)
    {
        SDL_LockMutex(cdvd_lock);

        cdvd_req[cdvd_stat.cmd_wp] = rq;
        cdvd_stat.cmd_wp = (cdvd_stat.cmd_wp + 1) % 32;
        cdvd_stat.cmd_count++;
        ICdvdSetRetStat(rq.id, 1);

        SDL_UnlockMutex(cdvd_lock);
    }
}

void ICdvdTransSe(IOP_COMMAND *cmd)
{
    info_log("ICdvdTransSe");
}

void ICdvdTransSeInit()
{
    iop_stat.cdvd.se_trans = 0;
}

void ICdvdBreak()
{
}

void ICdvdCmd(IOP_COMMAND *cmd)
{
    switch (cmd->cmd_no)
    {

        case IC_CDVD_INIT:
            ICdvdInit(cmd->data1);
            break;
        case IC_CDVD_LOAD:
        case IC_CDVD_LOAD_SECT:
        case IC_CDVD_SEEK:
            ICdvdAddCmd(cmd);
            break;
        case IC_CDVD_SE_TRANS:
            ICdvdTransSe(cmd);
            break;
        case IC_CDVD_SE_TRANS_RESET:
            ICdvdTransSeInit();
            break;
        case IC_CDVD_BREAK:
            ICdvdBreak();
            break;
        default:
            info_log("Error: CDVD Command %d not yet implemented!",
                     cmd->cmd_no);
            break;
    }
}

#pragma GCC diagnostic pop
