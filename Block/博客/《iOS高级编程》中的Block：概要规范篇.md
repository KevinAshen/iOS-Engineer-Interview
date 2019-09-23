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
- 可是我实验下来和正常情况没有什么区别，不是很懂

![887D7308-7318-4B92-A638-EBFF549147A1](https://tva1.sinaimg.cn/large/006y8mN6ly1g6ze9voasvj312v0u0n4g.jpg)

# Block语法

## 与一般C语言函数不同点：

- 没有函数名
- 带有“^”号【插入记号】

## 格式

### 标准格式

![ECDEC5C7-C6FF-4F5F-A1B8-DB8BA7F2BB6C](https://tva1.sinaimg.cn/large/006y8mN6ly1g6zekzwx0oj30c5021q3a.jpg)

```c++
^int (int count){return count + 1;}
```

### 省略格式

- 可以省略返回值类型
- 省略返回值类型的情况下，return什么类型返回值就是什么类型，不写return的话默认就是void
- 可以省略参数列表
- 没有参数的情况下block可以这么写

![60F591B8-D410-4FE9-886F-CFC273B52AEB](https://tva1.sinaimg.cn/large/006y8mN6ly1g6zfmd4rm0j30ba00x0sv.jpg)

- 也可以直接把(void)省略，直接这么写

![B89A2A1D-FFEB-49F7-867B-721ABAB15A98](https://tva1.sinaimg.cn/large/006y8mN6ly1g6zfp92224j307601874b.jpg)

- 所以我们的block省略完可以这么写：

![40B1C054-38B8-42A4-AD0A-43608AB50B85](https://tva1.sinaimg.cn/large/006y8mN6ly1g6zfqm875sj30c006l75b.jpg)

## block变量

- block变量类似于函数指针【联想：我们说block是匿名函数】
- 对于函数，我们可以使用一个函数指针，将函数的地址赋值给函数指针，在调用函数的时候可以直接通过函数指针调用函数

```c++
//ex:C语言函数指针
int func(int count) {
  return count + 1;
}

int (*funcptr)(int) = &func;

//这样子下面两个语句代表的含义一样
int res = func(2);
int res = funcptr(2);
```

- 同样对于block，我们可以使用block变量调用block中的表达式
- 声明block变量的语法和函数指针基本一致，只是把*号换成了^号

![2E876C3B-C47B-4345-A03C-5794F45A8996](https://tva1.sinaimg.cn/large/006y8mN6ly1g6zg4fy3q6j307y01b3ym.jpg)

- block变量和C语言变量完全相同，可以作为以下用途：
  - 自动变量
  - 函数参数
  - 静态变量
  - 静态全局变量
  - 全局变量

# 截获自动变量

- 看一段代码：

```c++
int dmy = 256;
int val = 10;
const char  *fmt = "val = %d\n";
void (^blk)(void) = ^{
  
		printf(fmt, val);
	};
	val = 2;
	fmt = "THESE VALUES WERE CHANGED. val = %d\n";
	blk();
```

- 结果为val = 10，而不是2，也就是说val这个变量被截获了，截获时它是10，虽然在块里面的代码在val = 2之后才被执行，但打印的依然是截获时的10，这就是截获自动变量
- 也就是说变量在代码运行到定义那一块就被截获了，执行的时候已经不是原变量了

# __block说明符

- block可以截获变量，但是不能在块里修改变量的值

![668D72AD-79AB-47D5-842E-C3CDFCFF8374](https://tva1.sinaimg.cn/large/006y8mN6ly1g70becuv8fj30n709779j.jpg)

- 此时就是需要使用__block修饰符修饰变量，对需要在block内进行赋值的变量，使用修饰符修饰，保证可以对变量进行赋值
- 赋值后按序执行，也就是说执行完了以后，变量的值就是块里面的新的值

![EBAFF0CC-6012-43D7-9B20-02DBA469A3DD](https://tva1.sinaimg.cn/large/006y8mN6ly1g70bvgcjfjj30n009j78u.jpg)

- 比如上述代码，就过就是1而不是0，还是要记住，block的定义和执行是分为两块的，两者可能会垮文件，相隔十万八千里
- 但只有执行完了代码才会生效

# 截获的自动变量

- 这一段主要讲的东西我认为其实就是我们捕获到的东西不能赋值，但是如果只是拿来用的话是没问题的