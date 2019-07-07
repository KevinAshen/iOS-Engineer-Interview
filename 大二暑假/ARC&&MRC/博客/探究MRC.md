[TOC]

# 前言

- MRC是在ARC没有出现前的荒蛮时代采用的管理引用计数的方法，学习它主要是为了能理解引用计数的问题，为理解属性关键字，ARC，特别是ARC中的Autoreleasepool

# 准备工作

- 开启MRC

  - [将项目改为MRC的方法(有图有真相)](https://blog.csdn.net/abelyulbb/article/details/51473445)

- 阅读完《Objective-C 高级编程》中的p.1 ~ 29
  - 这里面作者在p.13说Apple没有公布NSObject的源码，因为本书写于2013年
  
  - 现在苹果已经将该源码公布了，感兴趣可以直接clone[objc4](https://github.com/opensource-apple/objc4)