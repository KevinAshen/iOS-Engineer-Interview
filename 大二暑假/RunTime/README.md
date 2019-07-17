[TOC]
# 前言

- 看完《高级编程》中的MRC&&ARC，开始研究RunTime源码，目标在于弄懂ARC中真正的实现，消息发送机制，类与对象的本质，weak底层实现
- 学的时候比看书的时候痛苦多了，看《高级编程》到底只要老老实实看书就行，可这玩意得网上各种找资料看
- 再加上很多人源代码的版本不统一，简直了，要开着几个版本的源码对照着看
- 写博客其实一开始也没想好写什么内容，是看着看着，大概理清楚思路后才明白应该总结那些内容，真的hard模式
- 看的时候常常让我回想到当初看《百年孤独》的感受，真就魔幻现实呗
- 尤其是什么类，对象，isa，不觉得像一句诗吗？
- runtime东西真的非常多，从任何一点看下去都能学到些东西，但怎么说呢，皓首穷经谁顶得住，所以在看完基本结构体后，我就直接开始看weak之类的特别的知识点，从这些东西出发，回过去看runtime，不再不管不顾的看了，实在太多

# 难点
## 类，对象，isa
### isa闭环
- 里面又有元类，又有父类，要理清楚还挺麻烦的
- 另外还要明白所谓一切都是对象

### isa_t
- 这个主要比较麻烦，从定义到初始化过程，都要很细致的一点点找下去
- 特别是初始化和获取isa那一块，再次暴露我计算机基础之薄弱🌚

# 正确打开【学习】姿势
## 类，对象，isa
- 这部分虽然零零碎碎看了很多文章，真正特别有用的是这几篇
- 首先一定要看下[iOS Runtime系列一、objc_class深深的误解](https://www.jianshu.com/p/1e16e3b745c1)，会让你找资料的时候远离那些上古文章
- 对象类基础一块把[Runtime源码 —— 对象、类和isa](https://www.jianshu.com/p/a8eade8a1c6d)认真看下；此外最后的打印试验建议也跟着做一遍，会提高感触
- isa部分[从 NSObject 的初始化了解 isa](https://github.com/Draveness/iOS-Source-Code-Analyze/blob/master/contents/objc/从%20NSObject%20的初始化了解%20isa.md)是真的强，一定要精读，作者已经是我偶像了，太强了
- runtime概念[探秘Runtime - Runtime介绍](https://www.jianshu.com/p/ce97c66027cd)
- 类的bit部分[Class的结构](https://www.jianshu.com/p/fd7aad6297f0)
- 最后查漏补缺[探秘Runtime - 剖析Runtime结构体](https://www.jianshu.com/p/5b7e7c8075ef)