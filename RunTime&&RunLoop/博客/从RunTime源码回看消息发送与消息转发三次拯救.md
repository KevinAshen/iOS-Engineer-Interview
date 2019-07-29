[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 正如前面文章讲到《高级编程》中的源码解析大多伪代码，且大多已经落后于当前版本，因此在研读了RunTime源码后再来研究《高级编程》中的ARC里提到的操作
- 本文只探讨RunTime中的实现部分，RunLoop中的部分暂不深入

# 准备工作

- 请准备好750.1版本的objc4源码一份【目前最新的版本】，打开它，找到文章中提到的方法，类型，对象
- 一切请以手中源码为准，不要轻信任何人，任何文章，包括本篇博客
- 文章中的源码都请过了我的删改，建议还是先看看源码
- 源码建议从Apple官方开源网站获取[obj4](https://opensource.apple.com/tarballs/objc4/objc4-750.1.tar.gz)
- 官网上下载下来需要自己配置才能编译运行，如果不想配置，可以在[RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)中clone

# method_t

- 我们知道方法是存储在类中的bits指向的class_rw_t中的methods里，而methods是一个二维数组，里面真正存储方法信息的是method_t结构体

```objective-c
///class_rw_t
// 方法信息
method_array_t methods;

///method_array_t
list_array_tt<method_t, method_list_t>
  
///method_t
struct method_t {
    SEL name;
    const char *types;	//方法标签
    MethodListIMP imp;
};
```

- 也就是说每一个方法在method_t里要存放三个东西，sel方法签名，方法类型，IMP函数指针【这三个宝贝不同的文章有不同的叫法，本文统一为上面👆三个叫法】

# SEL name

- Objective-C 在编译的时候, 会根据方法的名字(包括参数序列),生成一个用 来区分这个方法的唯一的一个 ID,这个 ID 就是 SEL 类型的。我们需要注意的是,只要方法的名字(包括参数序列)相同,那么它们的 ID 都是相同的。就是 说,不管是超类还是子类,不管是有没有超类和子类的关系,只要名字相同 那么 ID 就是一样的
```objective-c
///Person.m
#import "Person.h"
@implementation Person

- (void)eat {
    NSLog(@"Person EAT");
}

- (void)eat:(NSString *)str {
    NSLog(@"Person EATSTR");
}

- (void)dayin {
    NSLog(@"dayin");
    SEL sell1 = @selector(eat:);
    NSLog(@"sell1:%p", sell1);
    SEL sell2 = @selector(eat);
    NSLog(@"sell2:%p", sell2);
}

@end

///main.m
#import <Foundation/Foundation.h>
#include "Student.h"
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        Person *newPerson = [[Person alloc] init];
        [newPerson dayin];
    }
    return 0;
}

///结果
//dayin
//sell1:0x100000f63
//sell2:0x100000f68
```
- 这里要注意，@selector等同于是把方法名翻译成sel方法签名，它只关心方法名和参数个数，在dayin方法里，可以在eat后加上任意个:都有效果
- 同时，这个生成sel的过程是固定的，因为它只是一个表明方法的ID，不管是在任何类里去写这个dayin方法，还是运行好几次，这个sel都是固定这个【eat就是0x100000f68，eat:就是0x100000f63】
- 经过测试，在不同的项目里sel的值是不一样的，但在同一个项目里是绝对一样的
- 当然，这里我们可以直观的看出，sel不关心返回值与参数的数据类型，你在生成sel的时候都没有把这些东西放上去，它们当然不会生成不同的sel
- 这也是为什么我们不能在一个类里面写两个同名方法【哪怕参数的类型不一样，返回值不一样】，因为存放的时候，会把两个方法看作一个
- 不同的类可以拥有相同的方法，因为sel的存储是存在这个类的methods里的
- 本质上，SEL只是一个指向方法的指针（准确的说，只是一个根据方法名hash化了的KEY值，能唯一代表一个方法），它的存在只是为了加快方法的查询速度。
- 虽然 SEL 定义成 char *, 我们可 以把它想象成 int. 每个方法的名字对应一个唯一的 int 值

![28653839_1361282379yvAk](http://ww2.sinaimg.cn/large/006tNc79ly1g5h0h8zl0dj30hl09fjs5.jpg)

### const char *types
- 为了协助运行时系统，编译器用字符串为每个方法的返回值和参数类型和方法选择器编码。使用的编码方案在其他情况下也很有用，所以它是public 的，可用于@encode() 编译器指令。当给定一个类型参数，返回一个编码类型字符串。类型可以是一个基本类型如int，指针，结构或联合标记，或任何类型的类名，事实上，都可以作为C sizeof() 运算符的参数。这个机制也是为了提高Runtime的效率.
- 编码翻译表：

![QQ20190729-194442](http://ww2.sinaimg.cn/large/006tNc79ly1g5gy5kxyjlj30hs0o30uk.jpg)

- 可以使用@encode进行验证

```objective-c
char *buf1 = @encode(int **);
char *buf2 = @encode(struct key);
char *buf3 = @encode(Rectangle);
```

- 也就是说，为了提高调取方法的速度，runtime会把方法的参数转换成字符串形式的方法类型进行存储

# MethodListIMP imp

- imp是函数的地址，我们知道runtime实现了OC的底层实现，实际上就是OC在编译运行的时候，底层都是通过C++运行的，包括方法的调用

## 方法调用的大致流程【消息发送】

- 消息转发基本是最底层的一块的部分，其中的具体实现涉及到的都是汇编语言，已经超出笔者的作用域了，因此我们只能大概分析下流程

```objective-c
- (void)test:(NSInteger)num {
    NSLog(@"You know nothing about power！");
}

[person test:12];

//这短短一句话传递了，三个信息
//调用者：person
//方法名：test:
//参数：12
//这样一句话就会被转换为objc_msgSend(self, @selector(test:), 12);
```

- 我们无法看到objc_msgSend的具体实现，但接下来发生的事情就是根据sel去self的isa指向的类对象里去找该方法【查找过程可以看我前面讲isa的文章】
- 匹配到相应的sel后，我们集齐了sel，imp，obj【参数】，receiver【调用者】，就可以去执行该方法了

## imp具体作用

- imp就是函数的地址，这里可以直接用C语言的方式来理解，这就相当于某个函数的入口位置，我们调用方法【执行对应函数】其实就是按部就班的执行该C++函数
- 我们取得了函数指针之后,也就意味着我们取得了执行的时候的这段方法的代码的入口,这样我 们就可以像普通的 C 语言函数调用一样使用这个函数指针。当然我们可以把函数指针作为参数 传递到其他的方法,或者实例变量里面,从而获得极大的动态性。我们获得了动态性,但是付出 的代价就是编译器不知道我们要执行哪一个方法所以在编译的时候不会替我们找出错误,我们只 有执行的时候才知道,我们写的函数指针是否是正确的。所以,在使用函数指针的时候要非常准 确地把握能够出现的所有可能,并且做出预防。尤其是当你在写一个供他人调用的接口 API 的 时候,这一点非常重要。
- 我们同样进行一些打印试验

```objective-c
- (void)dayin {
    NSLog(@"dayin");
    SEL sell1 = @selector(eat);
    NSLog(@"sell1:%p", sell1);
    IMP imp1 = [self methodForSelector:sell1];	//根据sel获取imp
    NSLog(@"imp1:%p", imp1);
    SEL sell2 = @selector(eat:);
    NSLog(@"sell2:%p", sell2);
    IMP imp2 = [self methodForSelector:sell2];
    NSLog(@"imp2:%p", imp2);
    SEL sell3 = @selector(drink:);
    NSLog(@"sell3:%p", sell3);
    IMP imp3 = [self methodForSelector:sell3];
    NSLog(@"imp3:%p", imp3);
  
    imp1();//可以像这样直接调用imp函数，如果要加参数的话，直接在methodForSelector中加
}

//sell1:0x100000f51
//imp1:0x100000be0
//sell2:0x100000f68
//imp2:0x7fff7c89c300
//sell3:0x100000f6d
//imp3:0x7fff7c89c300
```

- 其中eat，eat:，drink:只有eat方法是我有写实现，也只有这个方法的imp是确实存在的【imp1:0x100000be0】，而剩下两个的imp都指向同一个地址【0x7fff7c89c300】，这些数据和sel一样，不管运行几次都是一样的
- 因此与sel这样只是生成一个特殊ID不一样，imp是必须要该方法存在【函数存在】才能生成的【也就是方法必须有实现】