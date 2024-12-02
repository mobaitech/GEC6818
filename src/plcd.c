#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int *plcd = NULL;

void lcd_draw_point(int x, int y, int color) {
  if (x >= 0 && x < 800 && y >= 0 && y < 480) {
    plcd[y * 800 + x] = color;
  }
}

int main() {
  // 1.打开文件
  int lcd_fd = open("/dev/fb0", O_RDWR);
  if (lcd_fd == -1) {
    printf("open lcd failed");
    return 0;
  }
  // 2.打开映射
  plcd =
      mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
  // 3.操作映射
  int x, y;
  for (y = 0; y < 480; y++) {
    for (x = 0; x < 800; x++) {
      lcd_draw_point(x, y, 0x00ff0000);
    }
  }
  // 4.关闭文件
  munmap(plcd, 800 * 480 * 4);
  close(lcd_fd);
  return 0;
}