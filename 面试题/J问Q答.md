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
	- 由于女主演罢工，本栏目被迫从男女知识问答互动节目改成沉闷单身男士自言自语节目
	- 参照节目从《国光帮帮忙》变成《晓说》
	- 不说了，眼泪要留下来了，各位观众不妨点个关注，加个赞，看看节目会进化成什么模式😢
# 主演

- J先生：
  - 简介：单身帅气，热情潇洒，热爱iOS开发，目前在努力集齐苹果全家桶
  - GitHub：[KevinAshen](https://github.com/KevinAshen)
  - CSDN：[太阳骑士索拉尔](https://blog.csdn.net/KevinAshen)
  - 简书：[太阳骑士索拉尔](https://www.jianshu.com/u/0de1c21e79ca)
- Q小姐：
  - 简介：女
  - GitHub：[macoooo](https://github.com/macoooo)
  - CSDN：[qiangshuting](https://blog.csdn.net/qiangshuting)
  - 简书：[阡陌啦啦啦](https://www.jianshu.com/u/5bfc6017c805)

# GCD

## 串行队列先异步后同步

### Question

```objective-c
    dispatch_queue_t serialQueue = dispatch_queue_create("test", DISPATCH_QUEUE_SERIAL);
    
    NSLog(@"1");
    
    dispatch_async(serialQueue, ^{
        
         NSLog(@"2");
    });
    
    NSLog(@"3");
    
    dispatch_sync(serialQueue, ^{
        
        NSLog(@"4");
    });
    
    NSLog(@"5");
```

### Answer

- 顺序是13245
- 首先这段代码的背景就是在主队列中，打印完1后，进入任务2，由于是异步添加的任务，所以不会阻塞线程，主线程直接继续往下进行；打印完3后，到了任务4，此时任务还是添加到的任务2那个串行队列里，由于任务4是同步添加的，同步添加的特点就是必须要等待添加的任务执行完之后才能继续进行下去
- 而此时在该串行队列里面此时有两个任务，一个是任务4，一个是任务2，由于任务4是一个同步任务，它会阻塞主线程，必须执行完任务4才能继续往下执行
- 而队列又是先进先出的，任务2在任务4前面，所以会先执行任务2，再执行任务4
- 最后执行任务5
- 如果任务4是添加到另一个串行队列或者并行队列，则任务2和任务4无序执行(可以添加多个任务看效果)

## performSelector与GCD

### Question

```objective-c
dispatch_async(dispatch_get_global_queue(0, 0), ^{
        
        [self performSelector:@selector(test:) withObject:nil afterDelay:0];
    });
```

### Answer

- 这里的test方法是不会去执行的，原因在于performSelector方法必须要创建提交任务到runloop，而gcd底层创建的线程是默认没有开启对应runloop的，所有这个方法就会失效
- 如果将dispatch_get_global_queue改成主队列，由于主队列所在的主线程是默认开启了runloop的，就会去执行(将dispatch_async改成同步，因为同步是在当前线程执行，那么如果当前线程是主线程，test方法也是会去执行的)

## Category&&实例变量

### Question

- 为什么Category不能添加实例变量

### Answer

- 首先，编译器会按照实例变量的顺序，从对象的内存区域的起始地址开始，存储每一个实例变量，由于对象的布局在编译期就决定了，而Category是在运行期加载，所以不能添加实例变量。而Category添加属性，并没有合成setter,getter方法，实例变量，只是有两个方法声明

## OC怎么调用方法的

### Question

- 描述OC调用方法的全过程

### Answer

- 消息发送阶段：
  - 调用了objc_msgSend函数【objc_msgSend(id _Nullable self, SEL _Nonnull op, ...)消息接收者，方法名，其他参数】
  - 根据isa指针查到该对象对应的类对象
    - 查看该类对象的缓存方法列表cache，如果没找到，转入查找class_rw_t的方法列表
    - 查看class_rw_t【运行期决定的】中的方法列表，如果没找到，查找该类对象的父类；如果找到了，将该方法存入类对象的方法缓存列表
  - 在父类中重复上述过程，其中如果在父类class_rw_t中的方法列表中找到了该方法依然会存放在最初的类对象的缓存中
  - 如果一直找到根类NSObject，依然没有找到该方法，就会进入消息转发阶段
- 三次拯救：
  - 动态方法解析阶段
    - 动态方法解析【resolveInstanceMethod】
  - 消息转发阶段
    - 备用接受者【(id)forwardingTargetForSelector:(SEL)aSelector】
    - 完整转发【(void)forwardInvocation:(NSInvocation )anInvocation】
  - 这三种方式都必须手动重写才能开启，三个方法会依此执行，三个方法的关系是递进的，并且这三个方法是递进的，前一个实现了就不会走后一个，同时越往后处理的代价越大
  - 同时这三次拯救也会顺着父类继承链往上走
- 抛出错误

## 多继承

### Question

- OC有多继承吗？能用什么方式模拟多继承？

### Answer

- 通过组合实现“多继承”

  - 其实就是在子类中去创建父类对象，外部调用时让创建的父类对象去调用父类方法，实现多继承
  - 【缺点：需要额外创建父类对象】
- 通过协议实现“多继承”

  - OC规定一个类可以遵守多个协议，我们可以通过协议的方式模拟多继承。但是协议只能提供接口，不能提供实现，所以我们要在子类里面添加对应的实现
  - 【缺点：需要自己在子类里面添加对应的实现】
- 通过Category

  - 单独使用Category不能实现多继承，因为Category的用处只是给类添加几个可以调用的方法
  - 【缺点：只能添加父类同名方法，且需要自己实现】
- 快速消息转发【备用接受者】【forwardingTargetForSelector】
  - 通过重写forwardingTargetForSelector方法，可以把没有实现的方法转发给指定类来执行，从而实现多继承
  - 【缺点：需要消耗比较大】

## 自动释放池的应用场景

### Question

- 自动释放池的应用场景

### Answer
- 对象作为方法返回值

  - 当一个对象要作为函数返回值的时候，本来应该在方法返回时释放该对象，但是为了能正常使用该对象，就可以使用autorelease延迟释放的特性，将其在返回之前做一次autorelease，加入到自动释放池中，保证可以被返回
  - 在一次runloop之后系统会帮我们释放他

- 临时生成大量对象,一定要将自动释放池放在for循环里面，要释放在外面，就会因为大量对象得不到及时释放，而造成内存紧张，最后程序意外退出

  - ```objective-c
    for (int i = 0; i<10000; i++) {
            @autoreleasepool {
                UIImageView *imegeV = [[UIImageView alloc]init];
                imegeV.image = [UIImage imageNamed:@"efef"];
                [self.view addSubview:imegeV];
            }
        }
    ```
## autorelease的优化

### Question

- 在ARC下，对autorelease有什么优化？

### Answer

- 拆分成了两个函数，objc_autoreleaseReturnValue与objc_retainAutoreleasedReturnValue
- objc_autoreleaseReturnValue调用时，会先判断有没有紧接着调用objc_retainAutoreleasedReturnValue
  - 判断是通过__builtin_return_address这个内建函数return address，他可以告诉我们函数返回地址，根据偏移量可以推算新的下一个被调用的函数
- 如果是的话返回对象再注册到自动释放池中，而是存储在TLS中，做一个优化标记
  - 优化策略TLS(线程局部存储),就是为每个线程单独分配一块栈控件。以key-value的形式对数据进行存储（ARC对autorelease对象的优化标记）
- 在外部接收这个返回值的objc_retainAutoreleasedReturnValue（）会先在TLS中查询有没有这个对象，如果有，那么就直接返回这个对象（不调用retain）
- 使用这两个函数通过TSL进行了一次中转，省下了autorelease和retain的步骤，在一定程度上达到了最优化

## id相关

### id与instancetype的区别

- 知识补充之返回关联类型：

  - 描述：

    - 类方法中alloc，new开头
    - 实例方法中autorelease，init，retain，self开头

  - 代码：

    ```objective-c
    for example
    @interface NSObject
    +(id)alloc;
    -(id)init;
    @end
    ```

  - 特点：

    - 会返回方法所在类的类型的对象，即关联返回类型的方法
    - 这些方法的返回结果以方法所在的类为类型

  - 举例：

    - 返回关联类型代码：

      NSArray*array = [[NSArray alloc] init];

      - [NSArray alloc]返回类型属于关联返回类型，NSArray *

    - 非返回关联类型代码：

      ```objective-c
      // 定义
      @interface NSArray
      +(id)constructAnArray;
      @end
      // 调用
      [NSArray constructAnArray]
      ```

      - 得到返回类型和方法声明的返回类型一样，是id

- 引入instancetype的作用就在于能使得非关联返回类型的方法返回所在类的类型

- 比如对于[NSArray constructAnArray]，我们得到的返回值类型就会是NSArray *

- 好处：确定对象类型帮编译器更好定位代码问题

- 相同点：

  - 都可以作为方法的返回类型

- 不同点：

  - instancetype 可以返回和方法所在类相同类型的对象，id只能返回未知类型的对象。
  - instancetype只能作为返回值，id可以作为参数.
  - instancetype在编译的时候可以判断对象的真实类型

### id与NSObject *的区别

- NSObject *
  - 静态数据类型，会惊醒编译时检查（需要强制类型转换）
  - NSObject *无法管理到NSProxy
- id
  - 动态数据类型，不需要强制类型转换，可以直接使用，编译器不会检查其类型
  - id是万能指针，可以代表一切OC对象

### id的作用

- 可以用于动态绑定。编译器看到id以后，认为是动态类型，不再检查类型
- 多态, 可以减少代码量, 避免调用子类特有的方法需要强制类型转换
- 可以用于代理(delegate)。在声明代理的时候，根本不用关心代理的类型是什么
- 有的时候，你不知道变量的类型，就可以使用id。比如，在一个存储着NSString和NSNumber和数组中，想要找出所有的字符串。由于不确定取出的元素是哪种类型，就可以使用id进行声明，然后再判断类型

## 数据类型

### 静态数据类型与动态数据类型

- 静态数据类型
  - 特点:
    - 在编译时就知道变量的类型,
    - 知道变量中有哪些属性和方法
    - 在编译的时候就可以访问这些属性和方法,
    - 如果是通过静态数据类型定义变量, 如果访问不了属于静态数据类型的属性和方法, 那么编译器就会报错
- 动态数据类型
  - 特点:
    - 在编译的时候编译器并不知道变量的真实类型, 只有在运行的时候才知道它的真实类型
    - 如果通过动态数据类型定义变量, 如果访问了不属于动态数据类型的属性和方法, 编译器不会报错
  - 弊端：
    - 由于动态数据类型可以调用任意方法, 所以有可能调用到不属于自己的方法, 而编译时又不会报错, 所以可能导致运行时的错误
  - 应用场景: 
    - 多态, 可以减少代码量, 避免调用子类特有的方法需要强制类型转换
  - 使用须知：
    - 为了避免动态数据类型引发的运行时的错误, 一般情况下如果使用动态数据类型定义一个变量, 在调用这个对象的方法之前会进行一次判断, 判断当前对象是否能够调用这个方法

# 设计模式

## KVO与通知的区别

### Question

- KVO
  - 一对一的监听
  - 只能监听属性
  - 步骤为：注册--响应--移除
- 通知？？？
  - 一对多监听
  - 可以监听方法，属性，等等
  - 步骤为：注册、接收通知(addObserver)--->发送通知(postNotification)--->响应--->移除通知(removeObserver)
  - 可以通过NSNotificationQueue实现异步通知，不在阻塞主线程

### Answer