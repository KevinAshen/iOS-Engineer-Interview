[TOC]
# 关于我的仓库

- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文主要涉及消息转发的三个方法的使用方式，通过打印试验验证消息查找链问题
- 翻译了部分Apple文档，以说明方法签名的作用
- 对消息转发的意义做了探讨

# 准备工作

- 请准备好750.1版本的objc4源码一份【目前最新的版本】，打开它，找到文章中提到的方法，类型，对象
- 一切请以手中源码为准，不要轻信任何人，任何文章，包括本篇博客
- 文章中的源码都请过了我的删改，建议还是先看看源码
- 源码建议从Apple官方开源网站获取[obj4](https://opensource.apple.com/tarballs/objc4/objc4-750.1.tar.gz)
- 官网上下载下来需要自己配置才能编译运行，如果不想配置，可以在[RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)中clone

# 一个说法

- 我们先可以对消息转发来个概述：当对象调用了一个它没有的方法时【父类也找不到实现】，会出现crash；消息转发简单来说就是处理调用对象没有的方法时的处理【这里的对象也指类对象】
- 因此，消息转发常常被人称为防止程序crash的措施
- 文章最后会探讨这种说法是否正确

# 关于消息转发

- 我们已经学习到，OC的底层都是C++实现的，而方法的调用，实际上是消息的发送
- 比如对象A调用方法B，我们就会查找A中的方法列表，寻找方法B对应的函数执行
- 这里用到的就是objc_msgSend
- 而消息转发派上用场的情况，就是在把当前类，父类，乃至到根类全都找完了，也没有找到方法B时进行的措施

## 三道防线

- Apple给消息转发提供了三个方法，三个都可以实现上面说的这件事
- 分别是动态方法解析，备用接收者，完整转发
- 并且这三个方法的关系是递进的，并且这三个方法是递进的，前一个实现了就不会走后一个，同时越往后处理的代价越大

## 关系图

![E81306B3-FD5F-4B68-B7DF-7FA084A20E62](http://ww1.sinaimg.cn/large/006tNc79ly1g5ho2v126rj30gl093t95.jpg)

- 下面开始详解这三个方法

# 动态方法解析【resolveInstanceMethod】

## 作用
- 当发现sel在class里的方法列表里对不上号的时候，会给类的方法列表添加该方法
- 需要自己实现方法对应的imp

## runtime源码

```objective-c
+ (BOOL)resolveInstanceMethod:(SEL)sel {
    return NO;
}
```

- 不重写的话，默认返回NO，不走该方法

## 实际使用

- 这里虽然我们要试验消息转发，但如果你直接让一个对象去调用不存在的方法，编译器肯定会给你报错，不让你编译的
- 因此为了试验，调用方法会在.h里面写申明，但是不会写实现，先骗过编译器再说
- 为什么有这个转发，实际上还是因为OC是动态语言，方法真正的调用要到运行时才会查找
- 先尝试直接返回YES，不做任何处理会发生什么

### 直接返回YES
```objective-c
///Person.m
#import "Person.h"
@implementation Person
+ (BOOL)resolveInstanceMethod:(SEL)sel {
    return YES;
}
@end

///main.m
#import <Foundation/Foundation.h>
#import "Student.h"
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        Student *student = [[Student alloc] init];
        [student eat:@"123"];
    }
    return 0;
}

//-[Person eat:]: unrecognized selector sent to instance 0x100700160
```
- 结果依然crash，找不到该sel对应的imp

### 正常调用
```objective-c
///Person.m
#import "Person.h"
#import <objc/runtime.h>
@implementation Person
+ (BOOL)resolveInstanceMethod:(SEL)sel
{
    if (sel == @selector(eat:)) {
        class_addMethod(self, sel, (IMP)Ceat, "v@:I");
        return YES;
    }
    return [super resolveInstanceMethod:sel];
}
void Ceat(id self, SEL cmd, NSInteger num) {
    NSLog(@"void Ceat(id self, SEL cmd, int num):");
    NSLog(@"%@, %s, %ld", [self class], sel_getName(cmd), num);
}

@end

///main.m
#import <Foundation/Foundation.h>
#import "Student.h"
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        Student *student = [[Student alloc] init];
        [student eat:@"123"];
    }
    return 0;
}

// messageTransmit[1221:273136] void Ceat(id self, SEL cmd, int num):
//messageTransmit[1221:273136] Person, eat:, 123
```
- 需要导入runtime头文件以使用某些方法
- "v@:I"中对应关系为
  - v : void
  - @ : eat 
  - : : sel
  - I : NSInteger
- 如果不理解这个类型编码，请看我上篇文章

### 加大难度
- 假如我们建一个student类继承于Person，给student也申明个不存在的eat:方法，调用一波
- 结果依然会走Ceat函数
- 这是为什么呢？因为继承本来就会走父类里实现的方法😂

### 整理波思路
- 所以这个方法调用的流程除了之前整理的cache缓存，method方法列表，之后又多了转发这一环节
- 转发一定是在上面这些流程完了之后才会进入的
- 再加上继承父类这一层，太神秘了呀

# 备用接受者【(id)forwardingTargetForSelector:(SEL)aSelector】

## 作用

- 指定某个类去代替执行该方法
- 这个类被称为备用接受者【它得有这个方法】

## runtime源码

```objective-c
- (id)forwardingTargetForSelector:(SEL)sel {
    return nil;
}
```

- 同样默认是不打开，直接返回nil

## 正常使用

-  新建Teacher类，同样继承于Person

```objective-c
- (id)forwardingTargetForSelector:(SEL)aSelector {
    NSString *selStr = NSStringFromSelector(aSelector);
    
    if ([selStr isEqualToString:@"eat:"]) {
        return [[Student alloc] init];
    }
    
    return [super forwardingTargetForSelector:aSelector];
}

//Student:eat:(NSString *)str:123
```

- 这样的委托同样是遵循我们这个方法调用流程，也就是说，如果Person类里再转发，依然能转发出去

## 意义

- 这个特性很容易让人产生联想，毕竟这样的委托执行，很想子类无法执行，交给父类去执行
- 其实我们就可以通过该方法实现类似于多重继承的效果

# 完整转发【(void)forwardInvocation:(NSInvocation )anInvocation】

- 完整转发要比前两个复杂一些
- 因为其中涉及到了几个没有了解过的概念，这些概念网上也没有什么特别好的讲解，所以我主要是翻译下Apple官方文档里的解释
- 不会纠结它的底层时怎么实现的，但需要知道他们起了什么作用

## runtime源码

```objective-c
// Replaced by CF (returns an NSMethodSignature)
- (NSMethodSignature *)methodSignatureForSelector:(SEL)sel {
    _objc_fatal("-[NSObject methodSignatureForSelector:] "
                "not available without CoreFoundation");
}

- (void)forwardInvocation:(NSInvocation *)invocation {
    [self doesNotRecognizeSelector:(invocation ? [invocation selector] : 0)];
}

// Replaced by CF (throws an NSException)
- (void)doesNotRecognizeSelector:(SEL)sel {
    _objc_fatal("-[%s %s]: unrecognized selector sent to instance %p", 
                object_getClassName(self), sel_getName(sel), self);
}
```

- 这里注意，完整转发作为最后一项保护手段，forwardInvocation方法如果不被重写，就会执行doesNotRecognizeSelector，也就是抛出了异常

## 概念解释

### NSMethodSignature

#### Apple文档

A record of the type information for the return value and parameters of a method.

Use an `NSMethodSignature` object to forward messages that the receiving object does not respond to—most notably in the case of distributed objects. You typically create an `NSMethodSignature`object using the [`NSObject`](apple-reference-documentation://hcttKQiP8Q) [`methodSignatureForSelector:`](apple-reference-documentation://hcqqt_hq1l)instance method (in macOS 10.5 and later you can also use [`signatureWithObjCTypes:`](apple-reference-documentation://hc5Ht0yIa0)). It is then used to create an [`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) object, which is passed as the argument to a [`forwardInvocation:`](apple-reference-documentation://hcR2PGYTDL) message to send the invocation on to whatever other object can handle the message. In the default case, [`NSObject`](apple-reference-documentation://hcttKQiP8Q) invokes [`doesNotRecognizeSelector:`](apple-reference-documentation://hcUsWRgbKL), which raises an exception. For distributed objects, the [`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) object is encoded using the information in the `NSMethodSignature` object and sent to the real object represented by the receiver of the message.



方法的返回值以及参数的类型信息的记录

使用一个`NSMethodSignature`对象来转发那些接受者无法响应的消息，尤其是在`distributed`对象的情况下。通常使用[`methodSignatureForSelector:`](apple-reference-documentation://hcqqt_hq1l)方法来创建一个`NSMethodSignature`对象。该对象之后会被用作创建一个[`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) 对象。这个[`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) 对象回作为传给[`forwardInvocation:`](apple-reference-documentation://hcR2PGYTDL) 方法的参数。这样子那些无法响应的消息会被发给[`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) 对象，转发给任意能处理这个消息的对象。在默认情况下，对象是会直接调用[`doesNotRecognizeSelector:`](apple-reference-documentation://hcUsWRgbKL)方法抛出异常。对于`distributed`对象，[`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) 对象根据在`NSMethodSignature`对象中的信息被编码发给真正的执行对象代替消息的接受者【原对象】

#### 解释

- 这段实在有点抽象的
- 我的理解就是先将无法被响应的方法封装到`NSMethodSignature`对象，再根据`NSMethodSignature`对象去创建[`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) 对象，作为[`forwardInvocation:`](apple-reference-documentation://hcR2PGYTDL) 方法的参数，进行转发
- 也就是说[`NSInvocation`](apple-reference-documentation://hcjVj4h-wP) 就类似于一个中转站，进行各路转发

### NSInvocation
#### Apple文档

An Objective-C message rendered as an object.

呈现一个对象的Objective-C信息

`NSInvocation` objects are used to store and forward messages between objects and between applications, primarily by [`NSTimer`](apple-reference-documentation://hcaODPrPmP)objects and the distributed objects system. An `NSInvocation` object contains all the elements of an Objective-C message: a target, a selector, arguments, and the return value. Each of these elements can be set directly, and the return value is set automatically when the `NSInvocation` object is dispatched.

`NSInvocation` 对象时用于在对象以及APP之间转发信息而存在的，主要通过NSTimer对象和distributed objects system。一个`NSInvocation` 对象包含OC消息中的所有元素：a target, a selector, 参数, 和返回值。 每一部分都能被直接设置，并且在`NSInvocation` 对象被发送时自动设置

An `NSInvocation` object can be repeatedly dispatched to different targets; its arguments can be modified between dispatch for varying results; even its selector can be changed to another with the same method signature (argument and return types). This flexibility makes `NSInvocation` useful for repeating messages with many arguments and variations; rather than retyping a slightly different expression for each message, you modify the `NSInvocation` object as needed each time before dispatching it to a new target.

一个`NSInvocation` 对象可以被重复发送到不同的target，它的参数可以在为了改变结果而进行的发送之间被修改。甚至它的selector可以变成另一个有着相同的方法签名【参数以及返回值】。这种灵活性使得`NSInvocation` 对于重复发送有着许多参数和变化的消息大有益处。而不再需要为每一个消息的不同，而去修改的它的表达，你只需在每次发送给新的target前修改这个 `NSInvocation` 对象【有需要的话】

#### 解释

- 越来越抽象了😢
- `NSInvocation`里买呢会真正封装消息所需的所有内容，在转发时，会将整个`NSInvocation`对象发送给新的target

## 正常使用
 ```objective-c
// Person.m

//我们必须重写该方法 消息转发机制使用从这个方法中获取的信息来创建NSInvocation对象。因此我们必须重写这个方法，为给定的selector提供一个合适的方法签名。
- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector
{
     NSString *sel = NSStringFromSelector(aSelector);
    // 判断要转发的SEL
    if ([sel isEqualToString:@"sleep"]) {
        // 为转发的方法手动生成签名
        return [NSMethodSignature signatureWithObjCTypes:"v@:"];
        //那么NSMethodSignature又是什么？来看看
    }
    
    return [super methodSignatureForSelector:aSelector]; 
}

//NSObject的forwardInvocation:方法实现只是简单调用了doesNotRecognizeSelector:方法，它不会转发任何消息。这样，如果不在以上所述的三个步骤中处理未知消息，则会引发一个异常。
//转发消息
- (void)forwardInvocation:(NSInvocation *)anInvocation
{
    //拿到消息
    SEL selector = [anInvocation selector];
    // 新建需要转发消息的对象 转发消息
    Child *child = [[Child alloc] init];
    if ([child respondsToSelector:selector]) {
        // 转发 唤醒这个方法
        [anInvocation invokeWithTarget:child];
    } else {
        [super forwardInvocation:anInvocation];
    }
}
//从某种意义上来讲，forwardInvocation:就像一个未知消息的分发中心，将这些未知的消息转发给其它对象。或者也可以像一个运输站一样将所有未知消息都发送给同一个接收对象。这取决于具体的实现。
 ```

## 疑惑

- 这里比较迷人的就是为什么NSMethodSignature对象只需要方法签名就可以生成了，其余的参数又是什么时候添加到NSInvocation对象里去的

# 消息转发的意义

- 显然防止crash这一点站不住脚，或者说属于正确的废话。
- 上述三个方法都得建立在已经知道了那个方法无法执行，得在知道了其sel之后才能用，假如我已经知道哪个方法，我直接补上就行了，何必要转发呢？
- 因此我们使用消息转发更多还是想实现某些骚操作，或者说Black Magic！
- 这方面由于笔者钻研不太深，贴一篇感觉比较好的文章
- [iOS中消息转发机制及工程应用](https://blog.csdn.net/cewei711/article/details/53189620)