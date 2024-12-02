#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // 1.打开文件
  int lcd_fd = open("/dev/fb0", O_RDWR);
  if (lcd_fd == -1) {
    printf("open lcd failed");
    return 0;
  }
  // 2.准备一个颜色数组
  int color[800 * 480] = {0};
  int i;
  for (i = 0; i < 800 * 480; i++) {
    color[i] = 0x00ff00;
  }
  // 3.把颜色写入到文件中
  write(lcd_fd, color, 800 * 480 * 4);
  // 4.关闭文件
  close(lcd_fd);
  return 0;
}