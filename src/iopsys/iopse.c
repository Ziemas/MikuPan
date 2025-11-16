#include "iopdrv.h"

#include "common/logging_c.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wimplicit-function-declaration"

u32 SeGetSndBufTop(int type)
{
    //return snd_buf_top[a1];
    return 0;
}

void ISeCmd(IOP_COMMAND *cmd)
{
    switch (cmd->cmd_no)
    {
        default:
            info_log("Error: SE Command %d not yet implemented!", cmd->cmd_no);
            break;
    }
}

#pragma GCC diagnostic pop
