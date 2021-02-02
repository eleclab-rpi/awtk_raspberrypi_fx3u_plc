/**
 * File:   tslib_thread.c
 * Author: AWTK Develop Team
 * Brief:  thread to handle touch screen events
 *
 * Copyright (c) 2018 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-09-07 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
//#include "tslib.h"
#include "tkc/mem.h"
#include "base/keys.h"
#include "tkc/thread.h"
#include "tslib_thread.h"
#include "tkc/utils.h"

#ifndef EV_SYN
#define EV_SYN 0x00
#endif

typedef struct _run_info_t {
  int fd;
  int32_t x;
  int32_t y;
  int32_t max_x;
  int32_t max_y;
  void* dispatch_ctx;
  char* filename;
  input_dispatch_t dispatch;
  union {
    int8_t d[3];
    struct input_event e; /* for EasyARM-iMX280A_283A_287A */
  } data;

  event_queue_req_t req;
} run_info_t;

static ret_t tslib_dispatch(run_info_t* info) {
  ret_t ret = info->dispatch(info->dispatch_ctx, &(info->req), "tslib");
  info->req.event.type = EVT_NONE;

  return ret;
}

static ret_t tslib_dispatch_one_event(run_info_t* info) {
  struct input_event e = {0};
  event_queue_req_t* req = &(info->req);  
  int ret = -1;

  if (info->fd < 0) {
    ret = -1;
  } else {
    ret = read(info->fd, &info->data.e, sizeof(info->data.e));
  }


  if (ret < 0) {
    sleep(2);

    if (access(info->filename, R_OK) == 0) {
      if (info->fd >= 0) {
        close(info->fd);
      }

      info->fd = open(info->filename, O_RDONLY);
      if (info->fd < 0) {
        log_debug("%s:%d: open tslib failed, fd=%d, filename=%s\n", __func__, __LINE__, info->fd,
                  info->filename);
        perror("print mouse: ");
      } else {
        log_debug("%s:%d: open tslib successful, fd=%d, filename=%s\n", __func__, __LINE__,
                  info->fd, info->filename);
      }
    }
  }

  switch (info->data.e.type) {
    case EV_KEY: {
      if (info->data.e.code == BTN_TOUCH) {
        req->event.type = info->data.e.value ? EVT_POINTER_DOWN : EVT_POINTER_UP;
      }

      break;
    }
    case EV_ABS: {
      switch (info->data.e.code) {
        case ABS_MT_POSITION_X:
        case ABS_X: {
          req->pointer_event.x = info->data.e.value;
          break;
        }
        case ABS_MT_POSITION_Y:
        case ABS_Y: {
          req->pointer_event.y = info->data.e.value;
          break;
        }
        case ABS_MT_TRACKING_ID: {
          if (info->data.e.value > 0) {
            req->event.type = EVT_POINTER_DOWN;
          } else {
            req->event.type = EVT_POINTER_UP;
          }
          break;
        }

      }

      if (req->event.type == EVT_NONE) {
        req->event.type = EVT_POINTER_MOVE;
      }

      break;
    }

    case EV_SYN: {
      switch (req->event.type) {
        case EVT_POINTER_DOWN:
        case EVT_POINTER_MOVE:
        case EVT_POINTER_UP: {
          return tslib_dispatch(info);
        }
        default:
          break;
      }
      break;
    }
  }
  return RET_OK;
  
}

void* tslib_run(void* ctx) {
  run_info_t info = *(run_info_t*)ctx;
  if (info.fd == NULL) {
    log_debug("%s:%d: open tslib failed, filename=%s\n", __func__, __LINE__, info.filename);
  } else {
    log_debug("%s:%d: open tslib successful, filename=%s\n", __func__, __LINE__, info.filename);
  }

  TKMEM_FREE(ctx);
  while (tslib_dispatch_one_event(&info) == RET_OK)
    ;
  close(info.fd);
  TKMEM_FREE(info.filename);

  return NULL;
}

static run_info_t* info_dup(run_info_t* info) {
  run_info_t* new_info = TKMEM_ZALLOC(run_info_t);

  *new_info = *info;

  return new_info;
}

tk_thread_t* tslib_thread_run(const char* filename, input_dispatch_t dispatch, void* ctx,
                              int32_t max_x, int32_t max_y) {
  run_info_t info;
  tk_thread_t* thread = NULL;
  return_value_if_fail(filename != NULL && dispatch != NULL, NULL);

  memset(&info, 0x00, sizeof(info));

  info.max_x = max_x;
  info.max_y = max_y;
  info.dispatch_ctx = ctx;
  info.dispatch = dispatch;
  info.fd = open(filename, 0);
  info.filename = tk_strdup(filename);

  //if (info.ts != NULL) {
  //  ts_config(info.ts);
  //}

  thread = tk_thread_create(tslib_run, info_dup(&info));
  if (thread != NULL) {
    tk_thread_start(thread);
  } else {
    close(info.fd);
    TKMEM_FREE(info.filename);
  }

  return thread;
}
