   * [大二暑假](#大二暑假)
      * [说明](#说明)
      * [时间范围](#时间范围)
      * [目标](#目标)
      * [内容](#内容)
         * [iOS基础相关（必会，面试必问）](#ios基础相关必会面试必问)
            * [语言和底层相关【重要： 面向对象的三大特性（关于OC）】：](#语言和底层相关重要-面向对象的三大特性关于oc)
            * [UI相关：](#ui相关)
         * [计算机基础相关（必会，面试必问）](#计算机基础相关必会面试必问)
   * [特别鸣谢](#特别鸣谢)
      * [说明](#说明-1)
      * [名单](#名单)

# 大二暑假

## 说明

- 自己手头还有一个项目没完善，另外暑假还要和人合作开发一个APP端与前端交互的轮子
- 这个仓库只会放与面试相关的复习，项目的内容不会放在这里
- 我会把每一部分的学习用的所有东西都归纳成一个文件夹
  - 文件夹里会包括一篇概览的md，写的代码demo，学习期间写的博客，参考的文章
  - 如果你看到这个仓库觉得对你确实有帮助，请点击右上角的star，要是发现我写的东西有问题，也非常欢迎提出issue
## 时间范围
- 2019.7.13 ～ 2019.8.16
- 其中7.21 ～ 8.16为实验室规定的统一的留校时间，前面一周是刚考完试自己的学习时间
## 目标
- 下一学期10月底左右参加面试，并在寒假参与日常实习
- 暑假将面试会考到的知识基本都涵盖到，开学那两个月进行复习

## 内容

### iOS基础相关（必会，面试必问）

#### 语言和底层相关【重要： 面向对象的三大特性（关于OC）】：

1. ARC与MRC的区别&&RunTime&&RunLoop
  - ARC与MRC的区别
    - [ARC和MRC](https://www.jianshu.com/p/5eac83471b23)
  - RunTime讲述整个轮廓
    - weak底层实现
  - RunLoop要了解
    - [autoreleasePool](http://blog.leichunfeng.com/blog/2015/05/31/objective-c-autorelease-pool-implementation-principle/)
3. 属性关键字&&循环引用&&Category&&消息发送三次拯救
  - [深入理解Objective-C：Category](https://tech.meituan.com/2015/03/03/diveintocategory.html)
4. 探索block的原理
  - 要求能独立描述block的整个实现
  -  可参考《iOS高级编程》
5. KVC、KVO原理、单例、代理、iOS 存储
  - 每个人必须熟悉
6. 多线程编程 GCD、 NSOpearation（线程安全、死锁）网络请求的几种方式
  - 了解NSURLSession 、NSURLSession 和NSURLConnection区别、第三方框架AFN
#### UI相关：
- 响应者链
- 贝塞尔曲线、CALayer
- iOS native 与 js 交互(JSBrdige)

### 计算机基础相关（必会，面试必问）

- 数据结构
	- 链表的常见算法：例如 逆置
	- 二叉树的常见算法： 例如 三种遍历（递归和非递归）
	- 可参考：[博客](https://blog.csdn.net/qq_38499859/article/list/2?)
- 算法
  - [Leetcode](https://leetcode-cn.com) 面试官经常从这里面抽题问
  - 根据个人进度，暑假每人最少刷够10道
- 计算机网络
- 《图解HTTP》（可选）
	- 需要知道从点击一个URL到显示的整个过程，对这部分不了解的必须看
# 特别鸣谢
## 说明
- 在各个部分的参考文章里吧主要的参考文章都列出来了，但是对于GitHub上clone的资料没有提到，就放在这里统一鸣谢，感谢这些大大的精彩分享
## 名单
- [acBool / RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)
- [DeveloperErenLiu / RuntimePDF](https://github.com/DeveloperErenLiu/RuntimePDF)
- [RuntimeAnalyze](https://github.com/DeveloperErenLiu/RuntimeAnalyze)