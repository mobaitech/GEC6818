#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// 方向宏定义
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

int *plcd = NULL;  // 全局变量,映射的首地址

int checkStart() {
  int tc_fd = open("/dev/input/event0", O_RDWR);
  if (tc_fd < 0) {
    perror("Failed to open input device");
    return 1;
  }

  struct input_event ev;
  int x = 0, y = 0;

  while (1) {
    if (read(tc_fd, &ev, sizeof(ev)) < 0) {
      perror("Failed to read input event");
      close(tc_fd);
      return 1;
    }

    // x的坐标值
    if (ev.type == EV_ABS && ev.code == ABS_X) {
      x = ev.value;
    }

    // y的坐标值
    if (ev.type == EV_ABS && ev.code == ABS_Y) {
      y = ev.value;
    }

    if (x > 0 && y > 0) {
      break;
    }
  }
  close(tc_fd);
  return 0;
}

void lcd_draw_point(int x, int y, int color) {
  if (x >= 0 && x < 800 && y >= 0 && y < 480) {
    *(plcd + 800 * y + x) = color;
  }
}

void bmp_function(const char *lcd_path, const char *bmp_path) {
  // 1.打开lcd的屏幕
  int lcd_fd = open(lcd_path, O_RDWR);
  if (lcd_fd == -1) {
    printf("open lcd fail\n");
    return;
  }

  // 2.打开映射
  plcd =
      mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);

  // 3.打开一个图片文件描述符
  int bmp_fd = open(bmp_path, O_RDWR);
  if (bmp_fd == -1) {
    printf("open bmp fail\n");
    munmap(plcd, 800 * 480 * 4);
    close(lcd_fd);
    return;
  }

  int width;
  lseek(bmp_fd, 0x12, SEEK_SET);
  read(bmp_fd, &width, 4);
  int height;
  lseek(bmp_fd, 0x16, SEEK_SET);
  read(bmp_fd, &height, 4);
  short depth;
  lseek(bmp_fd, 0x1c, SEEK_SET);
  read(bmp_fd, &depth, 2);
  int line_valid_bytes;
  int line_bytes;
  int total_bytes;
  int laizi = 0;

  line_valid_bytes = abs(width) * (depth / 8);

  if (line_valid_bytes % 4) {
    laizi = 4 - line_valid_bytes % 4;
  }
  line_bytes = line_valid_bytes + laizi;
  total_bytes = line_bytes * abs(height);

  unsigned char *piexls = malloc(total_bytes);
  lseek(bmp_fd, 54, SEEK_SET);
  read(bmp_fd, piexls, total_bytes);

  unsigned char a, r, g, b;
  int color;
  int i = 0;
  int x, y;
  for (y = 0; y < abs(height); y++) {
    for (x = 0; x < abs(width); x++) {
      b = piexls[i++];
      g = piexls[i++];
      r = piexls[i++];
      if (depth == 32) {
        a = piexls[i++];
      } else {
        a = 0;
      }
      color = a << 24 | r << 16 | g << 8 | b;
      lcd_draw_point(width > 0 ? x : abs(width) - 1 - x,
                     height > 0 ? abs(height) - 1 - y : y, color);
    }
    i += laizi;
  }

  free(piexls);
  munmap(plcd, 800 * 480 * 4);
  close(lcd_fd);
  close(bmp_fd);
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

      // 左上角退出
      if (x0 < 100 && y0 < 100) {
        return 6;
      }

      // 右下角循环
      if (x0 > 900 && y0 > 400) {
        return 5;
      }

      printf("x0 = %d, y0 = %d\n", x0, y0);
      // 左下角开始
      if (x0 <100 && y0 > 400) {
        return 10;
      }
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
            return RIGHT;
          } else {
            return LEFT;
          }
        } else {
          if (dy > 0) {
            return DOWN;
          } else {
            return UP;
          }
        }
        touch_active = 0;
      }
      break;
    }
  }
  close(tc_fd);
  return 0;
}

int main() {
  const char *pathname[] = {"./1.bmp", "./2.bmp", "./3.bmp", "./4.bmp",
                            "./5.bmp", "./6.bmp", "./7.bmp", "./8.bmp"};

  int check = 0;
  while (1) {
    bmp_function("/dev/fb0", "./0.bmp");
    bmp_function("/dev/fb0", "./-1.bmp");
    int dir = 0;
    int show = 0;
    int tt=checkStart();
    while (1) {
      dir = get_touch_dir();
      if (dir == 6) {
        bmp_function("/dev/fb0", "./0.bmp");
        bmp_function("/dev/fb0", "./-1.bmp");
        break;
      }
      if (dir == UP || dir == LEFT) {
        if (show == 0) show = 8;
        bmp_function("/dev/fb0", pathname[(++show) % 8]);
        bmp_function("/dev/fb0", "./-1.bmp");
      } else if (dir == DOWN || dir == RIGHT) {
        if (show == 0) show = 8;
        bmp_function("/dev/fb0", pathname[(--show) % 8]);
        bmp_function("/dev/fb0", "./-1.bmp");
      }
      if (dir == 5) {
        int cycle = 0;
        while (1) {
          if (show == 0) show = 8;
          bmp_function("/dev/fb0", pathname[(++show) % 8]);
          sleep(1);
          cycle++;
          if (cycle == 8) break;
        }
      }
    }
  }
  return 0;
}