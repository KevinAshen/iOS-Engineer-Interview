目录
=================

   * [前言](#前言)
   * [《iOS高级编程》中的内存管理基础【MRC】](#ios高级编程中的内存管理基础mrc)
      * [难点](#难点)
         * [引用计数&amp;&amp;持有](#引用计数持有)
         * [autorelease](#autorelease)
      * [我的博客](#我的博客)
   * [《iOS高级编程》中的ARC](#ios高级编程中的arc)
      * [难点](#难点-1)
         * [__strong修饰符](#__strong修饰符)
      * [我的博客](#我的博客-1)
   * [正确打开【学习】姿势](#正确打开学习姿势)
   * [参考文章](#参考文章)
---
# 前言

- 这里面我的两篇博客基本上都是在阅读《iOS高级编程》过程中总结而成的
- 关于这本书，学习资料里有pdf版
- 这本书确实写的非常好，个人觉得写的很透彻，但就可惜太老了，其中很多源代码实现目前都改了，所以我建议学习的时候对于其概念，内存管理的思维方式等等着重学习，其源代码实现可以不用太过纠结【当然，MRC&&ARC部分也没有太详细的源代码实现】
- 尤其是其中__weak实现这种非常重要的重点，光是知道有个weak表，并没有什么用
- 另外，书里提到什么iOS4以前，之类blblbl，我都没有看，都跳过了，这老黄历也太早了😳

# 《iOS高级编程》中的内存管理基础【MRC】
## 难点
### 引用计数&&持有

- 这两个本身就是一个意思，我觉得就按照作者所说，思考的时候使用持有，释放这样的方式去思考，对于多个持有的时候，用引用计数去思考，将引用计数作为一个辅助的理解手段，可能是个比较好的选择

### autorelease

- 越学到后面越发现autorelease非常重要，后面的很多东西都是借助其实现的
- 理解autorelease，关键还是分清对象，一个pool，一个对象本身，对于所有的操作，从这两个方面去思考其中的变化
## 我的博客
- 本文主要是《iOS高级编程》p.1~p.29内容的类似读书笔记的总结
- 在这一块，主要在于理解内存管理的四大原则，了解书中的持有以及引用计数之间的关系
- 链接：[《iOS高级编程》中的内存管理基础【MRC】](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/%E3%80%8A%E9%AB%98%E7%BA%A7%E7%BC%96%E7%A8%8B%E3%80%8B%E4%B8%AD%E7%9A%84MRC&&ARC%E3%80%90%E5%AE%8C%E7%BB%93%E3%80%91/%E5%8D%9A%E5%AE%A2/%E3%80%8AiOS%E9%AB%98%E7%BA%A7%E7%BC%96%E7%A8%8B%E3%80%8B%E4%B8%AD%E7%9A%84%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86%E5%9F%BA%E7%A1%80%E3%80%90MRC%E3%80%91.md)

# 《iOS高级编程》中的ARC
## 难点
### __strong修饰符

- __strong是默认修饰符，我花了很长时间去理解其中的过程，尤其是其中相互引用
- 在看相互引用的时候，我是直接画了表格，分开思考各自的引用计数，释放与否的关系，事实证明，这样的方式很有效，花的功夫也没有白费，对于__strong了解透彻了，后面就没什么难的了
- 也是因为，后面基本都跳了🌚

## 我的博客

- 本文主要是《iOS高级编程》p.29~p.78内容的类似读书笔记的总结
- 链接：[《iOS高级编程》中的ARC](https://github.com/KevinAshen/iOS-Engineer-Interview/blob/master/%E3%80%8A%E9%AB%98%E7%BA%A7%E7%BC%96%E7%A8%8B%E3%80%8B%E4%B8%AD%E7%9A%84MRC&&ARC%E3%80%90%E5%AE%8C%E7%BB%93%E3%80%91/%E5%8D%9A%E5%AE%A2/%E3%80%8AiOS%E9%AB%98%E7%BA%A7%E7%BC%96%E7%A8%8B%E3%80%8B%E4%B8%AD%E7%9A%84ARC.md)

# 正确打开【学习】姿势

- 这位大大的博客[《Objective-C 高级编程》干货三部曲（一）：引用计数篇](https://www.jianshu.com/p/9172ff9c8b5c)对我帮助比较大
  - 他把基本上所有涉及代码都放上去了，对于自己验证的时候很方便
  - 参看了下他觉得重要的章节，成功使我狠下心来跳了好几页😂
  - 他在其中补充的一些面试题，也成功让我耐下心来，啃了几页本来想放弃的天书🌚
  - 我一般是先看书，看完一部分，回过头来看他这部分的博客，看看有没有遗漏的知识

- 这篇博客[iOS-MRC与ARC区别以及五大内存区](https://www.jianshu.com/p/5eac83471b23)前面的内存区域部分建议在看书前先看一遍，作为知识补充
- 学习过程中，我感觉主要还是要分清对象，有些问题因为涉及到了多个对象，感觉难以理清的话可以画个表，分别思考
- 我看书的时候，先看，在书上写理解，在笔记本上写比较长的思考过程，学不动了就开始写博客，写博客的时候就照着书和笔记本一点一点往下写，有的时候也能查漏补缺到【主要是放松下，忒多了】
- 这里面要敲的代码不是很多，主要都是在看MRC的时候，做了很多实验，后面基本没怎么敲
- 比如在循环引用那里，你就算循环引用了也不会报错，没有用模拟机也用不了leak检验内存泄漏

# 参考文章
**程序员和上帝打赌要开发出更大更好——傻瓜都会用的软件。而上帝却总能创造出更大更傻的傻瓜。所以，上帝总能赢。**

- [iOS打印引用计数](https://www.jianshu.com/p/e059a54fb5f3)
- [将项目改为MRC的方法(有图有真相)](https://blog.csdn.net/abelyulbb/article/details/51473445)
- [如何理解init和dealloc方法 - OC](https://www.jianshu.com/p/cbcf0becba85)
- [iOS 数组~arraywithcapacity方法](https://blog.csdn.net/shihuboke/article/details/78153827)
- [SEL用法](https://www.jianshu.com/p/3291be7c7bb4)
- [iOS中的SEl和IMP到底是什么](https://blog.csdn.net/dkq972958298/article/details/69942077)
- [iOS强引用与弱引用](https://www.jianshu.com/p/2d0f3fee817f)
- [搞iOS的，面试官问Hash干嘛？原因远比我下面要介绍的多](https://juejin.im/post/5c6abfc86fb9a049c04396a7)
- [《Objective-C 高级编程》干货三部曲（一）：引用计数篇](https://www.jianshu.com/p/9172ff9c8b5c)
- [知识点：id 和NSObject *对比，以及id <NSObject>介绍](https://juejin.im/post/5be127506fb9a049f91208bc)
- [ARC 环境下 dealloc 的使用误区](https://blog.csdn.net/dt1991524/article/details/81355871)
- [iOS开发 ARC下dealloc的使用](https://blog.csdn.net/jijiji000111/article/details/52446684)
- [可能是史上最全面的内存管理文章](https://www.jianshu.com/p/6cf682f90fa2)
- [iOS __bridge那些事](https://www.jianshu.com/p/5fbe5478e24b)
- [iOS开发之__bridge，__bridge_transfer和__bridge_retained](http://www.qingpingshan.com/rjbc/ios/157477.html)
- [iOS-MRC与ARC区别以及五大内存区](https://www.jianshu.com/p/5eac83471b23)