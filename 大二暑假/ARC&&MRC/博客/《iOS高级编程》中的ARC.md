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
