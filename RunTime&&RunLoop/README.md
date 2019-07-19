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

# 难点

## 类，对象，isa
### isa闭环
- 里面又有元类，又有父类，要理清楚还挺麻烦的
- 另外还要明白所谓一切都是对象

### isa_t
- 这个主要比较麻烦，从定义到初始化过程，都要很细致的一点点找下去
- 特别是初始化和获取isa那一块，再次暴露我计算机基础之薄弱🌚

## autoreleasepool
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

## token

- token这个很多博文讲的不清楚，含糊过去了，导致我一直对releaseuntil不太懂
- 直到遇到了小神

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