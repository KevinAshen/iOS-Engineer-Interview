[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- ARC使用了四种所有权修饰符对引用计数进行管理，使得不再需要手动操作retain，release
- 理解ARC规则，使得对于内存管理的思考方式更进一步，以OC的方式来思考内存管理是理解的难点
- 由于markdown渲染的锅，本文忽略了一些双下划线

# 准备工作
- 阅读《Objective-C 高级编程》中的p.29 ~ 78

- 回顾下内存管理四大原则是不是**牢牢记住**了

## 内存管理四大原则

1. 自己生成的对象，自己持有
2. 非自己生成的对象，自己也能持有
3. 不再需要自己持有的对象时释放
4. 非自己持有的对象无法释放

# ARC规则(p.29 ~ p.65)

## __strong修饰符

- __strong是默认修饰符
```objective-c
id obj = [[NSObject alloc] init];
//id __strong obj = [[NSObject alloc] init];
NSObject *obj = [[NSObject alloc] init];
//NSObject __strong *obj = [[NSObject alloc] init];
```
### 补充知识：id的本质
- id是struct objc_object结构体指针,可以指向任何OC对象，当然不包括NSInteger等，因为这些数据类型不是OC对象。另外OC的基类，其实不仅仅就NSObject一个，虽然NSObject是绝大数OC对象的基类，但是还有个NSProxy虚类。所以不能说id类型和NSObject *是等价的。
- id类型的实例在编译阶段不会做类型检测，会在运行时确定，所以id类型是运行时的动态类型。类NSObject的实例会编译期要做编译检查，保证指针指向是其NSObject类或其子类，当然实例的具体类型要在运行期确定，这也是iOS的多态的体现。
- 记住：id本身就是个指针【不记住这一点的话，后面看到__autoreleasing可能会像我一样崩溃了🌚】

### 超出变量作用域 = 废弃
- 这里要开始转换思维模式了，比如这样一句代码id obj = [[NSObject alloc] init];
- 我们这里有两个东西，一个是obj，它是一个指针，指向生成对象；同时，它隐式使用了__strong，它也就持有了该对象，成为对象持有者【也就是说对象的引用计数+1了，还是强调一遍，把引用计数作为辅助手段去理解，还是要从持有与否的角度去看待内存管理】
- 这里我们看p.31最下面这个obj失效的例子
- 记住这个反应链：
  1. obj超出作用域，obj失效，不再持有该对象
  2. 失去强引用，对象不在被持有【也就是引用计数从0到1，自动dealloc】
  3. 对象被释放
- 这里一定要分清，持有者是持有者，对象是对象，两者不是一个东西

### __strong对象相互赋值

- 书上这个例子可能注释的还不是很好理解，我这里直接放一张表格，研究三个obj分别持有谁

|        语句        |     obj0     | obj1 |   obj2   |
| :----------------- | :----------: | :------: | :--: |
| id __strong obj0 = [[NSObject alloc] init];//生成对象A |  A  |    |      |
|      id __strong obj1 = [[NSObject alloc] init];//生成对象B      | A |    B    |      |
|      id __strong obj2 = nil;      |   A    |    B    | nil |
|      obj0 = obj1;//obj0强引用对象B；而对象A不再被ojb0引用，被废弃      |   B    | B | nil |
|  obj2 = obj0;//obj2强引用对象B（现在obj0，ojb1，obj2都强引用对象B）  |    B     |    B    | B |
| obj1 = nil;//obj1不再强引用对象B | B | nil | B |
| obj0 = nil;//obj0不再强引用对象B | nil | nil | B |
| obj2 = nil;//obj2不再强引用对象B，不再有任何强引用引用对象B，对象B被废弃 | nil | nil | nil |
- 赋值的本质是强引用的转变，这也可以帮助我们理解为什么要引入这一套规则，我们不能也不用去直接对对象进行操作管理，使用指针和引用计数安全，有效的进行使用

#### 方法参数中使用__strong

- 到这里还是重复确认下，__strong就是默认的修饰符
- 以及这句话很关键**废弃Test对象的同时，Test对象的obj_成员也被废除**
- 也就是说成员变量的生存周期是与对象同步的

### __strong导致的循环引用
- 前面所有的强调加黑的文字都是为了能理解这里的循环引用
#### 补充知识：内存泄漏
- 简单来说，内存泄漏就是在内存该被释放的时候没有释放，导致内存被浪费使用了
- 内存泄漏在iOS开发中轻则影响性能，重则导致crash
#### 赋值阶段
- 这里还是放一张表格，说明这两个test以及其成员持有谁

|        语句        |     test0     | test0.obj | test1 | test1.obj |
| :----------------- | :----------: | :------: | :--: | :----------------: |
| id test0 = [[Test alloc] init];//生成TestA |  TestA  |    |  |  |
|      id test1 = [[Test alloc] init];//生成TestB      | TestA |        | TestB |  |
|      [test0 setObject:test1];      | TestA |    TestB    | TestB | TestA |
|      [test1 setObject:test0];      | TestA | TestB | TestB | TestA |
#### 失效阶段
- 这里为了加强理解，直接把各自的引用计数，持有对象都列出来

|        What happen        |     TestA引用计数     | TestA持有者 | TestB引用计数 | TestB持有者 |
| :----------------: | :----------: | :------: | :--: | :----------------: |
| 初始状态 |  2  | test0，test1.obj | 2 | test0.obj，test1 |
|      test0超出作用域      | 1 | test1.obj | 2 | test0.obj，test1 |
|      test1超出作用域      | 1 |    test1.obj    | 1 | test0.obj |
#### 为什么test0失效的时候，obj_依然存在
- 这里还是用引用计数的方式来思考比较好

- 只有当某个对象的持有者都被释放了，该对象才会被dealloc，而这里的test0，test1仅仅只是**指针而已**

- 这点非常重要，如果在你的脑海里想的是test0被释放，他下面的obj_自然不复存在了，请牢记**test0仅仅是一个指针，是一个对象的持有者，不是对象本身**

#### 造成结果
##### 我要dealloc TestA
- 我们看，testA dealloc不了，因为test1.obj持有了testA
- 而想要废除test1.obj，就如我们上面介绍的，**废弃Test对象的同时，Test对象的obj_成员也被废除**
- 而test1是TestB内存里的成员变量
- 一句话，想dealloc TestA，必须先dealloc TestB才行
##### 我要dealloc TestB
- 我们看，testB dealloc不了，因为test0.obj持有了testB
- 而想要废除test0.obj，就如我们上面介绍的，**废弃Test对象的同时，Test对象的obj_成员也被废除**
- 而test0是TestA内存里的成员变量
- 一句话，想dealloc TestB，必须先dealloc TestA才行
#### 对自身的强引用
- 对自身的强引用和上面其实一个意思，test，test.obj同时持有了对象，test超出定义域release，对象引用计数-1
- 而test.obj遇到了和上面一样的难题，如果还是想不通发生了什么，请把上面两段话再认真看看
## __weak修饰符
- __weak持有弱引用
- 如果对于strong修饰符理解OK的话，这个weak修饰符其实很好懂

### 生成__weak的持有者

```objective-c
id __weak obj = [[NSObject alloc] init];
//这里如果直接使用__weak obj来持有对象，由于这里是弱引用，引用计数不会加一，对象随时可能会被dealloc
//后面会讲到，其实内部是用__autoreleasing来维持该对象不被dealloc

id __strong obj0 = [[NSObject alloc] init];
id __weak obj1 = obj0;
//在这里，obj0先强持有该对象，给它引用计数+1，防止其dealloc
//之后再让obj1去弱持有该对象，达成我们需要的目的
```
### 使用__weak的好处

- 如果对于前面循环引用的原因研究理解到位了，应该就能明白为什么__weak能避免循环引用
- __weak不会增加引用计数，相应的对象该dealloc就会dealloc，其中的持有者自然而然就会被release
- 这里可以思考下，如果一个弱引用，一个强引用，这样子的相互引用会导致循环引用吗？

## __autoreleasing修饰符

### 与MRC时比较

- 先回忆一下，MRC中autorelease的使用方法
  1. 生成并持有NSAutoreleasePool对象。
  2. 调用已分配对象的autorelease方法。【将对象注册到pool中】
  3. 废弃NSAutoreleasePool对象。【pool执行drain废除，其中的对象也跟着release】
- 我们会发现，这里出现两个部分，一个pool，一个对象
- 于此相对的，__autoreleasing同样分成两块

![64149CAE-5DF0-44CC-9E67-3A10081A9DC3](http://ww1.sinaimg.cn/large/006tNc79ly1g5094xtw97j318i0iqao4.jpg)
### 自动调用
- MRC中有介绍，像array这样的方法，生成的对象不是由自己持有的，其中就是靠__autoreleasing修饰符去实现
- 当编译器检测到这样的方法命名后，就会自动加上__autoreleasing修饰符
- 那么这里有个注意点就是，strong才是默认的修饰符，我们如果用strong修饰符去接收的对象，当其超出作用域的时候，strong修饰符先失效，走出@autoreleasepool块后，__autoreleasing修饰符失效

### weak修饰符与autoreleasing修饰符

- 如同上面提到的，weak修饰符的实现要借助autoreleasing修饰符
```objective-c
id  __weak obj1 = obj0;
NSLog(@"class = %@",[obj1 class]);

id __weak obj1 = obj0;
id __autoreleasing tmp = obj1;
NSLog(@"class = %@",[tmp class]);//实际访问的是注册到自动释放池的对象
```
- 由于weak不会增加引用计数，对象难以维持，所以要通过__autoreleasing来维护
- 在使用附有weak修饰符的变量时就必定要使用到autoreleasing修饰符

### autoreleasing修饰符无处不在

**牢记：只有作为alloc/new/copy/mutableCopy方法的返回值而渠道对象时，能够自己生成并持有对象，其他情况都是"取得非自己生成并持有的对象"，换句话说，就轮到我们的autoreleasing修饰符上场了**

## 具体ARC规则
### 规则
- 不能使用retain/release/retainCount/autorelease
- 不能使用NSAllocateObject/NSDeallocateObject
- 必须遵守内存管理的方法名规则
- 不要显式调用dealloc
- 使用@autorelease块代替NSAutoreleasePool
- 不能使用区域（NSZone）
- 对象型变量不能作为C语言结构体的成员
- 显式转换id和void*

### dealloc

- 重写dealloc方法时不需要写[super dealloc]
- dealloc无法释放不属于该对象的一些东西，需要我们重写时加上去，例如
  - 通知的观察者,或KVO的观察者
  - 对象强委托/引用的解除(例如XMPPMannerger的delegateQueue)
  - 做一些其他的注销之类的操作(关闭程序运行期间没有关闭的资源)
```objective-c
- (void)dealloc  
{  
    [[NSNotificationCenter defaultCenter] removeObserver:self];//移除通知观察者  
    [[XMPPManager sharedManager] removeFromDelegateQueue:self];//移除委托引用  
  [ [MyClass shareInstance]  doSomething ]//其他操作  
     
}  
```
### __bridge

- `__bridge`可以实现**Objective-C与C语言变量** 和 **Objective-C与Core Foundation对象**之间的互相转换
- `__bridge`**不会改变对象的持有状况**，既不会`retain`，也不会`release`
- `__bridge`转换需要慎重分析对象的持有情况，稍不注意就会**内存泄漏**
- `__bridge_retained`用于**将OC变量转换为C语言变量** 或 **将OC对象转换为Core Foundation对象**
- `__bridge_retained`类似于`retain`，“被转换的变量”所持有的对象在变量赋值给“转换目标变量”后**持有该对象**
- `__bridge_transfer`用于**将C语言变量转换为OC变量** 或 **将Core Foundation对象转换为OC对象**
- `__bridge_transfer`类似于`release`，“被转换的变量”所持有的对象在变量赋值给“转换目标变量”后**随之释放**

### 属性声明与所有权修饰符

![D1980B5B-D29C-4B3B-8FC0-121567F38F0C](http://ww2.sinaimg.cn/large/006tNc79ly1g50btvlsf3j31820im0ug.jpg)

# ARC实现(p.65 ~ p.78)
## 说明
- 这是一张最新（750.1）objc4库.mm文件列表

![C10E595E-A479-433F-AE43-9920A37275D1](http://ww3.sinaimg.cn/large/006tNc79ly1g50csayd7tj30740ogq43.jpg)

- 可以看到，这个里甚至没有作者提到的objc-arr.mm这个文件了
- 还是再强调一次，objc4库Apple一直在不停的更新，所以书里讲到的源码实现可能都和目前最新的脱轨了
- 所以看书的时候还是以了解为主，想理解现在真正的源码实现方式，当然还是要啃最新源码

## __strong修饰符实现

```objective-c
//自己持有
{
    id __strong obj = [NSObject alloc] init];//obj持有对象
}

id obj = objc_mesgSend(NSObject, @selector(alloc));
objc_msgSend(obj,@selector(init));
objc_release(obj);//超出作用域，释放对象

//非自己持有
{
    id __strong obj = [NSMutableArray array];
}

id obj = objc_msgSend(NSMutableArray, @selector(array));
objc_retainAutoreleasedReturnValue(obj);//objc_retainAutoreleasedReturnValue的作用：持有对象，将对象注册到autoreleasepool并返回。
objc_release(obj);

+ (id)array
{
   return [[NSMutableArray alloc] init];
}

+ (id)array
{
   id obj = objc_msgSend(NSMutableArray, @selector(alloc));
   objc_msgSend(obj,, @selector(init));
   return objc_autoreleaseReturnValue(obj);//objc_autoreleaseReturnValue:返回注册到autoreleasepool的对象。
}
```
### objc_retainAutoreleasedReturnValue与objc_autoreleaseReturnValue
#### 两个不一定
- objc_retainAutoreleasedReturnValue不一定非要持有注册到pool里的对象
- objc_autoreleaseReturnValue不一定非要注册到pool中

#### 解释

- 书上的图说的很清楚
  ![205617F1-C561-4157-BB78-B1EE275F7040](http://ww3.sinaimg.cn/large/006tNc79ly1g50cimney3j30r20bx796.jpg)

- 将这张图分成两部分去看

  - 首先是左侧三个灰色箭头代表的普通流程

    - object注册到pool中
    - 在pool中找到对象，返回
  - 然后是右侧三个黑色箭头构成的优化情况，就是当objc_autoreleaseReturnValue后直接objc_retainAutoreleasedReturnValue的情况
    - 直接就是objc_autoreleaseReturnValue获取对象
    - objc_retainAutoreleasedReturnValue持有返回的对象
  - 这样子，跳过了中间的pool这个中转站，实现优化

## __weak修饰符实现

### objc_storeWeak
- objc_storeWeak(&obj1, obj)会使用weak表来存储使用weak修饰符的变量的地址
- weak表同样也是哈希表，其key是对象的地址，value是附有weak修饰符变量的地址
- 如果第二参数是0的话，就把第一参数里的地址从weak表中删除
### 注册到autoreleasepool
```objective-c
id obj1;
objc_initWeak(&obj1,obj);//初始化附有__weak的变量
id tmp = objc_loadWeakRetained(&obj1);//取出附有__weak修饰符变量所引用的对象并retain
objc_autorelease(tmp);//将对象注册到autoreleasepool中
objc_destroyWeak(&obj1);//释放附有__weak的变量
```
- 这里注意，每次都是访问时，注册到pool里，每访问一次注册一次
- 因此书上推荐再将其赋值给一个strong

## __autoreleasing修饰符实现
```objective-c
id pool = objc_autoreleasePoolPush();//pool入栈
id obj = objc_msgSend(NSObject, @selector(alloc));
objc_msgSend(obj, @selector(init));
objc_autorelease(obj);
objc_autoreleasePoolPop(pool);//pool出栈
```

# 2019.7.22 更新：关于weak修饰符的一些打印实验

```objective-c
//在ARC中我们可以使用__bridge来查看应用计数

NSObject *obj0 = [[NSObject alloc] init];
        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj0))); 
        NSObject * __weak obj1 = obj0;
        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj1)));
        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj0)));
        
//        retain count = 1
//        retain count = 2
//        retain count = 1
```
- 这里我们可以对与weak修饰符有更深的了解，就如书上p.46页上说的
```objective-c
id  __weak obj1 = obj0;
NSLog(@"class = %@",[obj1 class]);

id __weak obj1 = obj0;
id __autoreleasing tmp = obj1;
NSLog(@"class = %@",[tmp class]);//实际访问的是注册到自动释放池的对象
```
- 这也是为什么我们第二次打印出来的retain count = 2，因为我们等同于将对象注册到了autoreleasepool中，因此引用计数+1
- 但是我们第三个打印又变回了1，这说明两件事
  - 不是一使用weak修饰符就会直接注册到pool中，是当你访问的时候才会生成一个__autoreleasing tmp，这也是为什么作者取变量名为tmp
  - 而接下来就变回1，说明当访问完之后就会直接释放，等同于release了，导致引用计数
  - 总结一下就是 ，weak修饰符之所以要生成tmp，只是为了防止该对象无人引用，会直接dealloc，因此使用一个tmp维护住它，当访问结束后，这个也就释放了