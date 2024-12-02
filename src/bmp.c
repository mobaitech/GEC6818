#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int *plcd = NULL;  // 全局变量,映射的首地址

void lcd_draw_point(int x, int y, int color) {
  // 判断这个点是否在帧缓冲的范围内
  if (x >= 0 && x < 800 && y >= 0 && y < 480) {
    *(plcd + 800 * y + x) = color;
  }
}

int main() {
  // 1.打开lcd的屏幕
  int lcd_fd = open("/dev/fb0", O_RDWR);
  if (lcd_fd == -1) {
    printf("open lcd fail\n");
    return 0;
  }

  // 2.打开映射
  plcd =
      mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);

  // 3.打开一个图片文件描述符
  int bmp_fd = open("./1.bmp", O_RDWR);
  if (bmp_fd == -1) {
    printf("open bmp fail\n");
    return 0;
  }
  int width;
  lseek(bmp_fd, 0x12, SEEK_SET);  // 把光标移动到文件的0x12的位置
  read(bmp_fd, &width, 4);
  int height;  // Fix typo to height
  lseek(bmp_fd, 0x16, SEEK_SET);
  read(bmp_fd, &height, 4);  // Fix typo to height
  short depth;
  lseek(bmp_fd, 0x1c, SEEK_SET);
  read(bmp_fd, &depth, 2);
  int line_valid_bytes;  // 每一行的有效像素数据字节数
  int line_bytes;   // 每一行的实际字节数 = 有效字节数 + 癞子数
  int total_bytes;  // 整个像素数组的字节数
  int laizi = 0;    // 每一行需要填充的字节数

  line_valid_bytes = abs(width) * (depth / 8);

  // 判断是否需要癞子
  if (line_valid_bytes % 4) {
    laizi = 4 - line_valid_bytes % 4;
  }
  line_bytes = line_valid_bytes + laizi;
  total_bytes = line_bytes * abs(height);

  // 现在已经知道了总字节数,我们就需要开始一口气把所有的数据都读取出来
  unsigned char *piexls = malloc(total_bytes);  // Fix pointer type
  lseek(bmp_fd, 54, SEEK_SET);
  read(bmp_fd, piexls, total_bytes);

  unsigned char a, r, g, b;  // 依次获取每个颜色的分量值
  int color;
  int i = 0;  // 遍历像素数组
  int x, y;   // 遍历对应的LCD屏幕
  for (y = 0; y < abs(height); y++) {  // Fix typo to height
    for (x = 0; x < abs(width); x++) {
      b = piexls[i++];
      g = piexls[i++];
      r = piexls[i++];
      // 判断色深是否为32
      if (depth == 32) {
        a = piexls[i++];
      } else {
        a = 0;
      }
      color = a << 24 | r << 16 | g << 8 | b;
      // 调用画点函数画在LCD屏幕上
      lcd_draw_point(width > 0 ? x : abs(width) - 1 - x,
                     height > 0 ? abs(height) - 1 - y : y, color);  // Fix typos to height
    }
    i += laizi;  // Fix typo to laizi
  }

  free(piexls);
  munmap(plcd, 800 * 480 * 4);  // Add cleanup for mmap
  close(lcd_fd);  // Add cleanup for file descriptor
  close(bmp_fd);  // Add cleanup for file descriptor
  return 0;  // Add return statement
}