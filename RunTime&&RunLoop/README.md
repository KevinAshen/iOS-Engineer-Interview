[TOC]

# 前言

- 看完《高级编程》中的MRC&&ARC，开始研究RunTime源码，目标在于弄懂ARC中真正的实现，消息发送机制，类与对象的本质，weak底层实现
- 学的时候比看书的时候痛苦多了，看《高级编程》到底只要老老实实看书就行，可这玩意得网上各种找资料看
- 再加上很多人源代码的版本不统一，简直了，要开着几个版本的源码对照着看
- 写博客其实一开始也没想好写什么内容，是看着看着，大概理清楚思路后才明白应该总结那些内容，真的hard模式
- 看的时候常常让我回想到当初看《百年孤独》的感受，真就魔幻现实呗
- 尤其是什么类，对象，isa，不觉得像一句诗吗？
- runtime东西真的非常多，从任何一点看下去都能学到些东西，但怎么说呢，皓首穷经谁顶得住，所以在看完基本结构体后，我就直接开始看weak之类的特别的知识点，从这些东西出发，回过去看runtime，不再不管不顾的看了，实在太多
- 看了一些源码后觉得学习runtime，第一步还是先理解数据结构，也就是其中的容器，比如说AutoreleasePoolPage，sideTable，sideTables这类
- 先把数据结构搞定，再去了解里面方法调用栈是怎么样的，相应的可以画一个流程图，分析完每个方法做了什么，有什么分支，相信可以在学习过程中起到事半功倍的效果
# 类，对象，isa
## 难点
### isa闭环
- 里面又有元类，又有父类，要理清楚还挺麻烦的
- 另外还要明白所谓一切都是对象

### isa_t
- 这个主要比较麻烦，从定义到初始化过程，都要很细致的一点点找下去
- 特别是初始化和获取isa那一块，再次暴露我计算机基础之薄弱🌚

## 我的博客
- 本文主要研究RunTime基础，里面的基本结构体，类与对象的基本模型
- 尤其是其中的isa结构体，对其存储进行了探究
- 链接：[RunTime源码中的基本结构体【类，对象，isa】](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/RunTime&&RunLoop/%E5%8D%9A%E5%AE%A2/RunTime%E6%BA%90%E7%A0%81%E4%B8%AD%E7%9A%84%E5%9F%BA%E6%9C%AC%E7%BB%93%E6%9E%84%E4%BD%93%E3%80%90%E7%B1%BB%EF%BC%8C%E5%AF%B9%E8%B1%A1%EF%BC%8Cisa%E3%80%91.md)
# autoreleasepool
## 难点
### AutoreleasePoolPage

- page是存储调用autorelease方法的对象的地址的数据结构
- 这里面需要了解到page的组成，栈的实现方式
- 理解了其中存储对象地址的方式才能理解下面的push与pop
### push
- 对于push，要明白这是在@autoreleasepool开始时调用的方法，调用fast方法仅仅是吧哨兵对象POOL_BOUNDARY压入
- 真正把对象压入的是autorelease方法
### pop

- pop和push分不开，要明白哨兵对象POOL_BOUNDARY在栈里面起了什么作用
- 结合数据结构方面的理解，就能很好的理解这里面的流程

### token
- token这个很多博文讲的不清楚，含糊过去了，导致我一直对releaseuntil不太懂
- 直到遇到了小神

## 我的博客
- 本文主要研究autoreleasepool，探究其中的autoreleasepage结构，push，pop方法实现
- 链接：[从RunTime源码回看autoreleasepool](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/RunTime&&RunLoop/%E5%8D%9A%E5%AE%A2/%E4%BB%8ERunTime%E6%BA%90%E7%A0%81%E5%9B%9E%E7%9C%8Bautoreleasepool.md)
# weak底层实现
## 难点
- weak实在是太麻烦了，里面涉及的C++的知识和前面几个根本不是一个level的，还有一些操作系统的知识，作为一个计算机小白能有什么办法，只能一点一点的补，一点一点的学，太累了
- weak主要难在数据模型太复杂了，要先捋清数据模型再去学习里面的方法，可能比较好
## 我的博客
- 本文主要研究weak的底层实现，先研究其中涉及到的SideTable里的数据模型，之后探究weak中相关的方法
- 链接：[从RunTime源码回看weak底层实现](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/RunTime&&RunLoop/%E5%8D%9A%E5%AE%A2/%E4%BB%8ERunTime%E6%BA%90%E7%A0%81%E5%9B%9E%E7%9C%8Bweak%E5%BA%95%E5%B1%82%E5%AE%9E%E7%8E%B0.md)

# 消息发送及相关知识
## 难点
- 个人认为这里比较奇怪的只有类型编码这一块，SEL，IMP这些其实挺好理解的
- 这一块主要是为了下面学习消息转发打基础，学一些基础知识

## 我的博客
- 本文为消息转发的先导文章，涉及sel，imp的作用，消息发送大概机制
- 链接：[从RunTime源码回看消息发送及其基础知识.md](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/RunTime&&RunLoop/博客/从RunTime源码回看消息发送及其基础知识.md)

# 消息转发机制

## 难点
- 第一第二种方法都很好理解，无非就是动态添加方法或者让其他类去执行该方法
- 第三种涉及到了NSMethodSignature以及NSInvocation这两个概念，需要认真理解下
- 消息转发另外比较重要的就是如何去理解它

## 我的博客
- 本文为消息转发的先导文章，涉及sel，imp的作用，消息发送大概机制
- 链接：[从RunTime源码回看消息转发机制【三次拯救】.md](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/RunTime&&RunLoop/%E5%8D%9A%E5%AE%A2/%E4%BB%8ERunTime%E6%BA%90%E7%A0%81%E5%9B%9E%E7%9C%8B%E6%B6%88%E6%81%AF%E8%BD%AC%E5%8F%91%E6%9C%BA%E5%88%B6%E3%80%90%E4%B8%89%E6%AC%A1%E6%8B%AF%E6%95%91%E3%80%91.md)

# 正确打开【学习】姿势
## 类，对象，isa
- 这部分虽然零零碎碎看了很多文章，真正特别有用的是这几篇
- 首先一定要看下[iOS Runtime系列一、objc_class深深的误解](https://www.jianshu.com/p/1e16e3b745c1)，会让你找资料的时候远离那些上古文章
- 对象类基础一块把[Runtime源码 —— 对象、类和isa](https://www.jianshu.com/p/a8eade8a1c6d)认真看下；此外最后的打印试验建议也跟着做一遍，会提高感触
- isa部分[从 NSObject 的初始化了解 isa](https://github.com/Draveness/iOS-Source-Code-Analyze/blob/master/contents/objc/从%20NSObject%20的初始化了解%20isa.md)是真的强，一定要精读，作者已经是我偶像了，太强了
- runtime概念[探秘Runtime - Runtime介绍](https://www.jianshu.com/p/ce97c66027cd)
- 类的bit部分[Class的结构](https://www.jianshu.com/p/fd7aad6297f0)
- 最后查漏补缺[探秘Runtime - 剖析Runtime结构体](https://www.jianshu.com/p/5b7e7c8075ef)

## autoreleasepool

- [OC源码 —— autoreleasepool](https://www.jianshu.com/p/ca7835b41699)H神这篇比较深入，把线程里的一些操作也掰开来讲了，对于想要深入的同学很值得研究
- [自动释放池的前世今生](https://github.com/draveness/analyze/blob/master/contents/objc/%E8%87%AA%E5%8A%A8%E9%87%8A%E6%94%BE%E6%B1%A0%E7%9A%84%E5%89%8D%E4%B8%96%E4%BB%8A%E7%94%9F.md)D神这篇图画的无比清晰，看了以后能彻底get到page的数据结构，美滋滋；结合图，能了解清楚哨兵对象POOL_BOUNDARY的作用
- [AutoreleasePool 的实现机制 (一)](https://www.jianshu.com/p/803960be736d)和[AutoreleasePool 的实现机制 (二)](https://www.jianshu.com/p/b0b739e064ad)以及[AutoreleasePool 的实现机制 (三)](https://www.jianshu.com/p/2cda49470fa4)最后[AutoreleasePool 的实现机制 (四)](https://www.jianshu.com/p/f658aae5ac3f)；天空霹雳一声响，小神现世，分析autoreleasepool的这四篇博文堪称妙绝，对于token这个点讲的很清楚，底层实现分析的更是深入，强无敌！

## weak底层实现

- 这里隆重介绍下I神，无敌，如果说我的博客对于sideTables，sideTable的分析有那么一点点清晰的话，毋庸置疑，这都来自于I神的教导
- [iOS管理对象内存的数据结构以及操作算法--SideTables、RefcountMap、weak_table_t-一](https://www.jianshu.com/p/ef6d9bf8fe59)以及[iOS管理对象内存的数据结构以及操作算法--SideTables、RefcountMap、weak_table_t-二](https://www.jianshu.com/p/8577286af88e)这两篇博文从标题上就显得如此高大上，充满了不俗的气质，里面画的图，大学宿舍楼的比喻，都绝了简直
- 小神对于weak的研究也是不凡[runtime(二) SideTables](https://www.jianshu.com/p/ea4c176ffb2b)中对于DenseMap的分析，真正让我看懂了墓碑桶等等的过程
- [runtime(三) weak_table_t](https://www.jianshu.com/p/7eb4d291d6d6)
- 如果对于DenseMap还感兴趣的旁友可以瞅瞅[llvm中的数据结构及内存分配策略 - DenseMap](https://blog.csdn.net/dashuniuniu/article/details/80043852)
- [浅谈iOS之weak底层实现原理](https://www.jianshu.com/p/f331bd5ce8f8)这篇其实是我最先找到的博客，可惜跟我的源码长得不太一样，就打入冷宫了，后来瞅瞅其实还不错

## 消息发送及相关知识
- 这一块还是涉及到了isa方面的知识，网上很多文章对isa的分析都不对，还是远古版本，因此没找到什么特别好的文章
- [iOS中的SEl和IMP到底是什么](https://www.jianshu.com/p/4a09d5ebdc2c)这一篇看似短小精悍，但讲的很清楚，要点都在里面了

## 消息转发机制

- 这一块还是上面说的NSMethodSignature以及NSInvocation这两个概念比较抽象，不好懂
- 对这块分析的比较好的[『ios』-objc_msgSend + 消息转发 全面解析（二）](https://www.jianshu.com/p/b2e620684e3f)以及[『ios』NSInvocation NSMethodSignature 全面解析](https://www.jianshu.com/p/20d7d81438dd)
- 对于消息转发的意义，要看[iOS中消息转发机制及工程应用](https://blog.csdn.net/cewei711/article/details/53189620)

# 参考文章
**源码面前，了无秘密**

- [OC源码 —— alloc, init, new和dealloc](https://www.jianshu.com/p/44f2ef4552a8)
- [Runtime源码 —— 对象、类和isa](https://www.jianshu.com/p/a8eade8a1c6d)
- [Runtime源码 —— 概述和调试环境准备](https://www.jianshu.com/p/5d94eafc53ca)
- [详解Class和MetaClass](https://blog.csdn.net/u014686721/article/details/82189342)
- [深入理解objc中的类与对象](https://www.jianshu.com/p/9128a3ccf1ff)
- [iOS Runtime系列一、objc_class深深的误解](https://www.jianshu.com/p/1e16e3b745c1)
- [iOS中isa的深层理解](https://www.jianshu.com/p/20f1251916b9)
- [C/C++ assert()函数用法总结](https://www.cnblogs.com/lvchaoshun/p/7816288.html)
- [Non-pointer isa](http://www.sealiesoftware.com/blog/archive/2013/09/24/objc_explain_Non-pointer_isa.html)
- [结构体 isa_t](https://www.jianshu.com/p/571373ddd0eb)
- [从 NSObject 的初始化了解 isa](https://github.com/Draveness/iOS-Source-Code-Analyze/blob/master/contents/objc/从%20NSObject%20的初始化了解%20isa.md)
- [What is a meta-class in Objective-C?](http://www.cocoawithlove.com/2010/01/what-is-meta-class-in-objective-c.html)
- [Classes and metaclasses](http://www.sealiesoftware.com/blog/archive/2009/04/14/objc_explain_Classes_and_metaclasses.html)
- [数据类型--ULL、ll](https://blog.csdn.net/shenwansangz/article/details/40078407)
- [arm64是什么](https://blog.csdn.net/weixin_33795743/article/details/91154782)
- [深入理解 Tagged Pointer](https://www.infoq.cn/article/deep-understanding-of-tagged-pointer/)
- [iOS开发之runtime（6）：初识class_rw_t](https://www.jianshu.com/p/569e3910cee9)
- [浅谈iOS之weak底层实现原理](https://www.jianshu.com/p/f331bd5ce8f8)
- [C++ 泛型基础](https://blog.csdn.net/xinzheng_wang/article/details/6674847)
- [Objective-C 小记（10）__weak](https://www.jianshu.com/p/eff6b9443800)
- [C++类模板 template <class T>](https://www.cnblogs.com/msymm/p/9750787.html)
- [自旋锁百度百科](https://baike.baidu.com/item/%E8%87%AA%E6%97%8B%E9%94%81/9137985?fr=aladdin)
- [Linux的自旋锁struct spinlock_t的使用](https://www.linuxidc.com/Linux/2012-02/54313.htm)
- [Retry机制](https://blog.csdn.net/qq_20160723/article/details/78085657)
- [Objective-C runtime机制(7)——SideTables, SideTable, weak_table, weak_entry_t](https://blog.csdn.net/u013378438/article/details/82790332)
- [iOS管理对象内存的数据结构以及操作算法--SideTables、RefcountMap、weak_table_t-一](https://www.jianshu.com/p/ef6d9bf8fe59)
- [并发编程系列之一：锁的意义](https://blog.csdn.net/caoshangpa/article/details/78845203)
- [C++ map的基本操作和使用](http://blog.sina.com.cn/s/blog_61533c9b0100fa7w.html)
- [C语言中的位屏蔽(bit masking)是怎么回事](http://c.biancheng.net/cpp/html/1611.html)
- [深入浅出ARC(上)](http://blog.tracyone.com/2015/06/14/%E6%B7%B1%E5%85%A5%E6%B5%85%E5%87%BAARC-%E4%B8%8A/)
- [并发(Concurrent)与并行(Parallel)的区别](https://blog.csdn.net/lconline/article/details/5982237)
- [操作系统内存管理——分区、页式、段式管理](https://blog.csdn.net/hguisu/article/details/5713164)
- [iOS管理对象内存的数据结构以及操作算法--SideTables、RefcountMap、weak_table_t-二](https://www.jianshu.com/p/8577286af88e)
- [OC源码 —— autoreleasepool](https://www.jianshu.com/p/ca7835b41699)
- [ios内存管理（四）：Autorelease技术](https://www.jianshu.com/p/4048230ecda5)
- [ios内存管理（五）：RunLoop与AutoreleasePool](https://www.jianshu.com/p/739496fe4db1)
- [OC源码 —— retain和release](https://www.jianshu.com/p/18c3e88dfbf1)
- [自动释放池的前世今生](https://github.com/draveness/analyze/blob/master/contents/objc/%E8%87%AA%E5%8A%A8%E9%87%8A%E6%94%BE%E6%B1%A0%E7%9A%84%E5%89%8D%E4%B8%96%E4%BB%8A%E7%94%9F.md)
- [Objective-C Autorelease Pool 的实现原理](http://blog.leichunfeng.com/blog/2015/05/31/objective-c-autorelease-pool-implementation-principle/)
- [alignas 说明符 (C++11 起)](https://zh.cppreference.com/w/cpp/language/alignas)
- [runtime(二) SideTables](https://www.jianshu.com/p/ea4c176ffb2b)
- [哈希桶算法](https://www.cnblogs.com/xqn2017/p/7997666.html)
- [llvm中的数据结构及内存分配策略 - DenseMap](https://blog.csdn.net/dashuniuniu/article/details/80043852)
- [<<、>>、>>>移位操作](https://www.cnblogs.com/yulinfeng/p/6602902.html)
- [runtime(三) weak_table_t](https://www.jianshu.com/p/7eb4d291d6d6)
- [C++ 重载运算符和重载函数](https://www.runoob.com/cplusplus/cpp-overloading.html)
- [操作系统中锁的原理](https://www.jianshu.com/p/61490effab35)
- [读读objc源码(二)：weak类型指针的实现](https://www.jianshu.com/p/ed43b17c8a72)
- [weak引用变量是否线程安全](https://www.cnblogs.com/doudouyoutang/p/6275077.html)
- [进一步谈谈 __strong 和 __weak](http://ziecho.com/post/ios/2015-09-02)
- [OC Runtime之Weak(2)---weak_entry_t](https://www.jianshu.com/p/045294e1f062)
- [Objective-C之isa引用计数存储验证](https://www.jianshu.com/p/a4c34e52bdac)
- [探究 cache_t （方法缓存）的本质](https://www.jianshu.com/p/5f59814cd8ba)
- [Objective-C runtime机制(前传2)——Mach-O格式和runtime](https://blog.csdn.net/u013378438/article/details/80431465)
- [Objective-C runtime机制(4)——深入理解Category](https://www.jianshu.com/p/e7c487cfe5eb)
- [iOS 程序 main 函数之前发生了什么](https://blog.sunnyxx.com/2014/08/30/objc-pre-main/)
- [Objective-C Runtime 运行时之三：方法与消息](http://www.cocoachina.com/articles/10150)
- [SEL、IMP和Class](https://www.jianshu.com/p/aff083ec3203)
- [iOS中的SEl和IMP到底是什么](https://www.jianshu.com/p/4a09d5ebdc2c)
- [类型编码（Type Encodings）【苹果官方文章翻译】](https://blog.csdn.net/iosswift/article/details/42361233)
- [iOS多线程-各种线程锁的简单介绍](https://www.jianshu.com/p/35dd92bcfe8c)
- [iOS 代理 重定向消息 forwardInvocation](https://www.cnblogs.com/treejohn/p/3596531.html)
- [iOS 进程间通信翻译](https://www.jianshu.com/p/c0893c93f2cb)
- [Inter-Process Communication](https://nshipster.com/inter-process-communication/)
- [iOS中消息转发机制及工程应用](https://blog.csdn.net/cewei711/article/details/53189620)
- [『ios』NSInvocation NSMethodSignature 全面解析](https://www.jianshu.com/p/20d7d81438dd)
- [『ios』-objc_msgSend + 消息转发 全面解析（二）](https://www.jianshu.com/p/b2e620684e3f)
- [OC学习Runtime之消息传递,消息转发机制](https://blog.csdn.net/u014410695/article/details/48650965)
- [iOS底层原理总结 - RunLoop](https://www.jianshu.com/p/de752066d0ad)
- [RunLoop的深入学习](https://puqin2333.github.io/2018/03/13/RunLoop的深入学习)



