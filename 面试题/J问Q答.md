[toc]

# 主演

- J先生：
  - GitHub：[KevinAshen](https://github.com/KevinAshen)
  - CSDN：[太阳骑士索拉尔](https://blog.csdn.net/KevinAshen)
  - 简书：[太阳骑士索拉尔](https://www.jianshu.com/u/0de1c21e79ca)
- Q小姐：
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

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

# RunTime

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
- 消息转发阶段
  - 动态方法解析【resolveInstanceMethod】
  - 备用接受者【(id)forwardingTargetForSelector:(SEL)aSelector】
  - 完整转发【(void)forwardInvocation:(NSInvocation )anInvocation】
  - 这三种方式都必须手动重写才能开启，三个方法会依此执行，三个方法的关系是递进的，并且这三个方法是递进的，前一个实现了就不会走后一个，同时越往后处理的代价越大
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
- 目前不太能明白多继承的意义是什么

## 自动释放池的应用场景

### Question

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

### Answer

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

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer

## xx

### Question



### Answer