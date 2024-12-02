#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int get_touch_dir();

int main() {
  get_touch_dir();
  return 0;
}

int get_touch_dir() {
  int tc_fd = open("/dev/input/event0", O_RDWR);
  if (tc_fd < 0) {
    perror("Failed to open input device");
    return 1;
  }

  struct input_event ev;
  int x = 0, y = 0;
  int x0 = 0, y0 = 0;
  int x1 = 0, y1 = 0;
  int touch_active = 0;

  while (1) {
    while (1) {
      read(tc_fd, &ev, sizeof(ev));
      // x的坐标值
      if (ev.type == EV_ABS && ev.code == ABS_X) {
        x = ev.value;
      }
      // y的坐标值
      if (ev.type == EV_ABS && ev.code == ABS_Y) {
        y = ev.value;
      }
      // 当手指按下触控板时记录初始坐标
      if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 1) {
        x0 = x;
        y0 = y;
        touch_active = 1;
      }
      // 当手指离开触控板时记录结束坐标并输出方向
      if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0) {
        if (touch_active) {
          x1 = x;
          y1 = y;
          int dx = x1 - x0;
          int dy = y1 - y0;
          if (abs(dx) > abs(dy)) {
            if (dx > 0) {
              printf("Right\n");
            } else {
              printf("Left\n");
            }
          } else {
            if (dy > 0) {
              printf("Down\n");
            } else {
              printf("Up\n");
            }
          }
          touch_active = 0;
        }
        break;
      }
    }
  }
  close(tc_fd);
}