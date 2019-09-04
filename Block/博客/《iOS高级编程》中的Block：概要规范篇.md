[TOC]
# 关于我的仓库

- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文主要是对于《高级编程》类似于总结的学习笔记

# 准备工作 
- 阅读《Objective-C 高级编程》中的p.79 ~ 91

# Blocks概要

- 一句话概括Blocks的扩充功能：带有自动变量（局部变量）的匿名函数

## 疑惑： p.81 buttonId

 ```objective-c
int buttonID = 0;

void buttonCallback(int event) {

    NSLog(@"button:%d event = %d", buttonID, event);
}

void setButtonCallback(int offset, void (*p)(int)) {

    p(offset);
}

void setButtonCallbacks() {

    for (int i = 0; i < 10; i++) {
        buttonID = i;
        setButtonCallback(0 + i, &buttonCallback);
    }
}

 ```

- 书上没有给出setButtonCallback函数的具体实现，但估计肯定是这样的，因为我们使用函数指针很大一部分就是为了使用函数回调
- 可是书上说，这样写了会出现问题，我觉得他的意思是说这样写会出现buttonID只会是for循环的最后一个，也就是9
- 可是我实验下来和正常情况没有什么区别啊