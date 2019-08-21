[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文主要涉及到KVO的简单使用以及底层原理
- 感谢涂神精妙的图

# 使用以及注意点

## 三个方法

### addObserver:forKeyPath:options:context:

- 方法注册观察者，观察者可以接收keyPath属性的变化事件
- @observer:就是观察者，是谁想要观测对象的值的改变。
- @keyPath:就是想要观察的对象属性。
- @options:options一般选择NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld，这样当属性值发生改变时我们可以同时获得旧值和新值，如果我们只填NSKeyValueObservingOptionNew则属性发生改变时只会获得新值。
- @context:想要携带的其他信息，比如一个字符串或者字典什么的。往往用来区分标志。
- 这里要注意的是比如[A addObserver:B forKeyPath:@"name" options:context: ]
- 这么一句话的意思是，给A添加一个观察者B，B负责观察A的name属性
- 要分清楚存在观察对象和观察者两个东西

### observeValueForKeyPath:ofObject:change:context:
- 当keyPath属性发生改变后，KVO会回调这个方法来通知观察者
- @keyPath:观察的属性
- @object:观察的是哪个对象的属性
- @change:这是一个字典类型的值，通过键值对显示新的属性值和旧的属性值
- @context:上面添加观察者时携带的信息
- 这里注意这个方法是属于观察者的，在添加时，应该写在观察者的类里面，同样是上面的例子[A addObserver:B forKeyPath:@"name" options:context: ]就应该把该方法的写在B里面

### removeObserver:forKeyPath:
- 移除观察者
- @observer:就是观察者，是谁想要观测对象的值的改变。
- @keyPath:就是想要观察的对象属性。
- 添加观察者后一定更要记得移除

## 实操见真章
- 我写了这样一个Demo

![8FA8B5E2-B8A7-4306-81FE-136642B74C1C](http://ww3.sinaimg.cn/large/006tNc79ly1g5r8of4jj8j30br0nawez.jpg)

- 四个按钮一共有四个相应的点击事件，分别是添加观察者，移除观察者，改变属性值，将观察者置nil

```objective-c
- (void)clickButton:(UIButton *)sender {
    NSLog(@"CLICK");
    if (sender.tag == 101) {
        NSLog(@"CLICK1");
        [self addObserver:self.kvoPerson forKeyPath:@"testStr" options:NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew context:nil];
    } else if (sender.tag == 102) {
        NSLog(@"CLICK2");
        [self removeObserver:self.kvoPerson forKeyPath:@"testStr"];
    } else if (sender.tag == 103) {
        NSLog(@"CLICK3");
        self.testStr = @"QSTSD";
    } else {
        _kvoPerson = nil;
    }
}
```

- 下面就是我左点右点试出来的crash以及一些特性
  1. add与remove成对出现，也就是说add了两次就可以remove两次
  2. 如果remove超过add，会出现crash
  3. 添加了观察者，如果没有实现observeValueForKeyPath，在改变属性的时候，就会crash
  4. 添加了观察者，将观察者置nil，改变属性，crash
  5. 此外，只要调用了属性的set方法就会触发KVO，哪怕前后的值一样，这一点有好有坏，下面说怎么解决
- 因此Apple官方会建议把添加观察者写在viewDidLoad里，将移除操作写在dealloc里面

# 俺也整一个【自己实现KVO】
- 虽然叫自己实现KVO，但本质上就多了一点操作，要调用的还是KVO相关的方法

```objective-c
//第一步：我们需要认识下面这个方法，如果想要手动调用或自己实现KVO需要重写该方法该方法返回YES表示可以调用，返回NO则表示不可以调用。
+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)theKey {
    BOOL automatic = NO;
    if ([theKey isEqualToString:@"name"]) {
        automatic = NO;//对该key禁用系统自动通知，若要直接禁用该类的KVO则直接返回NO；
    }
    else {
        automatic = [super automaticallyNotifiesObserversForKey:theKey];
    }
    return automatic;
}

//第二步：我们需要重写setter方法
- (void)setName:(NSString *)name {
    if (name != _name) {
        [self willChangeValueForKey:@"name"];
        _name = name;
        [self didChangeValueForKey:@"name"];
    }
}
```

- 这样我们就完成了手动控制KVO的时机，并且可以看到我们在这里实现了优化，此时只有复制结果不一样才会走KVO

# 实现原理

- KVO的实现原理要涉及到runtime里的isa方面的知识，不知道的读者可以翻看下我以前的博客

- `KVO`是通过isa 混写(`isa-swizzling`)技术实现的。在运行时根据原类创建一个中间类，这个中间类是原类的子类，并动态修改当前对象的`isa`指向中间类。并且将`class`方法重写，返回原类的`Class`。所以苹果建议在开发中不应该依赖`isa`指针，而是通过`class`实例方法来获取对象类型。

- 下面来分析下这个流程

## 测试代码

```objective-c
NSKeyValueObservingOptions option = NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew;
    
NSLog(@"person1添加KVO监听对象之前-类对象 -%@", object_getClass(self.person1));
NSLog(@"person1添加KVO监听之前-方法实现 -%p", [self.person1 methodForSelector:@selector(setAge:)]);
NSLog(@"person1添加KVO监听之前-元类对象 -%@", object_getClass(object_getClass(self.person1)));
    
[self.person1 addObserver:self forKeyPath:@"age" options:option context:@"age chage"];
    
NSLog(@"person1添加KVO监听对象之后-类对象 -%@", object_getClass(self.person1));
NSLog(@"person1添加KVO监听之后-方法实现 -%p", [self.person1 methodForSelector:@selector(setAge:)]);
NSLog(@"person1添加KVO监听之后-元类对象 -%@", object_getClass(object_getClass(self.person1)));

//打印结果
KVO-test[1214:513029] person1添加KVO监听对象之前-类对象 -Person
KVO-test[1214:513029] person1添加KVO监听之前-方法实现 -0x100411470
KVO-test[1214:513029] person1添加KVO监听之前-元类对象 -Person
  
KVO-test[1214:513029] person1添加KVO监听对象之后-类对象 -NSKVONotifying_Person
KVO-test[1214:513029] person1添加KVO监听之后-方法实现 -0x10076c844
KVO-test[1214:513029] person1添加KVO监听之后-元类对象 -NSKVONotifying_Person
  
//通过地址查找方法
(lldb) p (IMP)0x10f24b470
(IMP) $0 = 0x000000010f24b470 (KVO-test`-[Person setAge:] at Person.h:15)
(lldb) p (IMP)0x10f5a6844
(IMP) $1 = 0x000000010f5a6844 (Foundation`_NSSetLongLongValueAndNotify)
```

- 上面代码中虽然元类对象和类对象名字都叫Person，但是他们地址不一样，确实一个是元类一个是类对象
- 我们会发现，在添加观察者之前我们的person很合理，但在添加后，他的所属类也就是isa的指向变成了NSKVONotifying_Person
- 并且调用的setAge:方法变成了_NSSetLongLongValueAndNotify方法

## NSKVONotifying_Person

- `KVO`会在运行时动态创建一个新类，将对象的`isa`指向新创建的类，新类是原类的子类，命名规则是`NSKVONotifying_xxx`的格式。
- 在最开始，没有添加KVO监听的时候，对象和类的关系如下

![img](http://ww1.sinaimg.cn/large/006tNc79ly1g5rd79e75lj31700sugoj.jpg)

- 使用KVO监听对象后，对象和类对象之间会添加一个中间对象

![img](http://ww1.sinaimg.cn/large/006tNc79ly1g5rdel2fc0j317u0u043e.jpg)

- 总结下：
  1. 最开始person实例对象isa指向Person类对象
  2. 添加KVO后，在中间添加了一个临时类对象NSKVONotifying_Person
  3. person实例对象isa指针变成指向NSKVONotifying_Person
  4. NSKVONotifying_Person继承于Person
  5. 而NSKVONotifying_Person与Person的isa指针指向他们各自的元类
  6. 在这个NSKVONotifying_Person只会添加被监听的属性的set方法【其余方法都不添加，还是使用superClass去原来的Class里找】
  7. 改变class方法，让person实例对象返回的类对象依然是Person，防止暴露中间类
  8. _isKVO返回YES

## setter方法内部实现

- `KVO`会重写`keyPath`对应属性的`setter`方法，没有被`KVO`的属性则不会重写其`setter`方法。在重写的`setter`方法中，修改值之前会调用`willChangeValueForKey:`方法，修改值之后会调用`didChangeValueForKey:`方法，这两个方法最终都会被调用到`observeValueForKeyPath:ofObject:change:context:`方法中。

## 几个注意点

- isa混写之后如何调用方法
  1. 调用**监听的属性设置方法**，如 `setAge:`，都会先调用 `NSKVONotify_Person` 对应的属性设置方法；
  2. 调用**非监听属性设置方法**，如 `test`，会通过 `NSKVONotify_Person` 的 `superclass`，找到 `Person` 类对象，再调用其 `[Person test]` 方法
- 为什么重写`class`方法
  - 如果没有重写`class`方法,当该对象调用`class`方法时,会在自己的方法缓存列表,方法列表,父类缓存,方法列表一直向上去查找该方法,因为`class`方法是`NSObject`中的方法,如果不重写最终可能会返回`NSKVONotifying_Person`,就会将该类暴露出来,也给开发者造成困扰,写的是`Person`,添加KVO之后`class`方法返回怎么是另一个类。
- _isKVOA有什么作用
  - 这个方法可以当做使用了`KVO`的一个标记，系统可能也是这么用的。如果我们想判断当前类是否是`KVO`动态生成的类，就可以从方法列表中搜索这个方法。

# 全流程图

![687474703a2f2f7777322e73696e61696d672e636e2f6c617267652f303036744e6337396c793167357132676a316834736a333165363075307161382e6a7067](http://ww2.sinaimg.cn/large/006tNc79ly1g5re5kvpdyj31e60u045l.jpg)

- 会在didChange方法里面就会触发回调
- longlongValueAndNotify()，longlong是通配符