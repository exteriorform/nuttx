/****************************************************************************
 * graphics/nxmu/nx_requestbkgd.c
 *
 *   Copyright (C) 2008 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/nx.h>
#include "nxfe.h"

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nx_requestbkgd
 *
 * Description:
 *   NX normally controls a separate window called the background window.
 *   It repaints the window as necessary using only a solid color fill.  The
 *   background window always represents the entire screen and is always
 *   below other windows.  It is useful for an application to control the
 *   background window in the following conditions:
 *
 *   - If you want to implement a windowless solution.  The single screen
 *     can be used to creat a truly simple graphic environment.  In this
 *     case, you should probably also de-select CONFIG_NX_MULTIUSER as well.
 *   - When you want more on the background than a solid color.  For
 *     example, if you want an image in the background, or animations in the
 *     background, or live video, etc.
 *
 *   This API only requests the handle of the background window.  That
 *   handle will be returned asynchronously in a subsequent position and
 *   redraw callbacks.
 *
 *
 *   Cautions:
 *   - The following should never be called using the background window.
 *     They are guaranteed to cause severe crashes:
 *
 *       nx_setposition, nx_setsize, nx_raise, nx_lower.
 *
 *   - Neither nx_opengbwindow or nx_closebgwindow should be called more than
 *     once.  Multiple instances of the background window are not supported.
 *
 * Input Parameters:
 *   handle - The handle returned by nx_connect
 *   cb     - Callbacks to use for processing background window events
 *
 * Return:
 *   OK: Success; ERROR of failure with errno set appropriately.
 *
 ****************************************************************************/

int nx_requestbkgd(NXHANDLE handle, FAR const struct nx_callback_s *cb)
{
  FAR struct nxfe_conn_s *conn = (FAR struct nxfe_conn_s *)handle;
  struct nxsvrmsg_requestbkgd_s outmsg;
  int ret;

#ifdef CONFIG_DEBUG
  if (!conn || !cb)
    {
      errno = EINVAL;
      return ERROR;
    }
#endif

  /* Request access to the background window from the server */

  outmsg.msgid = NX_SVRMSG_REQUESTBKGD;
  outmsg.conn  = conn;
  outmsg.cb    = cb;

  ret = mq_send(conn->cwrmq, &outmsg, sizeof(struct nxsvrmsg_requestbkgd_s), NX_SVRMSG_PRIO);
  if (ret < 0)
    {
      gdbg("mq_send failed: %d\n", errno);
      return ERROR;
    }
  return OK;
}

