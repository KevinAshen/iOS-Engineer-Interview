[toc]

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