[TOC]
# 关于我的仓库

- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文主要是对于《高级编程》类似于总结的学习笔记
- 其实这一部分本质上就是根据多个block的源代码实例，分析其背后真正的实现原理
- 在这一块，我会以例子带入来讲，尽可能把每一个block的源码讲清楚
- 这一块难就难在它的顺序很乱其实，这个知识点会涉及到好几个别的知识点，这些知识点我就统一写在后面，大家看到看不懂的地方，看目录翻后面的就行
- 每一部分的源代码都是现场手动生成

# 准备工作

- 阅读《Objective-C 高级编程》中的p.91 ~ 136

# 最简单block

```objective-c
//OC代码
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        void (^blk)(void) = ^{
            
            printf("Block\n");
        };
        
        blk();
    }
    return 0;
}
```

```c++
//经过clang转换后的C++代码
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself {

            printf("Block\n");
}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 
        void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA));

        ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    }
    return 0;
}
```

- 我们把代码分成几块，一块一块分析

## __block_impl结构体

```c++
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};
```

- 名字中的impl即implementation的缩写，换句话说这一部分是block的实现部分结构体
- void *isa：
  - C语言中void * 为 “不确定类型指针”，void *可以用来声明指针。
  - 看到isa就会联想到之前在objc_class结构体，因此我们的block本质上也是一个对象【而且是个类对象】
  - 我们知道实例对象->类对象->元类构成了isa链中的一条，而这个__block_impl结构体占据的是中间类对象的位置
  - 实例对象应该是生成的block变量，个人认为
  - 因此这里的isa指针会指向元类，这里的元类主要是为了说明这个块的存储区域【详见：Block存储域&&Block元类】
- int Flags：
  - 标识符，在实现block的内部操作时会用到
- int Reserved：
  - 注明今后版本升级所需区域大小`Reserved`
  - 一般就是填个0
- void *FuncPtr：
  - 函数指针
  - 实际执行的函数，也就是block中花括号里面的代码内容，最后是转化成一个C语言函数执行的

## struct __main_block_impl_0结构体
```c++
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
```
- 该结构体才是块的完整结构
- struct __block_impl impl：
  - 就是上面的结构体，作为块的实现部分
- struct __main_block_desc_0* Desc：
  - 这里的desc即description，作为块的补充信息
  - 下面分析
- `__main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0)`
  - 这是该结构体的构造函数
  - 其中fp是实际执行的C语言函数指针
  - int flags = 0是C++中的缺省参数，表示默认是0
  - 具体内容就是对impl中相应的内容进行赋值，要说明的是impl.isa = &_NSConcreteStackBlock这个参看Block存储域&&Block元类

## `static void __main_block_func_0(struct __main_block_impl_0 *__cself)`

```c++
static void __main_block_func_0(struct __main_block_impl_0 *__cself {

            printf("Block\n");
}
```

- 这一块就是Blcok执行的实际代码块，就如我上面所说，它被转换为了一个C++函数
- 它也是上面的fp函数指针指向的内容
- 这里要注意的是传入的这个`__cself`参数，他其实就是C语言版的self，代表的就是block本身，毕竟其数据类型就是`struct __main_block_impl_0`
- 当然从这段代码看不出来传入的cself有什么用，因为我们的代码就只输出一段话，没有用到捕获的变量，后面会讲到cself到底怎么用

## `static struct __main_block_desc_0`

```c++
static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};
```

- size_t reserved：
  - 今后版本升级所需区域大小
  - 一般就填0
- size_t Block_size：
  - Block大小
- `__main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};`：
  - 这就是和我们平时用结构体一样，在定义完最后写一个结构体实例变量，变量名就是`__main_block_desc_0_DATA`
  - 其中reserved为0，Block_size是`sizeof(struct __main_block_impl_0)`

## 主函数【blk实际调用】

```c++
int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 
        void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA));

        ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    }
    return 0;
}
```
- 正常人看到这一段都晕了🌚
- 我们把所有强制类型转换去掉，看个正常人看的懂的版本：

```C++
void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA));
// 这一段就是通过构造函数构造一个__main_block_impl_0结构体赋值给blk变量
// 翻译如下
struct __main_block_impl_0 temp = __main_block_impl_0(__main_block_func_0, &__main_block_desc_0_DATA);
struct __main_block_impl_0 blk = &temp;

// 下面是调用block中函数的过程，我们可以看到我们要调用的其实就是FuncPtr这个函数指针指向的函数
// 查看__main_block_func_0的参数，发现就是我们上面研究的cself
// 所以，该调用翻译如下：
((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
blk->FuncPtr(blk);
```

- 上面值得注意的是，在使用构造函数的时候，我们传入的参数一个是我们的block函数指针，一个是在定义结构体的时候定义的__main_block_desc_0_DATA
- 好，这就是不涉及截获自动变量的最简单Block分析，下面我们来看捕获自动变量的情况

# 截获自动变量的block

- 这里我们又需要看一波C++源码，这里面会有很多一样的代码，我就不做分析了

- GOGOGO

```c++
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        int dmy = 256;
        int val = 10;
        const char  *fmt = "val = %d\n";
        void (^blk)(void) = ^{
            
            printf(fmt, val);
        };
        val = 2;
        fmt = "THESE VALUES WERE CHANGED. val = %d\n";
        blk();
    }
    return 0;
}
```

```c++
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  const char *fmt;
  int val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, const char *_fmt, int _val, int flags=0) : fmt(_fmt), val(_val) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself {
  const char *fmt = __cself->fmt; // bound by copy
  int val = __cself->val; // bound by copy
  
	printf(fmt, val);
}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};
int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 
        int dmy = 256;
        int val = 10;
        const char *fmt = "val = %d\n";
        void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, fmt, val));
        val = 2;
        fmt = "THESE VALUES WERE CHANGED. val = %d\n";
        ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    }
    return 0;
}
```

## 新的`__main_block_impl_0`

- 我们会看到`__block_impl`结构体没有任何变化，而`__main_block_impl_0`多了点东西

```c++
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  const char *fmt;
  int val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, const char *_fmt, int _val, int flags=0) : fmt(_fmt), val(_val) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
```

- 我们的fmt，val这两个被block截获的自动变量被放入到该结构体当中，同时构造函数也发生了变化，构造时要给fmt，val赋值
- 这里我们就能大概猜出截获自动变量的原理了，自动变量会被存入block结构体
- 在这里也要注意我们等于是使用了一个长得一模一样，保存在结构体里的数来进行的赋值操作，所以我们不能对它进行赋值操作，因为我们操作的只能是我们自己建的数据，而不会是我们真正的变量

## 新的`__main_block_func_0`函数

- 这次就会用到上面说的cself了

```c++
static void __main_block_func_0(struct __main_block_impl_0 *__cself {
  const char *fmt = __cself->fmt; // bound by copy
  int val = __cself->val; // bound by copy
  
	printf(fmt, val);
}
```

- 这里在实际调用时，我们还是只需要传入一个cself，我们就会看到在函数内部，我们进行操作的拿来printf不是原来的fmt和val，而是通过块结构体保存的这两个值

# 没有截获自动变量，而是使用静态变量，全局变量情况

- 在前一篇文章，我们了解到，对于截获的自动变量，不能直接修改它的值，而对于静态变量，全局变量时OK的，我们来看下对于这些变量block是怎么处理的

```c++
int global_val = 10; // 全局变量
static int static_global_val = 20; // 静态全局变量

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        static int static_val = 30; // 静态局部变量
        
        void (^myLocalBlock)(void) = ^{
            global_val *= 1;
            static_global_val *= 2;
            static_val *= 3;
            
            printf("static_val = %d, static_global_val = %d, global_val = %d\n",static_val, static_global_val, global_val);
        };
        
        myLocalBlock();
    }
    return 0;
}
```

```c++
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

int global_val = 10;
static int static_global_val = 20;


struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  int *static_val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int *_static_val, int flags=0) : static_val(_static_val) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  int *static_val = __cself->static_val; // bound by copy

            global_val *= 1;
            static_global_val *= 2;
            (*static_val) *= 3;

            printf("static_val = %d, static_global_val = %d, global_val = %d\n",(*static_val), static_global_val, global_val);
        }

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};
int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 
        static int static_val = 30;

        void (*myLocalBlock)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, &static_val));

        ((void (*)(__block_impl *))((__block_impl *)myLocalBlock)->FuncPtr)((__block_impl *)myLocalBlock);
    }
    return 0;
}
```

## 新的`__main_block_impl_0`

```c++
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  int *static_val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int *_static_val, int flags=0) : static_val(_static_val) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
```

- 我们发现全局变量，静态全局变量，我们的Block都没有用结构体去特地保存它
- 只有对于我们的静态局部变量会来保存，但这里要注意，我们使用的不是int static_val，而是int *static_val
- 也就是说我们使用一个指针来保存的静态局部变量
- 它会直接保存该变量的地址，之后的操作也是直接对该值本身进行操作，而不是向之前截获的那些变量，等于是重新开辟空间进行保存

## 新的`__main_block_func_0`函数

```c++
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  int *static_val = __cself->static_val; // bound by copy

            global_val *= 1;
            static_global_val *= 2;
            (*static_val) *= 3;

            printf("static_val = %d, static_global_val = %d, global_val = %d\n",(*static_val), static_global_val, global_val);
}
```

- 这里我们使用的global_val以及static_global_val都是直接调用，只有static_val是通过指针获取值，进行修改
- 那么这种做法看起来很不错，为什么在截获自动变量的时候我们不用指针传值而是要用值传值呢？
  - 原因在于，我们的静态变量是存在数据区的，在程序结束前它其实一直都会存在，之所以会被称为局部，只是说出了作用域无法调用到它了，并不是说这块数据不存在了。因此我们只要自己准备好一个指针，保证出了作用域依然能调用到他就行；而对于自动变量，它们真正的问题在于一但出了作用域，直接被释放了，所以要在结构体里开辟空间重新存放，进行值传递

# 使用`__block`修饰符的情况

- 在前一篇文章，我们了解到，对于截获的自动变量，不能直接修改它的值，而对于静态变量，全局变量时OK的，我们来看下对于这些变量block是怎么处理的
- `__block 修饰符`类似于 `static`、`auto`、`register` 说明符，它们用于指定将变量值设置到哪个存储域中。例如`auto` 表示作为自动变量存储在**栈**中， `static`表示作为静态变量存储在**数据区**中。

```c++
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        
        __block int val = 10;
        void (^blk)(void) = ^{
            val = 1;
            printf("val = %d\n", val);
        };
        blk();
    }
    return 0;
}

```

```c++
struct __Block_byref_val_0 {
 void *__isa;
 __Block_byref_val_0 *__forwarding;
 int __flags;
 int __size;
 int val;
};

struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  __Block_byref_val_0 *val; // by ref
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, __Block_byref_val_0 *_val, int flags=0) : val(_val->__forwarding) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  __Block_byref_val_0 *val = __cself->val; // bound by ref

            (val->__forwarding->val) = 1;
            printf("val = %d\n", (val->__forwarding->val));
        }
static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {_Block_object_assign((void*)&dst->val, (void*)src->val, 8/*BLOCK_FIELD_IS_BYREF*/);}

static void __main_block_dispose_0(struct __main_block_impl_0*src) {_Block_object_dispose((void*)src->val, 8/*BLOCK_FIELD_IS_BYREF*/);}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
  void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);
  void (*dispose)(struct __main_block_impl_0*);
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};
int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 

        __attribute__((__blocks__(byref))) __Block_byref_val_0 val = {(void*)0,(__Block_byref_val_0 *)&val, 0, sizeof(__Block_byref_val_0), 10};
        void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, (__Block_byref_val_0 *)&val, 570425344));
        ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    }
    return 0;
}
```

- 当你觉得block的代码已经够多的时候，`__block`源代码反手给你个超级加倍告诉你什么才是多🌚

## `__Block_byref_val_0`结构体

```c++
struct __Block_byref_val_0 {
 void *__isa;
 __Block_byref_val_0 *__forwarding;
 int __flags;
 int __size;
 int val;
};
```

- 对于使用`__block`修饰的变量，不管在块里有没有使用，都会相应的给他生成一个结构体
- 这里的isa指针默认都是传的空，但实际上是对于C语言基础数据类型会是0，因为他们不是对象没有所属类，而对于对象其实isa指针指向的就是所属类
- 但为什么看源码会是全部都赋值为0呢，因为OC是一门动态语言，运行的时候才会确定下来，不放心的话可以通过class方法查看下
- 关于`__forwarding`参看`__block`的拷贝部分
- flags标志符位
- size大小
- val变量本身

## `__main_block_impl_0`结构体

```c++
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  __Block_byref_val_0 *val; // by ref
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, __Block_byref_val_0 *_val, int flags=0) : val(_val->__forwarding) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
```

- 这部分值得注意的是，对于我们的`__Block_byref_val_0`结构体，我们同样是用一个指针去保存，这么做的原因是通过`__block`修饰的变量可能会被不止一个block使用，使用指针可以保证其可以被多个block调用

## `__main_block_func_0`函数

```c++
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  __Block_byref_val_0 *val = __cself->val; // bound by ref

            (val->__forwarding->val) = 1;
            printf("val = %d\n", (val->__forwarding->val));
}
```

- 这里看到我们用val截获下来的就是一个`__Block_byref_val_0`结构体了，对它进行赋值的时候需要通过forwarding指针进行
- 下面我们先看下主函数

## 主函数

```c++
int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 

        __attribute__((__blocks__(byref))) __Block_byref_val_0 val = {(void*)0,(__Block_byref_val_0 *)&val, 0, sizeof(__Block_byref_val_0), 10};
        void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, (__Block_byref_val_0 *)&val, 570425344));
        ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    }
    return 0;
}
```

- 主要关注下`__Block_byref_val_0`结构体的赋值

```c++
__Block_byref_val_0 val = {
    (void*)0,
    (__Block_byref_val_0 *)&val, 
    0, 
    sizeof(__Block_byref_val_0), 
    10
};
```

- isa为0上面解释过了，forwarding为自身的地址，flags为0

![DE5552A4-9145-44BB-920B-1F4BA1350266](https://tva1.sinaimg.cn/large/006y8mN6ly1g73squ8x7bj30g808gjsv.jpg)

- 关于copy和dispose函数我们在下面讲对象的时候分析

# 使用`__block`修饰的OC对象

- 目前来看，这段应该是最后一段block使用的源代码了
- 兄弟们，顶住

```c++
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        __block NSObject *obj = [[NSObject alloc] init];
        NSLog(@"1:%@", obj);
        void (^blk)(void) = ^{
            
            obj = [[NSObject alloc] init];
            NSLog(@"2:%@", obj);
        };
        blk();
    }
    return 0;
}
```

```c++
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

// line 109
static void __Block_byref_id_object_copy_131(void *dst, void *src) {
 _Block_object_assign((char*)dst + 40, *(void * *) ((char*)src + 40), 131);
}
static void __Block_byref_id_object_dispose_131(void *src) {
 _Block_object_dispose(*(void * *) ((char*)src + 40), 131);
}

struct __Block_byref_obj_0 {
  void *__isa;
__Block_byref_obj_0 *__forwarding;
 int __flags;
 int __size;
 void (*__Block_byref_id_object_copy)(void*, void*);
 void (*__Block_byref_id_object_dispose)(void*);
 NSObject *obj;
};

struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  __Block_byref_obj_0 *obj; // by ref
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, __Block_byref_obj_0 *_obj, int flags=0) : obj(_obj->__forwarding) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  __Block_byref_obj_0 *obj = __cself->obj; // bound by ref


            (obj->__forwarding->obj) = ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"));
            NSLog((NSString *)&__NSConstantStringImpl__var_folders_fl_lthb7l6d16q33glgfvv7_md00000gn_T_main_de2130_mi_1, (obj->__forwarding->obj));
        }
static void __main_block_copy_0(struct __main_block_impl_0*dst, struct __main_block_impl_0*src) {_Block_object_assign((void*)&dst->obj, (void*)src->obj, 8/*BLOCK_FIELD_IS_BYREF*/);}

static void __main_block_dispose_0(struct __main_block_impl_0*src) {_Block_object_dispose((void*)src->obj, 8/*BLOCK_FIELD_IS_BYREF*/);}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
  void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*);
  void (*dispose)(struct __main_block_impl_0*);
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0), __main_block_copy_0, __main_block_dispose_0};
int main(int argc, const char * argv[]) {
    /* @autoreleasepool */ { __AtAutoreleasePool __autoreleasepool; 
        __attribute__((__blocks__(byref))) __Block_byref_obj_0 obj = {(void*)0,(__Block_byref_obj_0 *)&obj, 33554432, sizeof(__Block_byref_obj_0), __Block_byref_id_object_copy_131, __Block_byref_id_object_dispose_131, ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"))};
        NSLog((NSString *)&__NSConstantStringImpl__var_folders_fl_lthb7l6d16q33glgfvv7_md00000gn_T_main_de2130_mi_0, (obj.__forwarding->obj));
        void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, (__Block_byref_obj_0 *)&obj, 570425344));
        ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    }
    return 0;
}
```

## `__Block_byref_obj_0`

```c++
struct __Block_byref_obj_0 {
  void *__isa;
__Block_byref_obj_0 *__forwarding;
 int __flags;
 int __size;
 void (*__Block_byref_id_object_copy)(void*, void*);
 void (*__Block_byref_id_object_dispose)(void*);
 NSObject *obj;
};
```

- 我们结合主函数中给改结构体初始化的语句来研究下初值是怎么赋的：

```c++
// 原文初始化obj
__attribute__((__blocks__(byref))) __Block_byref_obj_0 obj = {(void*)0,(__Block_byref_obj_0 *)&obj, 33554432, sizeof(__Block_byref_obj_0), __Block_byref_id_object_copy_131, __Block_byref_id_object_dispose_131, ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"))};

// 精简后初始化obj
obj = {
  (void*)0,	// isa
  (__Block_byref_obj_0 *)&obj,	// __forwarding
  33554432,	//	__flags
  sizeof(__Block_byref_obj_0),	// __size
  __Block_byref_id_object_copy_131,	// __Block_byref_id_object_copy
  __Block_byref_id_object_dispose_131,	// __Block_byref_id_object_dispose
  ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"))	// obj
}
```

- 抛去那些我们已经能理解的，来看下一些新东西：
- flags = 33554432
  - 33554432即二进制的10000000000000000000000000即1 << 25
  - BLOCK_HAS_COPY_DISPOSE =  (1 << 25), // compiler 译：compiler 含有copy_dispose助手【即拥有copy和dispose函数】
  - 这一块参看：解析Block.private
- size
  
  - 没什么好说的
- __Block_byref_id_object_copy
  
  - 这一块参看：解析Block.private
- __Block_byref_id_object_dispose
  
  - 这一块参看：解析Block.private
- C语言结构体中，编译器很难处理对象的初始化和销毁操作，所以使用runtime来管理相关内存。

 # block 循环引用

- 举个例子，下面是一个类的`init`方法,`blk_`是该类的一个成员变量：

```objective-c
- (id)init {
	self = [super init];
	blk_ = ^{NSLog(@"self = %@", self);};
	return self;
}
```

- 初始化这个类的实例时，就会造成循环引用，因为 Block 语法赋值在了成员变量 `blk_`中，因此通过 Block 语法生成在栈上的 Block 此时由栈复制到堆，并持有所使用的 self。self 持有 Block，Block 持有 self。这正是循环引用。

- 注意：**Block 内使用类的成员变量实际截获的是这个类本身（self）。**对编译器来说，成员变量只不过是对象结构体的成员变量。所以如何Block是该类的成员变量，截获该类其他成员变量时，会造成循环引用。

# Block存储域&&Block元类

- 在__block_impl结构体中，我们的isa指向一共有三种，也就是Block的三个元类
- `_NSConcreteStackBlock`、`_NSConcreteGlobalBlock`、`_NSConcreteMallocBlock`
- 这三个元类本质上是说明该block是存在栈，数据，堆区

![FAD217A2-5F67-40CE-8EF0-97B39C5F1E6F](https://tva1.sinaimg.cn/large/006y8mN6ly1g72kbzxl9kj30kp0fzn1g.jpg)

## _NSConcreteGlobalBlock

- 在两种情况下使用的block会是 `_NSConcreteGlobalBlock` 类对象
  - 使用block作为全局变量，由于在使用全局变量的地方不能使用自动变量，所以不存在对自动变量进行截获的说法；换句话说，该block实例不依赖于执行时的状态，在整个程序中只需要一个实例
  - 另外就是在Block语法中没有截获自动变量
    - 请注意这里说的是自动变量，如果截获的是静态变量或者全局变量，依然是算没有截获自动变量的
    - 其实这么说第一点也是白瞎，在定义全局的地方肯定无法使用全局变量
    - 因此规则就记住凡事没有截获自动变量的就是`_NSConcreteGlobalBlock`就完事了，吵吵啥呢
- `_NSConcreteGlobalBlock`类的Block存储在**『程序的数据区域』**
- 我的理解就是，对于没有要截获自动变量的block，我们不需要依赖于其运行时的状态【捕获的变量】，这样我们就不涉及到block的copy情况，因此是放在数据区
- 关于Block的copy参看：Block的拷贝情况
- 此外要注意的是，通过clang编译出来的isa在第二种情况下会显示成stackblock，这是因为OC是一门动态语言，真正的元类还是在运行的情况下确定的，这种情况下可以使用lldb调试器查看

## _NSConcreteStackBlock

- stackBlock使用情况就跟简单了，上面两种情况之外的情况都是stackBlock
- 换句话说，凡是捕获了自动变量的Block都是stackBlock
- 对于数据区上的全局Block就算出了作用域也能使用，因为在数据区，不会被释放，直到程序结束；在出了作用域后，依然可以使用指针找到这个块
- 而对于StackBlock就没有那么好的事了，一旦作用域结束，Block就会被废弃；同样，`__block`也配置在栈上当`__block`也配置在栈上，当`__block`的作用域结束的时候，`__block`也会被废弃

![36D368CB-F071-4BB8-A559-EF531F8D2569](https://tva1.sinaimg.cn/large/006y8mN6ly1g72mn5082pj30u80dj0xl.jpg)

##  _NSConcreteMallocBlock

- 上面两个block好像已经把所有情况都包括进去了，那这个`_NSConcreteMallocBlock`有什么用呢，其实完全是为了block的拷贝存在的
- 不会有任何一个块一上来就被存在堆区，请牢记这一点！
- `_NSConcreteMallocBlock`存在的意义和autorelease一样，就是为了能延长block的作用域
- 我们将block对象和`__blcok`对象从栈区复制到堆区，这样就算栈上的block被废弃了，还是可以使用堆上那一个
- 可以联想我们在ARC是如何处理返回值中的`__strong`的，大概同理
- 在这里要思考一个问题：在栈上和堆上同时有一个block的情况下，我们的赋值，修改，废弃操作应该怎样管理？
- 具体使用，参看Block的拷贝情况

![609AFF6A-2136-49C8-B912-BFB40A4E496B](https://tva1.sinaimg.cn/large/006y8mN6ly1g72mzpckdjj30qu0ja7bk.jpg)

# Block的拷贝情况

## 编译器判断的Block的自动拷贝

- 在ARC中，在大多数情况下，我们的编译器都会适当地进行判断
- 在以下三种情况下，编译器会给我们自动复制：
  - 将Block作为函数返回值返回时，会自动拷贝
  - 向方法或函数的参数中传递 Block时，使用以下两种方法的情况下，会进行自动拷贝：
    - 属于Cocoa框架方法且方法名中含有`usingBlock`等时
    - `Grand Central Dispatch（GCD）` 的 API，其中大量用到的block也是会进行复制的
  - 将 Block 赋值给类的附有 `__strong`修饰符的`id`类型或 Block 类型成员变量时【当然这种情况就是最多的，只要赋值一个block变量就会进行复制】

## 手动拷贝
- 通过copy方法来对Block进行手动拷贝，当我们不确定Block是否会被遗弃，需不需要拷贝时，直接使用copy方法就行，不会造成任何问题
##  Block 不同类的拷贝效果

|        Block 类        |    存储区域    |   拷贝效果   |
| :--------------------: | :------------: | :----------: |
| _NSConcreteStackBlock  |      栈区      | 从栈拷贝到堆 |
| _NSConcreteGlobalBlock | 程序的数据区域 |   不做改变   |
| _NSConcreteMallocBlock |      堆区      | 引用计数增加 |
# `__block`变量的拷贝

- 首先`__block`的拷贝还是跟着block走的，在使用`__block` 变量的 Block 从栈复制到堆上时，`__block` 变量也会受到如下影响：

| __block 变量的配置存储区域 |   Block 从栈复制到堆时的影响    |
| :------------------------: | :-----------------------------: |
|            栈区            | 从栈复制到堆，并被 Block 所持有 |
|            堆区            |         被 Block 所持有         |

- 一旦到了堆上，也就是按照OC的内存管理机制来进行管理，此时两者的关系就从block使用`__block`变成了block持有`__block`

- ![C5C74FD8-D382-4CEE-BB11-BF4FECDB2676](https://tva1.sinaimg.cn/large/006y8mN6ly1g72wpg1yzhj30ih0b2djg.jpg)
- ![8AA4AE6C-CD09-4AC5-B860-183271BCA1D3](https://tva1.sinaimg.cn/large/006y8mN6ly1g72wqbfkouj30pu0kqgtz.jpg)
- ![8C276B23-89A3-49B7-9581-80D7FE6E9107](https://tva1.sinaimg.cn/large/006y8mN6ly1g72wqvn148j30r60l0qb0.jpg)

## `__forwarding`永远都能正确访问变量

- 在复制的过程中就会出现这样一个问题，由于在栈和堆上都会有我们的`__block`，我们怎么找到我们需要的那个呢？
- 这就用到了`__forwarding`指针，它在没有复制的时候就是简单的指向自己，而当进行复制以后，就会指向堆上的那个`__block`变量

![1B559743-50F3-49AE-9A28-50205F9FF0EE](https://tva1.sinaimg.cn/large/006y8mN6ly1g73fm2qlifj30r70le0zp.jpg)

- 这样讲可能有点抽象，我再来举个例子，帮助理解：

```c++
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        
        __block int val = 1;
        val++;
        printf("1. val = %d\n", val);
        void (^blk)(void) = ^{
            val++;
            printf("2. val = %d\n", val);
        };
        val++;
        printf("3. val = %d\n", val);
        blk();
        val++;
        printf("4. val = %d\n", val);
    }
    return 0;
}

// 打印结果为：
1. val = 2
3. val = 3
2. val = 4
4. val = 5
```

- 这里面第一个val还是在对于在栈上的val进行自加操作，而之后的2 3 4都是对于堆上的val进行的操作了
- 但不论是栈上还是堆上的val，它们都可以这么表示：

```c++
++(val->__forwarding->val);
```

- 第一个val指的是结构体val，之后通forwarding指向正确的当前结构体，堆上有的话就是指向堆上的那个结构体，而第二个val指向的是结构体中的变量val，通过这样一番操作，我们就无论如何都可以锁定到正确的变量val进行操作

# 解析Block.private

- 在Block.private文件里面会解释真·block源码
- 包括block的拷贝究竟是怎么实现的，block的结构为什么会变来变去等等就能得到解答
- 一如block深似海，发现private才知刚下水

## Block的定义

```c++
// Values for Block_layout->flags to describe block objects
enum {
    BLOCK_DEALLOCATING =      (0x0001),  // runtime
    BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime
    BLOCK_NEEDS_FREE =        (1 << 24), // runtime
    BLOCK_HAS_COPY_DISPOSE =  (1 << 25), // compiler
    BLOCK_HAS_CTOR =          (1 << 26), // compiler: helpers have C++ code
    BLOCK_IS_GC =             (1 << 27), // runtime
    BLOCK_IS_GLOBAL =         (1 << 28), // compiler
    BLOCK_USE_STRET =         (1 << 29), // compiler: undefined if !BLOCK_HAS_SIGNATURE
    BLOCK_HAS_SIGNATURE  =    (1 << 30), // compiler
    BLOCK_HAS_EXTENDED_LAYOUT=(1 << 31)  // compiler
};

#define BLOCK_DESCRIPTOR_1 1
struct Block_descriptor_1 {
    uintptr_t reserved;
    uintptr_t size;
};

#define BLOCK_DESCRIPTOR_2 1
struct Block_descriptor_2 {
    // requires BLOCK_HAS_COPY_DISPOSE
    BlockCopyFunction copy;
    BlockDisposeFunction dispose;
};

#define BLOCK_DESCRIPTOR_3 1
struct Block_descriptor_3 {
    // requires BLOCK_HAS_SIGNATURE
    const char *signature;
    const char *layout;     // contents depend on BLOCK_HAS_EXTENDED_LAYOUT
};

struct Block_layout {
    void *isa;
    volatile int32_t flags; // contains ref count
    int32_t reserved;
    BlockInvokeFunction invoke;
    struct Block_descriptor_1 *descriptor;
    // imported variables
};
```

- 画了下其对应的结构：

- ![为了拥有更好的博客而画图.001](https://tva1.sinaimg.cn/large/006y8mN6ly1g741yegu9kj30sg0lcn7b.jpg)

  - 在上面的结构里虽然Block_layout里只有一个Block_descriptor_1，但实际上会根据设定的flag值增加Block_descriptor_2或者Block_descriptor_3，但是在上面的结构里虽然Block_layout里只有一个Block_descriptor_1是一定会有的
  
- Block_layout结构体成员含义如下：
  - isa: 所属类指针，说明block的类型，就是`_NSConcreteStackBlock`，`_NSConcreteMallocBlock`，`_NSConcreteGlobalBlock`这几个，说明OC本身也是一个对象。
  - flags: 标志变量
  
  ```c++
  enum {
    	BLOCK_DEALLOCATING =      (0x0001),  // runtime
      BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime	用来标识栈Block
      BLOCK_NEEDS_FREE =        (1 << 24), // runtime	用来标识堆
      BLOCK_HAS_COPY_DISPOSE =  (1 << 25), // compiler compiler 含有copy_dispose助手	如果标记为BLOCK_HAS_COPY_DISPOSE代表含有copy_dispose助手，说明该lay_out拥有Block_descriptor_2
      BLOCK_HAS_CTOR =          (1 << 26), // compiler: helpers have C++ code
      BLOCK_IS_GC =             (1 << 27), // runtime
      BLOCK_IS_GLOBAL =         (1 << 28), // compiler 是否为全局Block
      BLOCK_USE_STRET =         (1 << 29), // compiler: undefined if !BLOCK_HAS_SIGNATURE
      BLOCK_HAS_SIGNATURE  =    (1 << 30), // compiler	判断是否有签名
      BLOCK_HAS_EXTENDED_LAYOUT=(1 << 31)  // compiler	
  };
  
  // 当然，这几个flag是可以通过与操作组合在一起的
  // 关于签名这一块，参看：NSInvocation动态调用Block
  ```
  
  - Reserved: 保留变量
  - invoke: block执行时调用的函数指针，block内部的执行代码都在这个函数
  - descriptor: block的详细描述，一定包含Block_descriptor_1，在面对不同的情况时，编译器会修改lay_out结构，按需增加
  - variables: block范围外的变量，如果block没有调用任何外部变量，该变量就不存在

## `__block`变量的定义

```c++
// Values for Block_byref->flags to describe __block variables
enum {
    // Byref refcount must use the same bits as Block_layout's refcount.
    // BLOCK_DEALLOCATING =      (0x0001),  // runtime
    // BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime

    BLOCK_BYREF_LAYOUT_MASK =       (0xf << 28), // compiler
    BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler
    BLOCK_BYREF_LAYOUT_NON_OBJECT = (  2 << 28), // compiler
    BLOCK_BYREF_LAYOUT_STRONG =     (  3 << 28), // compiler
    BLOCK_BYREF_LAYOUT_WEAK =       (  4 << 28), // compiler
    BLOCK_BYREF_LAYOUT_UNRETAINED = (  5 << 28), // compiler

    BLOCK_BYREF_IS_GC =             (  1 << 27), // runtime

    BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler
    BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime
};

struct Block_byref {
    void *isa;
    struct Block_byref *forwarding;
    volatile int32_t flags; // contains ref count
    uint32_t size;
};

struct Block_byref_2 {
    // requires BLOCK_BYREF_HAS_COPY_DISPOSE
    BlockByrefKeepFunction byref_keep;
    BlockByrefDestroyFunction byref_destroy;
};

struct Block_byref_3 {
    // requires BLOCK_BYREF_LAYOUT_EXTENDED
    const char *layout;
};

```

- isa
  
  - 指向其所属类，初始为0，运行时动态判断
  
- forwarding
  
  - 指向自身/堆上的复制
  
- flags
  
  - ```c++
    // Values for Block_byref->flags to describe __block variables
    enum {
        // Byref refcount must use the same bits as Block_layout's refcount.
        // BLOCK_DEALLOCATING =      (0x0001),  // runtime
        // BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime
    
        BLOCK_BYREF_LAYOUT_MASK =       (0xf << 28), // compiler
        BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler
        BLOCK_BYREF_LAYOUT_NON_OBJECT = (  2 << 28), // compiler
        BLOCK_BYREF_LAYOUT_STRONG =     (  3 << 28), // compiler
        BLOCK_BYREF_LAYOUT_WEAK =       (  4 << 28), // compiler
        BLOCK_BYREF_LAYOUT_UNRETAINED = (  5 << 28), // compiler
    
        BLOCK_BYREF_IS_GC =             (  1 << 27), // runtime
    
        BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler
        BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime
    };
    ```
  
  - 这个结构和上面基本一样，上面没有见名也能知意
  
  - 值得注意的是这三个：
  
  - ```c++
    BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler	含有layout
    BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler	含有copy_dispose函数，在__block捕获的变量为对象时就会生成copy_dispose函数用来管理对象内存
    BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime	判断是否要释放
    ```
  
- size
  
  - 所占内存大小
  
- Block_byref_2、Block_byref_3

  - Block_byref_2和Block_byref_3用来保存附加信息

# Block_copy以及Block_release的底层实现

- 这部分源码在[runtime.cpp](https://opensource.apple.com/source/libclosure/libclosure-73/runtime.cpp)

## Block_copy

- 在`Block.h`中，Block_copy是如此定义的：

```c++
// Create a heap based copy of a Block or simply add a reference to an existing one.
// This must be paired with Block_release to recover memory, even when running
// under Objective-C Garbage Collection.

// 创建块的基于堆的副本， 或者只需添加对现有块的引用
// 这必须与Block_release配对才能恢复内存， 即使在运行时
// 在Objective-C垃圾收集机制下
BLOCK_EXPORT void *_Block_copy(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
```

- 它的实现部分在runtime.cpp里面：

```c++
// Copy, or bump refcount, of a block.  If really copying, call the copy helper if present.

// 块的复制或凹凸【???】引用计数。如果真的要复制，请调用复制助手（如果存在）
// 应该是说不要显式调用， 不过复制助手也是不能显式调用的
void *_Block_copy(const void *arg) {
    struct Block_layout *aBlock;

    if (!arg) return NULL;
    
    // The following would be better done as a switch statement
  
    // 下面最好使用switch语句
  	// 这是FIXME？
    aBlock = (struct Block_layout *)arg;
    if (aBlock->flags & BLOCK_NEEDS_FREE) {
        // latches on high
        latching_incr_int(&aBlock->flags);
        return aBlock;
    }
    else if (aBlock->flags & BLOCK_IS_GLOBAL) {
        return aBlock;
    }
    else {
        // Its a stack block.  Make a copy.
      	
      	// 这是一个栈上的block，进行拷贝
        struct Block_layout *result =
            (struct Block_layout *)malloc(aBlock->descriptor->size);
        if (!result) return NULL;
        memmove(result, aBlock, aBlock->descriptor->size); // bitcopy first
      
      	// 首先进行位拷贝
#if __has_feature(ptrauth_calls)
        // Resign the invoke pointer as it uses address authentication.
      
      	// 在调用指针使用地址身份验证时将其重新制定
        result->invoke = aBlock->invoke;
#endif
        // reset refcount
      
      	// 重置引用计数
        result->flags &= ~(BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING);    // XXX not needed
      
      	// 不需要XXX
        result->flags |= BLOCK_NEEDS_FREE | 2;  // logical refcount 1
      	
      	// 逻辑引用计数1
        _Block_call_copy_helper(result, aBlock);
        // Set isa last so memory analysis tools see a fully-initialized object.
      
      	// 设置isa指向，以便于分析内存工具能够看到完全初始化的对象
        result->isa = _NSConcreteMallocBlock;
        return result;
    }
}
```

- 在详细分析copy实现过程前，我们先对其用到的子函数进行一些分析：

### `latching_incr_int()`

```c++
static int32_t latching_incr_int(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return BLOCK_REFCOUNT_MASK;
        }
        if (OSAtomicCompareAndSwapInt(old_value, old_value+2, where)) {
            return old_value+2;
        }
    }
}
```

- latch:占有；incr:增量寄存器
- 这一块就是对block的应用计数+1的操作，我们可以看到显然block的引用计数是通过flags去管理的
- 进入一个死循环，如果flags含有`BLOCK_REFCOUNT_MASK`证明其引用计数达到最大，直接返回，需要三万多个指针指向，正常情况下不会出现;其余情况下，使用`OSAtomicCompareAndSwapInt`函数【在`where`取值与`old_value`相同时，将`old_value + 2`赋给`where`】
- 这里值得注意的是，引用计数每次是 + 2的，block的引用计数是以flags的后16位代表的，以2为单位，每次递增2，1被`BLOCK_DEALLOCATING`占用

### 详解`Block_copy`

- 先对于传入的`Block_layout`结构体进行判断，如果为NULL就直接返回，接下来进入if判断【就是注释里说的可以使用switch】
  1. 对于堆上的block【`BLOCK_NEEDS_FREE`，话说这个宏真是简单不做作，堆上的数据就是需要释放的】，进入`latching_incr_int`函数对其进行引用计数增加操作【flags】
  2. 对于全局变量，不做任何操作，直接返回【也就是说，copy函数会一直调用喽】
  3. 剩下的情况就是在栈上的block
- 对于栈上的block进行拷贝：

  1. 先分配一块和原block一样大小的内存空间【失败直接返回】
  2. 使用memmove()函数进行位复制，将原block的每个字节都拷贝过去【memmove用于拷贝字节，如果目标区域和源区域有重叠的话，memmove能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中，但复制后源内容会被更改。但是当目标区域与源区域没有重叠则和memcpy函数功能相同】
  3. 将新Block的引用计数置零。`BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING`就是0xffff，~(0xffff)就是0x0000【所以咱就不能直接写一个0000宏么】，result->flags与0x0000与等就将result->flags的后16位置零。然后将新Block标识为堆Block并将其引用计数置为2【此处存疑，在栈上的block之前的引用计数时用来干嘛的呢？】
  4. 接着调用copy助手，如果有copy函数的话直接调用，没有的话直接返回
  5. 设置isa指针，指向`_NSConcreteMallocBlock`

- 下面我们看一个例子：

```objective-c
NSObject *obj = [[NSObject alloc] init];
void(^aBlock)() = ^{
  
  obj;
};
NSLog(@"%ld", CFGetRetainCount((__bridge void *)obj));
```


- 这样子的打印结果会是3
- 由于obj时截获的外部自动变量，所以我们会用结构体指针指向它来进行保存，所以引用计数会加
- 在讲块赋值给aBlock的时候，由于块从栈上复制到了堆上，在堆上的block也会指向obj，导致其引用计数再加一

## Block_release

- 在`Block.h`中，Block_release是如此定义的：

```c++
// Lose the reference, and if heap based and last reference, recover the memory

// 失去引用，如果是这是基于堆的并且这是最后一个引用，恢复内存
BLOCK_EXPORT void _Block_release(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
```

- 它的实现部分在runtime.cpp里面：

```c++
// API entry point to release a copied Block

// 释放复制块的API入口点
void _Block_release(const void *arg) {
    struct Block_layout *aBlock = (struct Block_layout *)arg;
    if (!aBlock) return;
    if (aBlock->flags & BLOCK_IS_GLOBAL) return;
    if (! (aBlock->flags & BLOCK_NEEDS_FREE)) return;

    if (latching_decr_int_should_deallocate(&aBlock->flags)) {
        _Block_call_dispose_helper(aBlock);
        _Block_destructInstance(aBlock);
        free(aBlock);
    }
}
```

### `latching_decr_int_should_deallocate`

```c++
// return should_deallocate?
static bool latching_decr_int_should_deallocate(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return false; // latched high
        }
        if ((old_value & BLOCK_REFCOUNT_MASK) == 0) {
            return false;   // underflow, latch low
        }
        int32_t new_value = old_value - 2;
        bool result = false;
        if ((old_value & (BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING)) == 2) {
            new_value = old_value - 1;
            result = true;
        }
        if (OSAtomicCompareAndSwapInt(old_value, new_value, where)) {
            return result;
        }
    }
}
```

- 对于引用计数过高(0xfffe)，或者过低(0) ，返回false
- 如果引用计数为2，减1，说明正在释放，返回true
- 如果大于2，则将其引用计数-2返回false

### 详解`Block_release`

- 对于全局Block，非堆Block不进行处理
- 判断引用计数需不需要释放
- 对于判断出来为true的，也就是引用计数2的，调用助手函数中的dispose函数
- free

# `_Block_object_assign`以及`_Block_object_dispose`

- 在block中的助手函数里，两个助手函数copy和dispose，他们实际上时调用的`_Block_object_assign`以及`_Block_object_dispose`

```c++
__attribute__((__blocks__(byref))) __Block_byref_obj_0 obj = {(void*)0,(__Block_byref_obj_0 *)&obj, 33554432, sizeof(__Block_byref_obj_0), __Block_byref_id_object_copy_131, __Block_byref_id_object_dispose_131, ((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)((NSObject *(*)(id, SEL))(void *)objc_msgSend)((id)objc_getClass("NSObject"), sel_registerName("alloc")), sel_registerName("init"))};
```

- 千山万水就差一点，冲冲冲！
- 我们先来了解下其中会用到的枚举含义：

```c++
// Runtime support functions used by compiler when generating copy/dispose helpers

// Values for _Block_object_assign() and _Block_object_dispose() parameters
enum {
    // see function implementation for a more complete description of these fields and combinations
    BLOCK_FIELD_IS_OBJECT   =  3,  // id, NSObject, __attribute__((NSObject)), block, ...	OC对象类型
    BLOCK_FIELD_IS_BLOCK    =  7,  // a block variable	另一个block
    BLOCK_FIELD_IS_BYREF    =  8,  // the on stack structure holding the __block variable	为一个被__block修饰后生成的结构体
    BLOCK_FIELD_IS_WEAK     = 16,  // declared __weak, only used in byref copy helpers	被__weak修饰过的弱引用，只在Block_byref管理内部对象内存时使用，也就是__block __weak id 
    BLOCK_BYREF_CALLER      = 128, // called from __block (byref) copy/dispose support routines.	在处理Block_byref内部对象内存的时候会加的一个额外标记，配合上面的枚举一起使用；
};

enum {
    BLOCK_ALL_COPY_DISPOSE_FLAGS = 
        BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_BLOCK | BLOCK_FIELD_IS_BYREF |
        BLOCK_FIELD_IS_WEAK | BLOCK_BYREF_CALLER
};

```

## `_Block_object_assign`

```c++
/
// When Blocks or Block_byrefs hold objects then their copy routine helpers use this entry point
// to do the assignment.
//
  
// blocks或block_byrefs保存对象时，它们的复制例程助手使用此入口点进行赋值。
void _Block_object_assign(void *destArg, const void *object, const int flags) {
    const void **dest = (const void **)destArg;
  	///  destArg为执行Block_copy()后的block中的对象、block、或者BYREF指针的指针，object为copy之前的变量指针。
    switch (os_assumes(flags & BLOCK_ALL_COPY_DISPOSE_FLAGS)) {
      case BLOCK_FIELD_IS_OBJECT:
        // 
        /*******
        id object = ...;
        [^{ object; } copy];
        ********/
				///	简单的指针赋值
        _Block_retain_object(object);
        *dest = object;
        break;

      case BLOCK_FIELD_IS_BLOCK:
        /*******
        void (^object)(void) = ...;
        [^{ object; } copy];
        ********/
				
        /// 当block捕获的变量为另外一个block时执行此步，copy一个新的block并赋值
        *dest = _Block_copy(object);
        break;
    
      case BLOCK_FIELD_IS_BYREF | BLOCK_FIELD_IS_WEAK:
      /// 当block捕获的变量为__block修饰的变量时会执行此步，执行byref_copy操作
      case BLOCK_FIELD_IS_BYREF:
      /// 当block捕获的变量为__block修饰的变量时会执行此步，执行byref_copy操作
        /*******
         // copy the onstack __block container to the heap
         // Note this __weak is old GC-weak/MRC-unretained.
         // ARC-style __weak is handled by the copy helper directly.
         __block ... x;
         __weak __block ... x;
         [^{ x; } copy];
         ********/

        *dest = _Block_byref_copy(object);
        break;
        
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK:
        /*******
         // copy the actual field held in the __block container
         // Note this is MRC unretained __block only. 
         // ARC retained __block is handled by the copy helper directly.
         __block id object;
         __block void (^object)(void);
         [^{ object; } copy];
         ********/

        *dest = object;
        break;

      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_WEAK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK  | BLOCK_FIELD_IS_WEAK:
        /*******
         // copy the actual field held in the __block container
         // Note this __weak is old GC-weak/MRC-unretained.
         // ARC-style __weak is handled by the copy helper directly.
         __weak __block id object;
         __weak __block void (^object)(void);
         [^{ object; } copy];
         ********/

        *dest = object;
        break;

      default:
        break;
    }
}
```

## `_Block_byref_copy`

```C++
// A closure has been copied and its fixup routine is asking us to fix up the reference to the shared byref data
/// 已经复制了一个闭包，它的修复例程要求我们修复对共享byref数据的引用
// Closures that aren't copied must still work, so everyone always accesses variables after dereferencing the forwarding ptr.
/// 未被复制的闭包必须仍然有效，因此每个人在取消对转发ptr的引用后总是访问变量。
// We ask if the byref pointer that we know about has already been copied to the heap, and if so, increment and return it.
/// 我们询问我们知道的byref指针是否已经复制到堆中，如果是，则递增并返回它。
// Otherwise we need to copy it and update the stack forwarding pointer
/// 否则我们需要复制它并更新堆栈转发指针
/// 机翻看的想吐🤮
static struct Block_byref *_Block_byref_copy(const void *arg) {
    struct Block_byref *src = (struct Block_byref *)arg;

    if ((src->forwarding->flags & BLOCK_REFCOUNT_MASK) == 0) {
        // src points to stack
      	/// 对于栈上的byref，先分配内存，isa设置为NULL
        struct Block_byref *copy = (struct Block_byref *)malloc(src->size);
        copy->isa = NULL;
        // byref value 4 is logical refcount of 2: one for caller, one for stack
      	/// 新的byref的引用计数要设置为4，标记为堆，一份为调用方，一份为栈所有，所以为4【还是2为单位】
        copy->flags = src->flags | BLOCK_BYREF_NEEDS_FREE | 4;
        copy->forwarding = copy; // patch heap copy to point to itself
        src->forwarding = copy;  // patch stack to point to heap copy
      	/// 将当前byref以及malloc的byref的forwarding都指向堆byref，操作堆栈都是同一份东西
        copy->size = src->size;

        if (src->flags & BLOCK_BYREF_HAS_COPY_DISPOSE) {
            // Trust copy helper to copy everything of interest
            // If more than one field shows up in a byref block this is wrong XXX
            struct Block_byref_2 *src2 = (struct Block_byref_2 *)(src+1);
            struct Block_byref_2 *copy2 = (struct Block_byref_2 *)(copy+1);
            copy2->byref_keep = src2->byref_keep;
          	// 执行byref的byref_keep函数(即assign函数，只是会加上BLOCK_BYREF_CALLER标志)，管理捕获的对象内存
            copy2->byref_destroy = src2->byref_destroy;

            if (src->flags & BLOCK_BYREF_LAYOUT_EXTENDED) {
                struct Block_byref_3 *src3 = (struct Block_byref_3 *)(src2+1);
                struct Block_byref_3 *copy3 = (struct Block_byref_3*)(copy2+1);
                copy3->layout = src3->layout;
            }

            (*src2->byref_keep)(copy, src);
        }
        else {
            // Bitwise copy.
            // This copy includes Block_byref_3, if any.
          
          	/// 如果捕获的是普通变量，就没有Block_byref_2，copy+1和src+1指向的就是Block_byref_3，执行字节拷贝。
            memmove(copy+1, src+1, src->size - sizeof(*src));
        }
    }
    // already copied to heap
  	/// 如果该byref是存在于堆，则只需要增加其引用计数。
    else if ((src->forwarding->flags & BLOCK_BYREF_NEEDS_FREE) == BLOCK_BYREF_NEEDS_FREE) {
        latching_incr_int(&src->forwarding->flags);
    }
    
    return src->forwarding;
}

```



## `_Block_object_dispose`

```c++
// When Blocks or Block_byrefs hold objects their destroy helper routines call this entry point
// to help dispose of the contents
void _Block_object_dispose(const void *object, const int flags) {
    switch (os_assumes(flags & BLOCK_ALL_COPY_DISPOSE_FLAGS)) {
      case BLOCK_FIELD_IS_BYREF | BLOCK_FIELD_IS_WEAK:
      case BLOCK_FIELD_IS_BYREF:
        // get rid of the __block data structure held in a Block
        _Block_byref_release(object);
        break;
      case BLOCK_FIELD_IS_BLOCK:
        _Block_release(object);
        break;
      case BLOCK_FIELD_IS_OBJECT:
        _Block_release_object(object);
        break;
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_WEAK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK  | BLOCK_FIELD_IS_WEAK:
        break;
      default:
        break;
    }
}
// 这部分有了上面的基础应该很好懂了
```

## `_Block_byref_release`

```C++
static void _Block_byref_release(const void *arg) {
    struct Block_byref *byref = (struct Block_byref *)arg;

    // dereference the forwarding pointer since the compiler isn't doing this anymore (ever?)
    byref = byref->forwarding;
    
    if (byref->flags & BLOCK_BYREF_NEEDS_FREE) {
        int32_t refcount = byref->flags & BLOCK_REFCOUNT_MASK;
        os_assert(refcount);
        if (latching_decr_int_should_deallocate(&byref->flags)) {
            if (byref->flags & BLOCK_BYREF_HAS_COPY_DISPOSE) {
                struct Block_byref_2 *byref2 = (struct Block_byref_2 *)(byref+1);
                (*byref2->byref_destroy)(byref);
            }
            free(byref);
        }
    }
}
```

- 首先赋值给forwaring
- 如果有助手的话执行destrory
- free

# 附录：Block.h文件

```c++
/*
 *  Block.h
 *
 * Copyright (c) 2008-2010 Apple Inc. All rights reserved.
 *
 * @APPLE_LLVM_LICENSE_HEADER@
 *
 */

#ifndef _Block_H_
#define _Block_H_

#if !defined(BLOCK_EXPORT)
#   if defined(__cplusplus)
#       define BLOCK_EXPORT extern "C" 
#   else
#       define BLOCK_EXPORT extern
#   endif
#endif

#include <Availability.h>
#include <TargetConditionals.h>

#if __cplusplus
extern "C" {
#endif

// Create a heap based copy of a Block or simply add a reference to an existing one.
// 添加块的基于堆的副本【？？？】，或者只是添加对于现有块的引用
// This must be paired with Block_release to recover memory, even when running
// 即使在运行时，它【复制品】也必须要与block_release配对才能恢复内存【也就是引用计数管理】
// under Objective-C Garbage Collection.
// 处在OC垃圾收集机制下
BLOCK_EXPORT void *_Block_copy(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);

// Lose the reference, and if heap based and last reference, recover the memory
// 失去引用，并且该引用为基于堆的引用并且是最后一个引用，恢复内存【dealloc】
BLOCK_EXPORT void _Block_release(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);


// Used by the compiler. Do not call this function yourself.
// 编译器调用，不要自行调用该函数
BLOCK_EXPORT void _Block_object_assign(void *, const void *, const int)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);

// Used by the compiler. Do not call this function yourself.
// 编译器调用，不要自行调用该函数
BLOCK_EXPORT void _Block_object_dispose(const void *, const int)
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);

// Used by the compiler. Do not call this function yourself.
// 编译器调用，不要自行调用该函数
BLOCK_EXPORT void * _NSConcreteGlobalBlock[32]
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
BLOCK_EXPORT void * _NSConcreteStackBlock[32]
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);


#if __cplusplus
}
#endif

// Type correct macros
// 键入正确的宏

#define Block_copy(...) ((__typeof(__VA_ARGS__))_Block_copy((const void *)(__VA_ARGS__)))
#define Block_release(...) _Block_release((const void *)(__VA_ARGS__))


#endif
```

## 补充知识：extern “C”

- 这是链接规范语法
- 规定：extern "language string"，说明之后的内容在应该按照指定语言的方式来处理
- C++编译器普遍支持"C"和"C++"，分别对应C语言和C++语言
- 更多相关内容[关于extern "C"（详细剖析）](https://blog.csdn.net/u010639500/article/details/87885421)

# 附录：Block_private.h文件

- [Block.private.h](https://opensource.apple.com/source/libclosure/libclosure-73/Block_private.h)

```c++
/*
 * Block_private.h
 *
 * SPI for Blocks
 *
 * Copyright (c) 2008-2010 Apple Inc. All rights reserved.
 *
 * @APPLE_LLVM_LICENSE_HEADER@
 *
 */

#ifndef _BLOCK_PRIVATE_H_
#define _BLOCK_PRIVATE_H_

#include <Availability.h>
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <Block.h>

#if __has_include(<ptrauth.h>)
#include <ptrauth.h>
#endif

#if __has_feature(ptrauth_calls) &&  __cplusplus < 201103L

// C ptrauth or old C++ ptrauth

#define _Block_set_function_pointer(field, value)                       \
    ((value)                                                            \
     ? ((field) =                                                       \
        (__typeof__(field))                                             \
        ptrauth_auth_and_resign((void*)(value),                         \
                                ptrauth_key_function_pointer, 0,        \
                                ptrauth_key_block_function, &(field)))  \
     : ((field) = 0))

#define _Block_get_function_pointer(field)                              \
    ((field)                                                            \
     ? (__typeof__(field))                                              \
       ptrauth_auth_function((void*)(field),                            \
                             ptrauth_key_block_function, &(field))      \
     : (__typeof__(field))0)

#else

// C++11 ptrauth or no ptrauth

#define _Block_set_function_pointer(field, value)       \
    (field) = (value)

#define _Block_get_function_pointer(field)      \
    (field)

#endif


#if __has_feature(ptrauth_calls)  &&  __cplusplus >= 201103L

// StorageSignedFunctionPointer<Key, Fn> stores a function pointer of type
// Fn but signed with the given ptrauth key and with the address of its
// storage as extra data.
// Function pointers inside block objects are signed this way.
template <typename Fn, ptrauth_key Key>
class StorageSignedFunctionPointer {
    uintptr_t bits;

 public:

    // Authenticate function pointer fn as a C function pointer.
    // Re-sign it with our key and the storage address as extra data.
    // DOES NOT actually write to our storage.
    uintptr_t prepareWrite(Fn fn) const
    {
        if (fn == nullptr) {
            return 0;
        } else {
            return (uintptr_t)
                ptrauth_auth_and_resign(fn, ptrauth_key_function_pointer, 0,
                                        Key, &bits);
        }
    }

    // Authenticate otherBits at otherStorage.
    // Re-sign it with our storage address.
    // DOES NOT actually write to our storage.
    uintptr_t prepareWrite(const StorageSignedFunctionPointer& other) const
    {
        if (other.bits == 0) {
            return 0;
        } else {
            return (uintptr_t)
                ptrauth_auth_and_resign((void*)other.bits, Key, &other.bits,
                                        Key, &bits);
        }
    }

    // Authenticate ptr as if it were stored at our storage address.
    // Re-sign it as a C function pointer.
    // DOES NOT actually read from our storage.
    Fn completeReadFn(uintptr_t ptr) const
    {
        if (ptr == 0) {
            return nullptr;
        } else {
            return ptrauth_auth_function((Fn)ptr, Key, &bits);
        }
    }

    // Authenticate ptr as if it were at our storage address.
    // Return it as a dereferenceable pointer.
    // DOES NOT actually read from our storage.
    void* completeReadRaw(uintptr_t ptr) const
    {
        if (ptr == 0) {
            return nullptr;
        } else {
            return ptrauth_auth_data((void*)ptr, Key, &bits);
        }
    }

    StorageSignedFunctionPointer() { }

    StorageSignedFunctionPointer(Fn value)
        : bits(prepareWrite(value)) { }

    StorageSignedFunctionPointer(const StorageSignedFunctionPointer& value)
        : bits(prepareWrite(value)) { }

    StorageSignedFunctionPointer&
    operator = (Fn rhs) {
        bits = prepareWrite(rhs);
        return *this;
    }

    StorageSignedFunctionPointer&
    operator = (const StorageSignedFunctionPointer& rhs) {
        bits = prepareWrite(rhs);
        return *this;
    }

    operator Fn () const {
        return completeReadFn(bits);
    }

    explicit operator void* () const {
        return completeReadRaw(bits);
    }

    explicit operator bool () const {
        return completeReadRaw(bits) != nullptr;
    }
};

using BlockCopyFunction = StorageSignedFunctionPointer
    <void(*)(void *, const void *),
     ptrauth_key_block_function>;

using BlockDisposeFunction = StorageSignedFunctionPointer
    <void(*)(const void *),
     ptrauth_key_block_function>;

using BlockInvokeFunction = StorageSignedFunctionPointer
    <void(*)(void *, ...),
     ptrauth_key_block_function>;

using BlockByrefKeepFunction = StorageSignedFunctionPointer
    <void(*)(struct Block_byref *, struct Block_byref *),
     ptrauth_key_block_function>;

using BlockByrefDestroyFunction = StorageSignedFunctionPointer
    <void(*)(struct Block_byref *),
     ptrauth_key_block_function>;

// c++11 and ptrauth_calls
#elif !__has_feature(ptrauth_calls)
// not ptrauth_calls

typedef void(*BlockCopyFunction)(void *, const void *);
typedef void(*BlockDisposeFunction)(const void *);
typedef void(*BlockInvokeFunction)(void *, ...);
typedef void(*BlockByrefKeepFunction)(struct Block_byref*, struct Block_byref*);
typedef void(*BlockByrefDestroyFunction)(struct Block_byref *);

#else
// ptrauth_calls but not c++11

typedef uintptr_t BlockCopyFunction;
typedef uintptr_t BlockDisposeFunction;
typedef uintptr_t BlockInvokeFunction;
typedef uintptr_t BlockByrefKeepFunction;
typedef uintptr_t BlockByrefDestroyFunction;

#endif


// Values for Block_layout->flags to describe block objects
enum {
    BLOCK_DEALLOCATING =      (0x0001),  // runtime
    BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime
    BLOCK_NEEDS_FREE =        (1 << 24), // runtime
    BLOCK_HAS_COPY_DISPOSE =  (1 << 25), // compiler
    BLOCK_HAS_CTOR =          (1 << 26), // compiler: helpers have C++ code
    BLOCK_IS_GC =             (1 << 27), // runtime
    BLOCK_IS_GLOBAL =         (1 << 28), // compiler
    BLOCK_USE_STRET =         (1 << 29), // compiler: undefined if !BLOCK_HAS_SIGNATURE
    BLOCK_HAS_SIGNATURE  =    (1 << 30), // compiler
    BLOCK_HAS_EXTENDED_LAYOUT=(1 << 31)  // compiler
};

#define BLOCK_DESCRIPTOR_1 1
struct Block_descriptor_1 {
    uintptr_t reserved;
    uintptr_t size;
};

#define BLOCK_DESCRIPTOR_2 1
struct Block_descriptor_2 {
    // requires BLOCK_HAS_COPY_DISPOSE
    BlockCopyFunction copy;
    BlockDisposeFunction dispose;
};

#define BLOCK_DESCRIPTOR_3 1
struct Block_descriptor_3 {
    // requires BLOCK_HAS_SIGNATURE
    const char *signature;
    const char *layout;     // contents depend on BLOCK_HAS_EXTENDED_LAYOUT
};

struct Block_layout {
    void *isa;
    volatile int32_t flags; // contains ref count
    int32_t reserved;
    BlockInvokeFunction invoke;
    struct Block_descriptor_1 *descriptor;
    // imported variables
};


// Values for Block_byref->flags to describe __block variables
enum {
    // Byref refcount must use the same bits as Block_layout's refcount.
    // BLOCK_DEALLOCATING =      (0x0001),  // runtime
    // BLOCK_REFCOUNT_MASK =     (0xfffe),  // runtime

    BLOCK_BYREF_LAYOUT_MASK =       (0xf << 28), // compiler
    BLOCK_BYREF_LAYOUT_EXTENDED =   (  1 << 28), // compiler
    BLOCK_BYREF_LAYOUT_NON_OBJECT = (  2 << 28), // compiler
    BLOCK_BYREF_LAYOUT_STRONG =     (  3 << 28), // compiler
    BLOCK_BYREF_LAYOUT_WEAK =       (  4 << 28), // compiler
    BLOCK_BYREF_LAYOUT_UNRETAINED = (  5 << 28), // compiler

    BLOCK_BYREF_IS_GC =             (  1 << 27), // runtime

    BLOCK_BYREF_HAS_COPY_DISPOSE =  (  1 << 25), // compiler
    BLOCK_BYREF_NEEDS_FREE =        (  1 << 24), // runtime
};

struct Block_byref {
    void *isa;
    struct Block_byref *forwarding;
    volatile int32_t flags; // contains ref count
    uint32_t size;
};

struct Block_byref_2 {
    // requires BLOCK_BYREF_HAS_COPY_DISPOSE
    BlockByrefKeepFunction byref_keep;
    BlockByrefDestroyFunction byref_destroy;
};

struct Block_byref_3 {
    // requires BLOCK_BYREF_LAYOUT_EXTENDED
    const char *layout;
};


// Extended layout encoding.

// Values for Block_descriptor_3->layout with BLOCK_HAS_EXTENDED_LAYOUT
// and for Block_byref_3->layout with BLOCK_BYREF_LAYOUT_EXTENDED

// If the layout field is less than 0x1000, then it is a compact encoding 
// of the form 0xXYZ: X strong pointers, then Y byref pointers, 
// then Z weak pointers.

// If the layout field is 0x1000 or greater, it points to a 
// string of layout bytes. Each byte is of the form 0xPN.
// Operator P is from the list below. Value N is a parameter for the operator.
// Byte 0x00 terminates the layout; remaining block data is non-pointer bytes.

enum {
    BLOCK_LAYOUT_ESCAPE = 0, // N=0 halt, rest is non-pointer. N!=0 reserved.
    BLOCK_LAYOUT_NON_OBJECT_BYTES = 1,    // N bytes non-objects
    BLOCK_LAYOUT_NON_OBJECT_WORDS = 2,    // N words non-objects
    BLOCK_LAYOUT_STRONG           = 3,    // N words strong pointers
    BLOCK_LAYOUT_BYREF            = 4,    // N words byref pointers
    BLOCK_LAYOUT_WEAK             = 5,    // N words weak pointers
    BLOCK_LAYOUT_UNRETAINED       = 6,    // N words unretained pointers
    BLOCK_LAYOUT_UNKNOWN_WORDS_7  = 7,    // N words, reserved
    BLOCK_LAYOUT_UNKNOWN_WORDS_8  = 8,    // N words, reserved
    BLOCK_LAYOUT_UNKNOWN_WORDS_9  = 9,    // N words, reserved
    BLOCK_LAYOUT_UNKNOWN_WORDS_A  = 0xA,  // N words, reserved
    BLOCK_LAYOUT_UNUSED_B         = 0xB,  // unspecified, reserved
    BLOCK_LAYOUT_UNUSED_C         = 0xC,  // unspecified, reserved
    BLOCK_LAYOUT_UNUSED_D         = 0xD,  // unspecified, reserved
    BLOCK_LAYOUT_UNUSED_E         = 0xE,  // unspecified, reserved
    BLOCK_LAYOUT_UNUSED_F         = 0xF,  // unspecified, reserved
};


// Runtime support functions used by compiler when generating copy/dispose helpers

// Values for _Block_object_assign() and _Block_object_dispose() parameters
enum {
    // see function implementation for a more complete description of these fields and combinations
    BLOCK_FIELD_IS_OBJECT   =  3,  // id, NSObject, __attribute__((NSObject)), block, ...
    BLOCK_FIELD_IS_BLOCK    =  7,  // a block variable
    BLOCK_FIELD_IS_BYREF    =  8,  // the on stack structure holding the __block variable
    BLOCK_FIELD_IS_WEAK     = 16,  // declared __weak, only used in byref copy helpers
    BLOCK_BYREF_CALLER      = 128, // called from __block (byref) copy/dispose support routines.
};

enum {
    BLOCK_ALL_COPY_DISPOSE_FLAGS = 
        BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_BLOCK | BLOCK_FIELD_IS_BYREF |
        BLOCK_FIELD_IS_WEAK | BLOCK_BYREF_CALLER
};


// Function pointer accessors

static inline __typeof__(void (*)(void *, ...))
_Block_get_invoke_fn(struct Block_layout *block)
{
    return (void (*)(void *, ...))_Block_get_function_pointer(block->invoke);
}

static inline void 
_Block_set_invoke_fn(struct Block_layout *block, void (*fn)(void *, ...))
{
    _Block_set_function_pointer(block->invoke, fn);
}


static inline __typeof__(void (*)(void *, const void *))
_Block_get_copy_fn(struct Block_descriptor_2 *desc)
{
    return (void (*)(void *, const void *))_Block_get_function_pointer(desc->copy);
}

static inline void 
_Block_set_copy_fn(struct Block_descriptor_2 *desc,
                   void (*fn)(void *, const void *))
{
    _Block_set_function_pointer(desc->copy, fn);
}


static inline __typeof__(void (*)(const void *))
_Block_get_dispose_fn(struct Block_descriptor_2 *desc)
{
    return (void (*)(const void *))_Block_get_function_pointer(desc->dispose);
}

static inline void 
_Block_set_dispose_fn(struct Block_descriptor_2 *desc,
                      void (*fn)(const void *))
{
    _Block_set_function_pointer(desc->dispose, fn);
}


// Other support functions


// runtime entry to get total size of a closure
BLOCK_EXPORT size_t Block_size(void *aBlock);

// indicates whether block was compiled with compiler that sets the ABI related metadata bits
BLOCK_EXPORT bool _Block_has_signature(void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);

// returns TRUE if return value of block is on the stack, FALSE otherwise
BLOCK_EXPORT bool _Block_use_stret(void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);

// Returns a string describing the block's parameter and return types.
// The encoding scheme is the same as Objective-C @encode.
// Returns NULL for blocks compiled with some compilers.
BLOCK_EXPORT const char * _Block_signature(void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);

// Returns a string describing the block's GC layout.
// This uses the GC skip/scan encoding.
// May return NULL.
BLOCK_EXPORT const char * _Block_layout(void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);

// Returns a string describing the block's layout.
// This uses the "extended layout" form described above.
// May return NULL.
BLOCK_EXPORT const char * _Block_extended_layout(void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_8, __IPHONE_7_0);

// Callable only from the ARR weak subsystem while in exclusion zone
BLOCK_EXPORT bool _Block_tryRetain(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);

// Callable only from the ARR weak subsystem while in exclusion zone
BLOCK_EXPORT bool _Block_isDeallocating(const void *aBlock)
    __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);


// the raw data space for runtime classes for blocks
// class+meta used for stack, malloc, and collectable based blocks
BLOCK_EXPORT void * _NSConcreteMallocBlock[32]
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
BLOCK_EXPORT void * _NSConcreteAutoBlock[32]
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
BLOCK_EXPORT void * _NSConcreteFinalizingBlock[32]
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
BLOCK_EXPORT void * _NSConcreteWeakBlockVariable[32]
    __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
// declared in Block.h
// BLOCK_EXPORT void * _NSConcreteGlobalBlock[32];
// BLOCK_EXPORT void * _NSConcreteStackBlock[32];


struct Block_callbacks_RR {
    size_t  size;                   // size == sizeof(struct Block_callbacks_RR)
    void  (*retain)(const void *);
    void  (*release)(const void *);
    void  (*destructInstance)(const void *);
};
typedef struct Block_callbacks_RR Block_callbacks_RR;

BLOCK_EXPORT void _Block_use_RR2(const Block_callbacks_RR *callbacks);


#endif
```

# 附录：runtime.cpp

```c++
/*
 * runtime.cpp
 * libclosure
 *
 * Copyright (c) 2008-2010 Apple Inc. All rights reserved.
 *
 * @APPLE_LLVM_LICENSE_HEADER@
 */


#include "Block_private.h"
#include <platform/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <os/assumes.h>
#ifndef os_assumes
#define os_assumes(_x) os_assumes(_x)
#endif
#ifndef os_assert
#define os_assert(_x) os_assert(_x)
#endif

#define memmove _platform_memmove

#if TARGET_OS_WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
static __inline bool OSAtomicCompareAndSwapLong(long oldl, long newl, long volatile *dst) 
{ 
    // fixme barrier is overkill -- see objc-os.h
    long original = InterlockedCompareExchange(dst, newl, oldl);
    return (original == oldl);
}

static __inline bool OSAtomicCompareAndSwapInt(int oldi, int newi, int volatile *dst) 
{ 
    // fixme barrier is overkill -- see objc-os.h
    int original = InterlockedCompareExchange(dst, newi, oldi);
    return (original == oldi);
}
#else
#define OSAtomicCompareAndSwapLong(_Old, _New, _Ptr) __sync_bool_compare_and_swap(_Ptr, _Old, _New)
#define OSAtomicCompareAndSwapInt(_Old, _New, _Ptr) __sync_bool_compare_and_swap(_Ptr, _Old, _New)
#endif


/*******************************************************************************
Internal Utilities
********************************************************************************/

static int32_t latching_incr_int(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return BLOCK_REFCOUNT_MASK;
        }
        if (OSAtomicCompareAndSwapInt(old_value, old_value+2, where)) {
            return old_value+2;
        }
    }
}

static bool latching_incr_int_not_deallocating(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if (old_value & BLOCK_DEALLOCATING) {
            // if deallocating we can't do this
            return false;
        }
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            // if latched, we're leaking this block, and we succeed
            return true;
        }
        if (OSAtomicCompareAndSwapInt(old_value, old_value+2, where)) {
            // otherwise, we must store a new retained value without the deallocating bit set
            return true;
        }
    }
}


// return should_deallocate?
static bool latching_decr_int_should_deallocate(volatile int32_t *where) {
    while (1) {
        int32_t old_value = *where;
        if ((old_value & BLOCK_REFCOUNT_MASK) == BLOCK_REFCOUNT_MASK) {
            return false; // latched high
        }
        if ((old_value & BLOCK_REFCOUNT_MASK) == 0) {
            return false;   // underflow, latch low
        }
        int32_t new_value = old_value - 2;
        bool result = false;
        if ((old_value & (BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING)) == 2) {
            new_value = old_value - 1;
            result = true;
        }
        if (OSAtomicCompareAndSwapInt(old_value, new_value, where)) {
            return result;
        }
    }
}


/**************************************************************************
Framework callback functions and their default implementations.
***************************************************************************/
#if !TARGET_OS_WIN32
#pragma mark Framework Callback Routines
#endif

static void _Block_retain_object_default(const void *ptr __unused) { }

static void _Block_release_object_default(const void *ptr __unused) { }

static void _Block_destructInstance_default(const void *aBlock __unused) {}

static void (*_Block_retain_object)(const void *ptr) = _Block_retain_object_default;
static void (*_Block_release_object)(const void *ptr) = _Block_release_object_default;
static void (*_Block_destructInstance) (const void *aBlock) = _Block_destructInstance_default;


/**************************************************************************
Callback registration from ObjC runtime and CoreFoundation
***************************************************************************/

void _Block_use_RR2(const Block_callbacks_RR *callbacks) {
    _Block_retain_object = callbacks->retain;
    _Block_release_object = callbacks->release;
    _Block_destructInstance = callbacks->destructInstance;
}

/****************************************************************************
Accessors for block descriptor fields
*****************************************************************************/
#if 0
static struct Block_descriptor_1 * _Block_descriptor_1(struct Block_layout *aBlock)
{
    return aBlock->descriptor;
}
#endif

static struct Block_descriptor_2 * _Block_descriptor_2(struct Block_layout *aBlock)
{
    if (! (aBlock->flags & BLOCK_HAS_COPY_DISPOSE)) return NULL;
    uint8_t *desc = (uint8_t *)aBlock->descriptor;
    desc += sizeof(struct Block_descriptor_1);
    return (struct Block_descriptor_2 *)desc;
}

static struct Block_descriptor_3 * _Block_descriptor_3(struct Block_layout *aBlock)
{
    if (! (aBlock->flags & BLOCK_HAS_SIGNATURE)) return NULL;
    uint8_t *desc = (uint8_t *)aBlock->descriptor;
    desc += sizeof(struct Block_descriptor_1);
    if (aBlock->flags & BLOCK_HAS_COPY_DISPOSE) {
        desc += sizeof(struct Block_descriptor_2);
    }
    return (struct Block_descriptor_3 *)desc;
}

static void _Block_call_copy_helper(void *result, struct Block_layout *aBlock)
{
    struct Block_descriptor_2 *desc = _Block_descriptor_2(aBlock);
    if (!desc) return;

    (*desc->copy)(result, aBlock); // do fixup
}

static void _Block_call_dispose_helper(struct Block_layout *aBlock)
{
    struct Block_descriptor_2 *desc = _Block_descriptor_2(aBlock);
    if (!desc) return;

    (*desc->dispose)(aBlock);
}

/*******************************************************************************
Internal Support routines for copying
********************************************************************************/

#if !TARGET_OS_WIN32
#pragma mark Copy/Release support
#endif

// Copy, or bump refcount, of a block.  If really copying, call the copy helper if present.
void *_Block_copy(const void *arg) {
    struct Block_layout *aBlock;

    if (!arg) return NULL;
    
    // The following would be better done as a switch statement
    aBlock = (struct Block_layout *)arg;
    if (aBlock->flags & BLOCK_NEEDS_FREE) {
        // latches on high
        latching_incr_int(&aBlock->flags);
        return aBlock;
    }
    else if (aBlock->flags & BLOCK_IS_GLOBAL) {
        return aBlock;
    }
    else {
        // Its a stack block.  Make a copy.
        struct Block_layout *result =
            (struct Block_layout *)malloc(aBlock->descriptor->size);
        if (!result) return NULL;
        memmove(result, aBlock, aBlock->descriptor->size); // bitcopy first
#if __has_feature(ptrauth_calls)
        // Resign the invoke pointer as it uses address authentication.
        result->invoke = aBlock->invoke;
#endif
        // reset refcount
        result->flags &= ~(BLOCK_REFCOUNT_MASK|BLOCK_DEALLOCATING);    // XXX not needed
        result->flags |= BLOCK_NEEDS_FREE | 2;  // logical refcount 1
        _Block_call_copy_helper(result, aBlock);
        // Set isa last so memory analysis tools see a fully-initialized object.
        result->isa = _NSConcreteMallocBlock;
        return result;
    }
}


// Runtime entry points for maintaining the sharing knowledge of byref data blocks.

// A closure has been copied and its fixup routine is asking us to fix up the reference to the shared byref data
// Closures that aren't copied must still work, so everyone always accesses variables after dereferencing the forwarding ptr.
// We ask if the byref pointer that we know about has already been copied to the heap, and if so, increment and return it.
// Otherwise we need to copy it and update the stack forwarding pointer
static struct Block_byref *_Block_byref_copy(const void *arg) {
    struct Block_byref *src = (struct Block_byref *)arg;

    if ((src->forwarding->flags & BLOCK_REFCOUNT_MASK) == 0) {
        // src points to stack
        struct Block_byref *copy = (struct Block_byref *)malloc(src->size);
        copy->isa = NULL;
        // byref value 4 is logical refcount of 2: one for caller, one for stack
        copy->flags = src->flags | BLOCK_BYREF_NEEDS_FREE | 4;
        copy->forwarding = copy; // patch heap copy to point to itself
        src->forwarding = copy;  // patch stack to point to heap copy
        copy->size = src->size;

        if (src->flags & BLOCK_BYREF_HAS_COPY_DISPOSE) {
            // Trust copy helper to copy everything of interest
            // If more than one field shows up in a byref block this is wrong XXX
            struct Block_byref_2 *src2 = (struct Block_byref_2 *)(src+1);
            struct Block_byref_2 *copy2 = (struct Block_byref_2 *)(copy+1);
            copy2->byref_keep = src2->byref_keep;
            copy2->byref_destroy = src2->byref_destroy;

            if (src->flags & BLOCK_BYREF_LAYOUT_EXTENDED) {
                struct Block_byref_3 *src3 = (struct Block_byref_3 *)(src2+1);
                struct Block_byref_3 *copy3 = (struct Block_byref_3*)(copy2+1);
                copy3->layout = src3->layout;
            }

            (*src2->byref_keep)(copy, src);
        }
        else {
            // Bitwise copy.
            // This copy includes Block_byref_3, if any.
            memmove(copy+1, src+1, src->size - sizeof(*src));
        }
    }
    // already copied to heap
    else if ((src->forwarding->flags & BLOCK_BYREF_NEEDS_FREE) == BLOCK_BYREF_NEEDS_FREE) {
        latching_incr_int(&src->forwarding->flags);
    }
    
    return src->forwarding;
}

static void _Block_byref_release(const void *arg) {
    struct Block_byref *byref = (struct Block_byref *)arg;

    // dereference the forwarding pointer since the compiler isn't doing this anymore (ever?)
    byref = byref->forwarding;
    
    if (byref->flags & BLOCK_BYREF_NEEDS_FREE) {
        int32_t refcount = byref->flags & BLOCK_REFCOUNT_MASK;
        os_assert(refcount);
        if (latching_decr_int_should_deallocate(&byref->flags)) {
            if (byref->flags & BLOCK_BYREF_HAS_COPY_DISPOSE) {
                struct Block_byref_2 *byref2 = (struct Block_byref_2 *)(byref+1);
                (*byref2->byref_destroy)(byref);
            }
            free(byref);
        }
    }
}


/************************************************************
 *
 * API supporting SPI
 * _Block_copy, _Block_release, and (old) _Block_destroy
 *
 ***********************************************************/

#if !TARGET_OS_WIN32
#pragma mark SPI/API
#endif


// API entry point to release a copied Block
void _Block_release(const void *arg) {
    struct Block_layout *aBlock = (struct Block_layout *)arg;
    if (!aBlock) return;
    if (aBlock->flags & BLOCK_IS_GLOBAL) return;
    if (! (aBlock->flags & BLOCK_NEEDS_FREE)) return;

    if (latching_decr_int_should_deallocate(&aBlock->flags)) {
        _Block_call_dispose_helper(aBlock);
        _Block_destructInstance(aBlock);
        free(aBlock);
    }
}

bool _Block_tryRetain(const void *arg) {
    struct Block_layout *aBlock = (struct Block_layout *)arg;
    return latching_incr_int_not_deallocating(&aBlock->flags);
}

bool _Block_isDeallocating(const void *arg) {
    struct Block_layout *aBlock = (struct Block_layout *)arg;
    return (aBlock->flags & BLOCK_DEALLOCATING) != 0;
}


/************************************************************
 *
 * SPI used by other layers
 *
 ***********************************************************/

size_t Block_size(void *aBlock) {
    return ((struct Block_layout *)aBlock)->descriptor->size;
}

bool _Block_use_stret(void *aBlock) {
    struct Block_layout *layout = (struct Block_layout *)aBlock;

    int requiredFlags = BLOCK_HAS_SIGNATURE | BLOCK_USE_STRET;
    return (layout->flags & requiredFlags) == requiredFlags;
}

// Checks for a valid signature, not merely the BLOCK_HAS_SIGNATURE bit.
bool _Block_has_signature(void *aBlock) {
    return _Block_signature(aBlock) ? true : false;
}

const char * _Block_signature(void *aBlock)
{
    struct Block_layout *layout = (struct Block_layout *)aBlock;
    struct Block_descriptor_3 *desc3 = _Block_descriptor_3(layout);
    if (!desc3) return NULL;

    return desc3->signature;
}

const char * _Block_layout(void *aBlock)
{
    // Don't return extended layout to callers expecting old GC layout
    struct Block_layout *layout = (struct Block_layout *)aBlock;
    if (layout->flags & BLOCK_HAS_EXTENDED_LAYOUT) return NULL;

    struct Block_descriptor_3 *desc3 = _Block_descriptor_3(layout);
    if (!desc3) return NULL;

    return desc3->layout;
}

const char * _Block_extended_layout(void *aBlock)
{
    // Don't return old GC layout to callers expecting extended layout
    struct Block_layout *layout = (struct Block_layout *)aBlock;
    if (! (layout->flags & BLOCK_HAS_EXTENDED_LAYOUT)) return NULL;

    struct Block_descriptor_3 *desc3 = _Block_descriptor_3(layout);
    if (!desc3) return NULL;

    // Return empty string (all non-object bytes) instead of NULL 
    // so callers can distinguish "empty layout" from "no layout".
    if (!desc3->layout) return "";
    else return desc3->layout;
}

#if !TARGET_OS_WIN32
#pragma mark Compiler SPI entry points
#endif

    
/*******************************************************

Entry points used by the compiler - the real API!


A Block can reference four different kinds of things that require help when the Block is copied to the heap.
1) C++ stack based objects
2) References to Objective-C objects
3) Other Blocks
4) __block variables

In these cases helper functions are synthesized by the compiler for use in Block_copy and Block_release, called the copy and dispose helpers.  The copy helper emits a call to the C++ const copy constructor for C++ stack based objects and for the rest calls into the runtime support function _Block_object_assign.  The dispose helper has a call to the C++ destructor for case 1 and a call into _Block_object_dispose for the rest.

The flags parameter of _Block_object_assign and _Block_object_dispose is set to
    * BLOCK_FIELD_IS_OBJECT (3), for the case of an Objective-C Object,
    * BLOCK_FIELD_IS_BLOCK (7), for the case of another Block, and
    * BLOCK_FIELD_IS_BYREF (8), for the case of a __block variable.
If the __block variable is marked weak the compiler also or's in BLOCK_FIELD_IS_WEAK (16)

So the Block copy/dispose helpers should only ever generate the four flag values of 3, 7, 8, and 24.

When  a __block variable is either a C++ object, an Objective-C object, or another Block then the compiler also generates copy/dispose helper functions.  Similarly to the Block copy helper, the "__block" copy helper (formerly and still a.k.a. "byref" copy helper) will do a C++ copy constructor (not a const one though!) and the dispose helper will do the destructor.  And similarly the helpers will call into the same two support functions with the same values for objects and Blocks with the additional BLOCK_BYREF_CALLER (128) bit of information supplied.

So the __block copy/dispose helpers will generate flag values of 3 or 7 for objects and Blocks respectively, with BLOCK_FIELD_IS_WEAK (16) or'ed as appropriate and always 128 or'd in, for the following set of possibilities:
    __block id                   128+3       (0x83)
    __block (^Block)             128+7       (0x87)
    __weak __block id            128+3+16    (0x93)
    __weak __block (^Block)      128+7+16    (0x97)
        

********************************************************/

//
// When Blocks or Block_byrefs hold objects then their copy routine helpers use this entry point
// to do the assignment.
//
void _Block_object_assign(void *destArg, const void *object, const int flags) {
    const void **dest = (const void **)destArg;
    switch (os_assumes(flags & BLOCK_ALL_COPY_DISPOSE_FLAGS)) {
      case BLOCK_FIELD_IS_OBJECT:
        /*******
        id object = ...;
        [^{ object; } copy];
        ********/

        _Block_retain_object(object);
        *dest = object;
        break;

      case BLOCK_FIELD_IS_BLOCK:
        /*******
        void (^object)(void) = ...;
        [^{ object; } copy];
        ********/

        *dest = _Block_copy(object);
        break;
    
      case BLOCK_FIELD_IS_BYREF | BLOCK_FIELD_IS_WEAK:
      case BLOCK_FIELD_IS_BYREF:
        /*******
         // copy the onstack __block container to the heap
         // Note this __weak is old GC-weak/MRC-unretained.
         // ARC-style __weak is handled by the copy helper directly.
         __block ... x;
         __weak __block ... x;
         [^{ x; } copy];
         ********/

        *dest = _Block_byref_copy(object);
        break;
        
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK:
        /*******
         // copy the actual field held in the __block container
         // Note this is MRC unretained __block only. 
         // ARC retained __block is handled by the copy helper directly.
         __block id object;
         __block void (^object)(void);
         [^{ object; } copy];
         ********/

        *dest = object;
        break;

      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_WEAK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK  | BLOCK_FIELD_IS_WEAK:
        /*******
         // copy the actual field held in the __block container
         // Note this __weak is old GC-weak/MRC-unretained.
         // ARC-style __weak is handled by the copy helper directly.
         __weak __block id object;
         __weak __block void (^object)(void);
         [^{ object; } copy];
         ********/

        *dest = object;
        break;

      default:
        break;
    }
}

// When Blocks or Block_byrefs hold objects their destroy helper routines call this entry point
// to help dispose of the contents
void _Block_object_dispose(const void *object, const int flags) {
    switch (os_assumes(flags & BLOCK_ALL_COPY_DISPOSE_FLAGS)) {
      case BLOCK_FIELD_IS_BYREF | BLOCK_FIELD_IS_WEAK:
      case BLOCK_FIELD_IS_BYREF:
        // get rid of the __block data structure held in a Block
        _Block_byref_release(object);
        break;
      case BLOCK_FIELD_IS_BLOCK:
        _Block_release(object);
        break;
      case BLOCK_FIELD_IS_OBJECT:
        _Block_release_object(object);
        break;
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_OBJECT | BLOCK_FIELD_IS_WEAK:
      case BLOCK_BYREF_CALLER | BLOCK_FIELD_IS_BLOCK  | BLOCK_FIELD_IS_WEAK:
        break;
      default:
        break;
    }
}


// Workaround for <rdar://26015603> dylib with no __DATA segment fails to rebase
__attribute__((used))
static int let_there_be_data = 42;
```



