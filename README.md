# 使用说明书

## 硬件要求

粤嵌GEC6818开发板

## 编译命令

```bash
sudo ln -s /usr/lib/x86_64-linux-gnu/libmpfr.so.6 /usr/lib/x86_64-linux-gnu/libmpfr.so.4
# 下载arm-linux-gcc，为ubuntu虚拟机配置arm-linux-gcc环境
# 执行sudo的密码是123456

arm-linux-gcc yourFile.c
# 将你的yourFile.c这个文件编译为可执行程序，然后ls查看结果即可
```

## 传输文件到开发板

```
rx 你编译好的程序名
# 比如rx main
```

然后代开

## 环境搭建

1. 使用arm-linux-gcc进行C语言代码编译
2. 使用VScode进行C语言代码编写项目文件
3. 编译传输到GEC6818开发板中运行

## 项目目录

1. bin文件夹：存放编译后的.out二进制文件
2. src文件夹：存放.c代码文件

> 本身是需要lib文件夹来存放需要的库文件和inc文件夹来存放需要的头文件，但是开发板里面有库文件了

## 素材文件

images

- 图片素材（对于标准的jpg等图片转为bmp文件类型即可）

## 功能操作

#### 电子相册

* 进入相册：主页任意位置双击进入（注意不是点击“进入相册”几个字）
* 下一张：向左滑动或向上滑动
* 上一张：向右滑动或向下滑动
* 退出：点击左上角退出按键
* 循环播放相册：进入相册后点击右下角区域
