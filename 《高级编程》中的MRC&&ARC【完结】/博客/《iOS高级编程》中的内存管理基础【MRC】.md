目录
=================

   * [关于我的仓库](#关于我的仓库)
   * [前言](#前言)
   * [准备工作](#准备工作)
      * [补充知识：栈与堆](#补充知识栈与堆)
   * [内存管理(引用计数)的思考(p.2 ~ p.13)](#内存管理引用计数的思考p2--p13)
      * [记住一张表格](#记住一张表格)
      * [内存管理四大原则](#内存管理四大原则)
         * [注意点](#注意点)
         * [疑点](#疑点)
   * [内存管理四兄弟【alloc】【retain】【release】【dealloc】(p.13 ~ p.20)](#内存管理四兄弟allocretainreleasedeallocp13--p20)
      * [说明](#说明)
      * [alloc](#alloc)
         * [计数](#计数)
         * [GNU实现](#gnu实现)
      * [retainCount](#retaincount)
      * [retain](#retain)
      * [release&amp;&amp;dealloc](#releasedealloc)
      * [Apple实现](#apple实现)
         * [引用计数&amp;&amp;哈希表](#引用计数哈希表)
   * [autorelease(p.20 ~ p.28)](#autoreleasep20--p28)
      * [arrayWithCapacity](#arraywithcapacity)
      * [苹果实现](#苹果实现)
         * [SEL&amp;&amp;IMP](#selimp)
         * [push&amp;&amp;pop](#pushpop)
   * [困惑](#困惑)
---
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- MRC是在ARC没有出现前的荒蛮时代采用的管理引用计数的方法，学习它主要是为了能理解引用计数的问题，为理解属性关键字，ARC，特别是ARC中的Autoreleasepool

# 准备工作

- 开启MRC

  - [将项目改为MRC的方法(有图有真相)](https://blog.csdn.net/abelyulbb/article/details/51473445)

- 阅读《Objective-C 高级编程》中的p.1 ~ 29

## 补充知识：栈与堆
- 栈：创建临时变量时由编译器自动分配，在不需要的时候自动清除的变量的存储区。里面的变量通常是局部变量、函数参数等。在一个进程中，位于用户虚拟地址空间顶部的是用户栈，编译器用它来实现函数的调用。和堆一样，用户栈在程序执行期间可以动态地扩展和收缩。
- 堆：就是那些由 new alloc 创建的对象所分配的内存块，它们的释放系统不会主动去管，由我们的开发者去告诉系统什么时候释放这块内存(一个对象引用计数为0是系统就会回销毁该内存区域对象)。一般一个 new 就要对应一个 release。在ARC下编译器会自动在合适位置为OC对象添加release操作。会在当前线程Runloop退出或休眠时销毁这些对象，MRC则需程序员手动释放。堆可以动态地扩展和收缩。
```objective-c
@interface TestObject()

@end
@implementation TestObject
- (void)testMethodWithName:(NSString *)name
{
   //方法参数name是一个指针，指向传入的参数指针所指向的对象内存地址。name是在栈中
  //通过打印地址可以看出来，传入参数的对象内存地址与方法参数的对象内存地址是一样的。但是指针地址不一样。
  NSLog(@"name指针地址:%p,name指针指向的对象内存地址:%p",&name,name);


  //*person 是指针变量,在栈中, [Person new]是创建的对象,放在堆中。
  //person指针指向了[Person new]所创建的对象。
  //那么[Person new]所创建的对象的引用计数器就被+1了,此时[Person new]对象的retainCount为1
  Person *person = [Person new];
}
```

# 内存管理(引用计数)的思考(p.2 ~ p.13)

## 记住一张表格

| 对象 | 方法 | 引用计数 |
| :----------: | :-----------: | :--: |
| 生成对象并自己持有 | alloc/copy | 变成 1 |
| 持有对象 | retain  | ++ |
| 释放对象 | release | -- |
| 废弃对象 | dealloc |  |
| 生成但不持有对象 | array | -1 |

## 内存管理四大原则

1. 自己生成的对象，自己持有
2. 非自己生成的对象，自己也能持有
3. 不再需要自己持有的对象时释放
4. 非自己持有的对象无法释放

### 注意点
1. 我们不要纠结于引用计数的事，引用计数本质上只是一个帮助我们观察状态的工具，合理的思考方式就是按照上面四个原则去思考
2. 持有到底是什么意思？持有的本质其实就是强引用
```objective-c
NSObject *obj1 = [[NSObject alloc] init];
//这句话里面我们在内存中分配了一块空间，存放一个NSObject对象，而我们的obj1是一个指针，指向该对象
//同时，obj1也强引用即持有了该对象，会使该对象的引用计数+1
//强引用即__strong，在OC中，不佳特别的修饰符会默认为强引用
```

### 疑点
1. 这里按照强应用即持有的观点，obj1持有了该对象，这和书上的是对象的使用环境（编程人员自身）持有对象这一观点不符
2. 但是如果按照书上的理解，实在很难想象为什么可以对象被多个持有

# 内存管理四兄弟【alloc】【retain】【release】【dealloc】(p.13 ~ p.20)

## 说明

- 《高级编程》中在p.13页提到Apple没有公布NSObject中的源代码，也就是说看不到这四个方法的具体实现

- 这么说其实是因为该书写于2013年，现在都2019年了，这部分的源码苹果其实已经早就公开了

- 也就是说从13到20这么多页其实都是作者通过研究GNUstep推测的Apple的实现

- 当然，这些依然还是有意义的，作者对于其中原理的讲解很是透彻

- 但除此之外在认真研究下Apple公司真正的实现方式也是很有必要的


## alloc

- 在ARC中，我们是无法调用【retain】【release】【dealloc】这几个操作的，只有alloc是可以使用的
- ARC与MRC相比，我们可以理解成，ARC帮助我们在适当的使用调用了上面几个方法，对引用计数进行了管理

### 计数
- 对某一对象进行alloc初始化，在打印它的retainCount

```objective-c
		NSObject *obj = [[NSObject alloc] init];
   [Print printRetainCount:obj];
```

- 结果为1，认为obj持有了这个对象
### GNU实现
- 它存储引用计数的方式是存在一个结构体里面，该结构体里只有一个NSInteger类型的数据retained，也就是引用计数
```objective-c
//p.15		
struct obj_layout {
    NSUInteger retained;	//引用计数
};
 
+ （id)alloc
{
    int size = sizeof(struct obj_layout) + 对象大小;	//size大小就是对象的是实际大小加上一个结构体
    struct obj_layout *p = (struct obj_layout *)calloc(1, size);
  	return (id)(p + 1)	//这里应该是返回该对象的内存地址，它返回了p+1，也就是说返回了掠过引用计数结构体的内存地址
		//也就是说引用计数存在了对象的头地址里
}
```

## retainCount
- 首先，我们看到初始化的时候其实是将里面的数据全部记为0的，在进行了alloc之后，按道理引用计数应该是0
- 而我们调用retainCount打印出来却会是1
```objective-c
//p.15
- (NSUInteger) retainCount
{
    return NSExtraRefCount(self) + 1;
}
 
inline NSUInteger
NSExtraRefCount(id anObject)
{
    return ((obj_layout)anObject)[-1].retained;	//这个[-1]实在是有点迷人，我的理解是我们这里的anObject是对象的内存地址，而上面我们看到了刨掉引用计数才是我们的内存地址，对于对象的地址是[0]，[-1]是它的前一个，也就是引用计数了
}
```

- return的是self+1，所以打印出来会是1

## retain
- retain要注意的是判断了下引用计数是否到达了最大值（防止引用计数溢出）

## release&&dealloc
- release和retain的套路基本一致，不同的是它要判断的是是否到0
- 当引用计数是1的时候，进行release会自动调用dealloc
- 这里的dealloc是直接将该内存空间free了

## Apple实现
- Apple的实现和前面分析的其实没什么区别，这里关键的其实是发现了Apple存储引用计数的方式——哈希表（看到散列表突然懵逼的同学请回去重修下数据结构🐶）
### 引用计数&&哈希表
- 先大概回顾下哈希表：哈希表（hash table，也叫散列表），是根据（key）直接访问访问在内存储存位置的数据结构。
哈希表本质是一个数组，数组中的每一个元素成为一个箱子，箱子中存放的是键值对。根据下标index从数组中取value。关键是如何获取index，这就需要一个固定的函数（哈希函数），将key转换成index。不论哈希函数设计的如何完美，都可能出现不同的key经过hash处理后得到相同的hash值，这时候就需要处理哈希冲突。
- 这里先不讨论Apple使用何种方式解决的哈希冲突，单单将该表理解成数组的话，其实就是一个键是对象的地址，值是引用计数的表，两者一一对应
- 而这样存储的优点和GNU比也很好理解，就是别把鸡蛋放在一个篮子里，用哈希表存储，两者相互独立，内存损坏也不影响查引用计数

# autorelease(p.20 ~ p.28)
- autorelease我的理解就是它使用了一个NSAutoreleasePool这个来进行辅助，帮助判断何时调用release方法
- 进行[pool drain]的时候，则同时会进行release
- 同时如果生成了好几个pool，使用的会是最近生成的一个
- pool等于是一个数组，每个调用了autorelease的对象，被添加到这个数组里面

## arrayWithCapacity
```objective-c
 NSMutableArray *orderIds = [NSMutableArray arrayWithCapacity:self.dataArray.count]
 初始化可变数组对象的长度,如果后面代码继续添加数组超过长度以后长度会自动扩充.
初始化方法 capacity后的NSUInteger代表了开辟内存的一个单位初始在内存中开辟5个内存,如果之后数组元素多余5个,则会再开辟新的5*2个新的内存,[考虑到数组的连续内存的特性]单位是以5,把之前的5个元素的内容拷贝到新的十个新的内存里面,把第六个也放进去,然后释放初始状态创建的内存5个最后得到了一块够用的连续的内存5*2
   
 NSMutableArray *array = ［NSMutableArray alloc] init];//这并不是一个好方法

 NSMutableArray *array = [NSMutableArray arrayWithCapacity:10]; //创建一个可变的数组长度为10

区别:

1. arrayWithCapacity是类autorelease的.

2. [NSMutableArray alloc]initWithCapacity需要自己release.
```

## 苹果实现
### SEL&&IMP
- SEL : 类成员方法的指针，但不同于C语言中的函数指针，函数指针直接保存了方法的地址，但SEL只是方法编号。
- 一个函数指针,保存了方法的地址
- 每一个继承于NSObject的类都能自动获得runtime的支持。在这样的一个类中，有一个isa指针，指向该类定义的数据结构体,这个结构体是由编译器编译时为类（需继承于NSObject）创建的.在这个结构体中有包括了指向其父类类定义的指针以及 Dispatch table. Dispatch table是一张SEL和IMP的对应表。
- 也就是说方法编号SEL最后还是要通过Dispatch table表寻找到对应的IMP，IMP就是一个函数指针，然后执行这个方法
### push&&pop
- Apple实现从push，pop这两个方法命名也就可以看出他本质上和前面一样，就是在数组里面进行压入压出操作

# 困惑
```objective-c
  [obj alloc]
  [obj release]
```

- 执行完release后，打印计数依然是1，此处存疑
  - 我们此时引用计数由0到1，应该自动调用了dealloc，而此时依然能打印计数，也就是对其进行了访问
    - 在此基础上发现此时可以对obj在访问一次，不管是进行release，访问计数，都可以，但是只有这一次，在进行访问就会报错
    - 另外很神奇的是retain不在此列，它不占这个名额，你可以进行任意多次的retain操作，也不会报错
    - 我们可以推测retain确实是指判断最大值的
  - 这是在MRC中进行的操作，我们如果再来一句dealloc，会提示obj已经被free，然后依然能打印出obj的地址
- 我们可以推测出release确实自动调用了dealloc，但是也不是像GNU里面一样简单的free了
- 当然，使用p.20的哈希表可以解决部分疑惑，我们唯一可以明确的就是计数0到1的时候自动dealloc了，且绝对没有将obj指针free。显然这中间到底发生了什么神秘的变化需要我们从源码中探究了