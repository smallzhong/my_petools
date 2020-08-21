# my_petools
 PE查看 遍历当前所有模块 滴水三期课后作业

## 项目说明

+ 本项目是[滴水三期视频](https://www.bilibili.com/video/BV1yt41127Cd)的一个课后作业，界面采用纯WIN32API编写，没有使用任何框架。

+ 主要功能：

  1. 遍历当前所有进程及其导入的所有模块（只能遍历32位进程的模块），并列出所有模块的 `ImageBase` 和 `SizeOfImage` 。

     ![看不见图片请爬梯子](https://raw.githubusercontent.com/smallzhong/picgo-pic-bed/master/image-20200821142559202.png)

  2. PE结构查看

     ![看不见图片请爬梯子](https://raw.githubusercontent.com/smallzhong/picgo-pic-bed/master/image-20200821142846647.png)

     由于PETOOLS已经非常强大，我觉得并没有必要做得像PETOOLS那么完善。因此只做了导入表信息的查看，并没有做其他表。文件头也只是列出了几个重要的信息。如果想要完善可以自己加控件。

+ 本项目使用了Process Status Helper的函数，在VC6中需要导入 `psapi.h` 和相应的静态库文件。这些相应文件我都放在了[my_petools](https://github.com/smallzhong/my_petools/tree/master/my_petools)文件夹中，不需要另外下载，直接可以编译通过。

## 编译环境

XP系统 + VC6

> 注意，由于字符编码的问题，本程序无法在VS中编译通过。如果想要兼容，可能可以通过以下方法
>
> ```cpp
> #ifdef _UNICODE
> #define _tsprintf swprintf
> #else
> #define _tsprintf sprintf
> #endif
> ```
>
> 并将 `sprintf` 中的第二个参数全部用 `TEXT("")` 括起来。
>
> 可能还要用 `setlocale(LC_ALL, "");` 之类的方法解决其他函数的编码问题，我太懒了就直接在XP系统下弄了

在 [releases](https://github.com/smallzhong/my_petools/releases) 中可以下载已经编译好的release版的二进制文件，如果因为墙的原因看不到本readme的效果图，也可自行下载EXE文件来试用，自行检查效果。

## 参考

1. 遍历进程+遍历模块 <https://blog.csdn.net/Kwansy/article/details/106432474>





