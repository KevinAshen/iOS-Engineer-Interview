[toc]

# 主演

- J先生：
  - 简介：单身帅气，热情潇洒，热爱iOS开发，目前在努力集齐苹果全家桶
  - GitHub：[KevinAshen](https://github.com/KevinAshen)
  - CSDN：[太阳骑士索拉尔](https://blog.csdn.net/KevinAshen)
  - 简书：[太阳骑士索拉尔](https://www.jianshu.com/u/0de1c21e79ca)
- J先生：
  - 简介：单身帅气，热情潇洒，热爱iOS开发，目前在努力集齐苹果全家桶
  - GitHub：[KevinAshen](https://github.com/KevinAshen)
  - CSDN：[太阳骑士索拉尔](https://blog.csdn.net/KevinAshen)
  - 简书：[太阳骑士索拉尔](https://www.jianshu.com/u/0de1c21e79ca)
- 说明：
	- 学无止境

## Category相关

### load、initialize的区别，以及它们在category重写的时候的调用的次序

- 区别在于调用方式和调用时刻
- 调用方式：
  - load是根据函数地址直接调用
  - initialize是通过objc_msgSend调用
- 调用时刻：
  - load是runtime加载类、分类的时候也就是程序刚运行的时候调用（只会调用1次）
  - initialize是类第一次接收到消息的时候调用，每一个类只会initialize一次（父类的initialize方法可能会被调用多次）
- 调用顺序：
  - 先调用类的load方法，再调用分类的load方法，先编译哪个类，就先调用哪个类的load。在调用load之前会先调用父类的load方法。分类中load方法不会覆盖本类的load方法，先编译的分类优先调用load方法。
  - initialize先初始化父类，之后再初始化子类。如果子类没有实现+initialize，会调用父类的+initialize（所以父类的+initialize可能会被调用多次），如果分类实现了+initialize，就覆盖类本身的+initialize调用。

### 如何给Category添加属性?

- 使用关联对象的特性
- 其本质是，runtime维护了一个全局的哈希Map，存储着所有设定好的的关联对象，在这其中使用对象的地址作为key，关联对象的地址作为value进行存储，两者建立可以查找的联系
- 使用到的API：
  - objc_setAssociatedObject(id _Nonnull object, const void * _Nonnull key, id _Nullable value, objc_AssociationPolicy policy), OBJC_AVAILABLE(10.6, 3.1, 9.0, 1.0, 2.0);
    - OBJC_AVAILABLE可以设置存储策略，也就相当于属性关键字
  - objc_getAssociatedObject(id _Nonnull object, const void * _Nonnull key), OBJC_AVAILABLE(10.6, 3.1, 9.0, 1.0, 2.0);
  - objc_removeAssociatedObjects(id _Nonnull object), OBJC_AVAILABLE(10.6, 3.1, 9.0, 1.0, 2.0);

## 属性关键字相关

### 是不是使用了atomic就一定多线程安全呢？

  - 当使用atomic时，虽然对属性的读和写是原子性的，但是仍然可能出现线程错误：当线程A进行写操作，这时其他线程的读或者写操作会因为该操作而等待。当A线程的写操作结束后，B线程进行写操作，然后当A线程需要读操作时，却获得了在B线程中的值，这就破坏了线程安全，如果有线程C在A线程读操作前release了该属性，那么还会导致程序崩溃。所以仅仅使用atomic并不会使得线程安全，我们还要为线程添加lock来确保线程的安全
  - atomic所说的线程安全只是保证了getter和setter存取方法的线程安全，并不能保证整个对象是线程安全的
  - 举例：
    - @ property (atomic, strong) NSMutableArray *arr;
    - 如果一个线程循环的读数据，一个线程循环写数据，那么肯定会产生内存问题，因为这和setter、getter没有关系。如使用[self.arr objectAtIndex:index]就不是线程安全的。好的解决方案就是加锁

### weak和assign的区别：

  - assgin：修饰基础类型，直接赋值，不改变引用计数，
    - 如果修饰对象，当对象销毁时，属性对象指针还是指向该块内存，出现悬垂指针的问题
  - weak：修饰OC对象，不改变引用计数，表示一种非持有关系
    - 对象销毁时，weak指针修饰的属性对象的指针会自动指向nil，不会出现访问悬垂指针错误

### @ synthesize 和 @ dynamic 分别有什么作用

  - @ property 有两个对应的词,一个是@synthesize,一个是@dynamic。如果@synthesize 和@dynamic 都没写,那么默认的就是，@ syntheszie var = _var;
  - @ synthesize 的语义是使用一个实例变量绑定到该属性
    - 情形1:不使用@synthesize，可以使用自动生成的带下划线的实例变量名
    - 情形2:使用@synthesize为属性添加带下划线的别名，与不使用@synthesize相同
    - @synthesize 声明的属性=变量。意思是，将属性的setter,getter方法，作用于这个变量。@synthesize还有一个作用，就是可以指定与属性对应的实例变量，例如我可以这样写@synthesize age = myAge;，那这样子的话我们去调用的时候self.age其实是操作的实例变量myAge,而不是_age了。
    - 情形3:使用@synthesize为属性添加任意别名，此时使用自动生成的实例变量名将报错，只能使用指定的别名。
  - @dynamic 告诉编译器:属性的 setter 与 getter 方法由用户自己实现,不自动生成。(当然对于 readonly 的属性只需提供 getter 即可)
  - 注意点：
    - @ synthesize 的作用:是为属性添加一个实例变量名，或者说别名。同时会为该属性生成 setter/getter 方法。
    - 禁止@synthesize:如果某属性已经在某处实现了自己的 setter/getter ,可以使用 @dynamic 来阻止 @synthesize 自动生成新的 setter/getter 覆盖。
    - 内存管理：@synthesize 和 ARC 无关。
    - 使用：一般情况下无需对属性添加 @synthesize ，但一些特殊情形仍然需要，例如protocol中声明的属性
  - 例子：
    - 假如一个属性被声明为@dynamic var；然后你没有提供@setter 方法和@getter 方法,编译的时候没问题,但是当程序运行到 instance.var = someVar,由于缺 setter方法会导致程序崩溃;
    - 或者当运行到 someVar = instance.var 时,由于缺 getter 方法同样会导致崩溃。
  - 编译时没问题,运行时才执行相应的方法,这就是所谓的动态绑定

### ARC 下,不显式指定任何属性关键字时,默认的关键字都有哪些？

  - 基本数据： atomic,readwrite,assign
  - 普通的 OC 对象: atomic,readwrite,strong

### @ synthesize 合成实例变量的规则是什么?

  - 规则：
    - 如果指定了成员变量的名称,会生成一个指定的名称的成员变量如果这个成员已经存在了就不再生成了。
    - 如果指定@synthesize foo;就会生成一个名称为 foo 的成员变量,也就是说:会自动生成一个属性同名的成员变量。

- 在有了自动合成属性实例变量之后,@ synthesize 还有哪些使用场景?

  - 不会自动添加@synthesize的情况：
    - 同时重写了 setter 和 getter 时
    - 重写了只读属性的 getter 时
    - 使用了@dynamic 时
  - 使用场景：
    - 在 @ protocol 中定义的所有属性在
    - category 中定义的所有属性重载的属性
    - 当你在子类中重载了父类中的属性,必须使用@synthesize 来手动合成ivar

### 怎么用copy关键字？

  - NSString、NSArray、NSDictionary 等等经常使用 copy 关键字,是因为他们有对应的可变类型:NSMutableString、NSMutableArray、NSMutableDictionary。为确保对象中的属性值不会无意间变动,应该在设置新属性值时拷贝一份,保护其封装性
  - 在block中也会使用copy关键字，,在 MRC 中,方法内部的 block 是在栈区的,使用 copy 可以把它放到堆区。在ARC时代，对于 block 使用 copy 还是 strong 效果是一样的,但是建议写上 copy,因为这样显示告知调用者“编译器会自动对 block 进行了 copy 操作

- 用@property 声明的 NSString(或 NSArray,NSDictionary)经常使用 copy 关键字,为什么?如果改用 strong 关键字,可能造成什么问题?

  - 因为父类指针可以指向子类对象,使用 copy 的目的是为了让本对象的属性不受外界影响,使用 copy 无论给我传入是一个可变对象还是不可对象,我本身持有的就是一个不可变的副本.
  - 如果我们使用是 strong,那么这个属性就有可能指向一个可变对象,如果这个可变对象在外部被修改了,那么会影响该属性

### 这个写法会出什么问题: @ property(copy)NSMutableArray *array

  - 我们使用copy方法修饰可变对象，我们持有的将会是一个不可变的副本，此时我们如果那这个不可变副本去调用可变对象才能调用的增删改查这类操作，会因为我们没有实现这些方法而报错

### 如何让自定义类可以用 copy 修饰符?如何重写带 copy 关键字的 setter?

  - 遵守NSCopying协议

  - 实现协议里面的- (id)copyWithZone:(NSZone *)zone

    - 举例：

      ![img](https://tva1.sinaimg.cn/large/006y8mN6ly1g91fj6ghejj31100cin0c.jpg)

  - 重写setter方法：

    - 例子：
      // 重写带 copy 关键字的 setter- (void)setName:(NSString *)name {  _name = [name copy];}