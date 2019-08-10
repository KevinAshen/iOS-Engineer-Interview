[TOC]
# 关于我的仓库

- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文主要是对于《高级编程》类似于总结的学习笔记

# 准备工作 
- 阅读《Objective-C 高级编程》中的p.138 ~ 175

# 什么是多线程以及GCD做了什么

## 多线程的定义

### 补充知识：进程与线程

- 系统中正在运行的每一个应用程序都是一个进程，每个进程系统都会分配给它独立的内存运行。放到我们的iOS系统中，每一个应用都是一个进程。确切的来说，当一个程序进入内存运行，即变成一个进程，进程是处于运行过程中的程序，并且具有一定独立功能。
- 一个进程的所有任务都在线程中进行，因此每个进程至少要有一个线程，也就是主线程。同一类线程共享代码和数据空间，每个线程有独立的运行栈和程序计数器(PC)，线程切换开销小。它是程序执行流的最小单元，是进程中的一个实体，是执行程序最基本的单元，有自己栈和寄存器。
- 　线程和进程一样分为五个阶段：创建、就绪、运行、阻塞、终止。多进程是指操作系统能同时运行多个任务（程序）。多线程是指在同一程序中有多个顺序流在执行。

### 补充知识：CPU，CPU核以及时间片

- 在用户启动程序后，CPU会从程序指定的地址开始，一个一个执行CPU命令列
- 这样的执行是一条无分叉的路径【可能会迂回，因为有函数调用】，但这意味着一个CPU一次只能执行一个命令，不能执行某处分开的并列的两个命令，执行也不会出现分歧

![9E60A594-2ED9-433A-A4EB-74FB1DA49B10](http://ww4.sinaimg.cn/large/006tNc79ly1g5s3zd9zsbj30fc0qkgqk.jpg)

- 这样一条无分叉路径本质上就是一个线程，这样来看似乎多线程技术无从谈起，因为一个CPU无法同时做两件事【道路不会有分叉】
- 但是尽管道路没有分叉，但我们却可以有很多这样的道路，让一个CPU在不同的道路上左右横跳😂

![20F74E5E-FF07-4CA2-88F2-B44077D834E7](http://ww4.sinaimg.cn/large/006tNc79ly1g5s477vue2j31es0niwo6.jpg)

- 这就是“虚假的多线程”，而时间片即CPU分配给各个程序的时间，每个线程被分配一个时间段，称作它的时间片，即该进程允许运行的时间，使各个程序从表面上看是同时进行的。如果在时间片结束时进程还在运行，则CPU将被剥夺并分配给另一个进程。如果进程在时间片结束前阻塞或结束，则CPU当即进行切换。而不会造成CPU资源浪费。在宏观上：我们可以同时打开多个应用程序，每个程序并行不悖，同时运行。但在微观上：由于只有一个CPU，一次只能处理程序要求的一部分，如何处理公平，一种方法就是引入时间片，每个程序轮流执行。
- 现在再引入CPU核概念：虽然CPU一个机器只有一个，但是我们在里面集成多个CPU核，计算核心，每个CPU核一次能够执行的CPU命令始终为1
- 由于集成了多个CPU核，现在我们的多线程就不是看起来像多线程那么简单了，而是真正实现了多线程
- 也就是实现从并发到并行

![687474703a2f2f7777342e73696e61696d672e636e2f6c617267652f303036744e6337396c7931673536633866667971766a33306d313066393075652e6a7067](http://ww1.sinaimg.cn/large/006tNc79ly1g5shifnzs8j30m10f9mxm.jpg)

### 多线程利弊

- 我们的CPU核虽然有多个，但他们的资源依然是共享的，多个线程之间还是会存在竞争关系，出现各种问题

![99A3CA0E-F480-4285-B549-C9F47285282B](http://ww1.sinaimg.cn/large/006tNc79ly1g5sei2ha2wj30o40akadn.jpg)

- 但我们依然要使用多线程，因为对于一个复杂的程序，肯定会有主线程【用户看得到的UI界面】，以及负责处理后台的事物的其他线程
- 放在iOS里面，我们通过主线程来描绘用户界面，触摸屏幕事件，假如把后台下载等工作都放在主线程去进行，显然就会导致主线程RunLoop堵塞，应用程序画面长时间停留

![08A8A176-481C-45F7-8552-58107F56D185](http://ww4.sinaimg.cn/large/006tNc79ly1g5shd44uohj30gl0jz792.jpg)

# GCD中的API

## Dispatch Queue

- 开发者要做的仅仅是定义像执行的任务并追加到适当的Dispatch Queue中
- 队列（Dispatch Queue）：这里的队列指执行任务的等待队列，即用来存放任务的队列。队列是一种特殊的线性表，采用 FIFO（先进先出）的原则，即新任务总是被插入到队列的末尾，而读取任务的时候总是从队列的头部开始读取。每读取一个任务，则从队列中释放一个任务。
- Serial Dispatch Queue【串行队列】：每次只有一个任务被执行。让任务一个接着一个地执行。（只开启一个线程，一个任务执行完毕后，再执行下一个任务）
- 并发队列（Concurrent Dispatch Queue）【并行队列】：可以让多个任务并发（同时）执行。（可以开启多个线程，并且同时执行任务）

![EC6A44D2-503A-4920-AF2C-DDCF5D865189](http://ww4.sinaimg.cn/large/006tNc79ly1g5t7rd8kgjj318v0u0kam.jpg)

![0C180D41-2CF4-450E-9209-6AA8E9CF2A20](http://ww1.sinaimg.cn/large/006tNc79ly1g5t7rrczipj31ba0sytpn.jpg)

- 这里进行并行的是队列，也就是说放在队列里的任务无需等待，这是一个抽象的概念；而我们的CPU核是实打实存在的，它的处理能力是定死的，就是同一时间只能处理一个任务，它是一个任务完成后进行下一个
- 因此能实现并行靠的是多线程同步进行

![8767B4F1-35CD-4850-A764-5BD3B20FEEBB](http://ww3.sinaimg.cn/large/006tNc79ly1g5t7x4qawej30oa0agtdj.jpg)

## dispatch_queue_create

- iOS6之前，需要进行手动管理。现已纳入ARC管理范围
- queue从前为C语言指针，现在为OC对象
- 该方法两个参数：
  - 指定生成返回的Dispatch Queue的名称，该名称在Xcode和Instruments的调试器中作为Dispatch Queue的名称表示，该名称也会出现在程序崩溃时所生成的CrashLog中；队列的名称推荐使用应用程序 ID 这种逆序全程域名。
  - 第二个参数用来识别是串行队列还是并发队列。DISPATCH_QUEUE_SERIAL/NULL 表示串行队列，DISPATCH_QUEUE_CONCURRENT 表示并发队列。
- 串行队列可以通过开多个队列，由于一个串行队列对应一个线程，因此多开队列可以形成类似于并行队列的效果；但这样很可能消耗大量的内存，降低系统的响应性能，而通过并行队列，由于是通过XNU内核进行的线程管理，就不会出现这样的问题

![04A13A2C-EAA5-4A37-AD45-713B7395E6B9](http://ww1.sinaimg.cn/large/006tNc79ly1g5t8dpsr2wj31e60u0kdy.jpg)

- 我们使用串行队列来防止多个线程更新相同资源导致数据竞争

![331F2D9E-06BC-4883-87DE-33472A4D22CE](http://ww3.sinaimg.cn/large/006tNc79ly1g5t8easxu2j31cc0p8wsy.jpg)

## Main Dispatch Queue/Global Dispatch Queue

- 有两个队列是不需要我们手动创建，release的，就是Main Dispatch Queue/Global Dispatch Queue，是系统为我们准备好的

  ![25C63EB5-78A5-4324-9338-4E9F25B7BBE9](http://ww4.sinaimg.cn/large/006tNc79ly1g5t8kkjvfsj314k0giwm8.jpg)

  - Main Dispatch Queue：是在主线程中执行的Dispatch Queue。因为主线程只有一个，所以它是Serial Dispatch Queue。追加到Main Dispatch Queue的处理在主线程的RunLoop中执行。
  - Global Dispatch Queue：是所有应用程序都能够使用的Concurrent Dispatch Queue。有四个执行优先级，高优先级（High Priority）、默认优先级（Default Priority）、低优先级（Low Priority）和后台优先级（Background Priority）。XNU内核管理的用于Global Dispatch Queue的线程，将各自使用的Global Dispatch Queue的执行优先级作为线程的执行优先级使用。向Global Dispatch Queue中追加处理时，要选择与处理内容对应的执行优先级的Global Dispatch Queue。



## dispatch_set_target_queue

- dispatch_set_target_queue(dispatch_object_t object, dispatch_queue_t queue);
- 第一个参数是要执行变更的队列（不能指定主队列和全局队列），第二个参数是目标队列（指定全局队列）
- dispatch_set_target_queue 函数有两个作用：第一，变更队列的执行优先级；第二，目标队列可以成为原队列的执行阶层。
- 使用该函数，可以将object队列设置为目标队列的优先级
- 另一个作用设置执行阶层

```objective-c
//首先创建5个串行队列
dispatch_queue_t serialQueue1 = dispatch_queue_create("com.gcd.setTargetQueue2.serialQueue1", NULL);
dispatch_queue_t serialQueue2 = dispatch_queue_create("com.gcd.setTargetQueue2.serialQueue2", NULL);
dispatch_queue_t serialQueue3 = dispatch_queue_create("com.gcd.setTargetQueue2.serialQueue3", NULL);
dispatch_queue_t serialQueue4 = dispatch_queue_create("com.gcd.setTargetQueue2.serialQueue4", NULL);
dispatch_queue_t serialQueue5 = dispatch_queue_create("com.gcd.setTargetQueue2.serialQueue5", NULL);

//每个队列上输出一个数字
dispatch_async(serialQueue1, ^{
    NSLog(@"1");
});
dispatch_async(serialQueue2, ^{
    NSLog(@"2");
});
dispatch_async(serialQueue3, ^{
    NSLog(@"3");
});
dispatch_async(serialQueue4, ^{
    NSLog(@"4");
});
dispatch_async(serialQueue5, ^{
    NSLog(@"5");
});

//这样就是5个串行队列在并行执行操作，执行结果无固定顺序
2017-02-28 21:32:48.787 GCDLearn[1449:71250] 5
2017-02-28 21:32:48.786 GCDLearn[1449:71242] 3
2017-02-28 21:32:48.786 GCDLearn[1449:71226] 1
2017-02-28 21:32:48.786 GCDLearn[1449:71235] 2
2017-02-28 21:32:48.786 GCDLearn[1449:71244] 4
  
//创建目标串行队列
dispatch_queue_t targetSerialQueue = dispatch_queue_create("com.gcd.setTargetQueue2.targetSerialQueue", NULL);

//设置执行阶层
dispatch_set_target_queue(serialQueue1, targetSerialQueue);
dispatch_set_target_queue(serialQueue2, targetSerialQueue);
dispatch_set_target_queue(serialQueue3, targetSerialQueue);
dispatch_set_target_queue(serialQueue4, targetSerialQueue);
dispatch_set_target_queue(serialQueue5, targetSerialQueue);

//执行操作
dispatch_async(serialQueue1, ^{
    NSLog(@"1");
});
dispatch_async(serialQueue2, ^{
    NSLog(@"2");
});
dispatch_async(serialQueue3, ^{
    NSLog(@"3");
});
dispatch_async(serialQueue4, ^{
    NSLog(@"4");
});
dispatch_async(serialQueue5, ^{
    NSLog(@"5");
});

//有序
2017-02-28 21:38:06.606 GCDLearn[1506:75803] 1
2017-02-28 21:38:06.607 GCDLearn[1506:75803] 2
2017-02-28 21:38:06.607 GCDLearn[1506:75803] 3
2017-02-28 21:38:06.608 GCDLearn[1506:75803] 4
2017-02-28 21:38:06.608 GCDLearn[1506:75803] 5
```

- 在必须将不可并行执行的处理追加到多个Serial Dispatch Queue中时，如果使用dispatch_set_target_queue函数将目标指定为某一个Serial Dispatch Queue，即可防止处理并行执行
- 这个函数平时见的可能不多，但后面很多操作其实都是依靠这个函数来执行的，是一个基础函数

## dispatch_after

- 想在指定时间后执行处理的情况

```objective-c
dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, 3ull * NSEC_PER_SEC);

// 在3秒后追加Block到Main Dispatch Queue中
dispatch_after(time, dispatch_get_main_queue(), ^{
    
    NSLog(@"waited at least three seconds");;
});

dispatch_after函数
第一个参数:
指定时间的dispatch_time_t类型的值，可以使用dispatch_time函数或dispatch_walltime函数作成。

第二个参数：
要追加处理的Dispatch Queue。

第三个参数：
要执行处理的Block。
```

- dispatch_after函数不是在指定时间后执行处理，而只是在指定时间追加处理到Dispatch Queue（第二个参数）。
- dispatch_after函数精度问题，有误差，大致延迟执行处理，可以用该函数，严格要求时间下会出现问题。
- dispatch_time函数获取从第一个参数指定时间开始，到第二个指定时间后的时间，通常用于计算相对时间。DISPATCH_TIME_NOW 表示现在的时间。

```objective-c
// 从现在开始1秒后的dispatch_time_t类型的值
dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, 1ull * NSEC_PER_SEC);

“ull”是C语言的数值字面量，是显示表明类型时使用的字符串。
NSEC_PER_SEC单位：毫微秒
NSEC_PER_MSEC单位：毫秒
```

## Dispatch Group

- 在追加到Dispatch Queue中的多个处理全部结束后想执行结束处理，使用一个Serial Dispatch Queue时，只要将结束处理最后追加即可。使用Concurrent Dispatch Queue时，使用Dispatch Group。
- 作用就是可以监听group里的操作是否走完，通过dispatch_group_notify在监听的group走完后才执行某个任务

```objective-c
// 例子：追加3个Block到Global Dispatch Queue，这些处理全部执行完毕，会执行追加到Main Dispatch Queue中的结束处理Block

dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

dispatch_group_t group = dispatch_group_create();

dispatch_group_async(group, queue, ^{
    NSLog(@"0");
});

dispatch_group_async(group, queue, ^{
    NSLog(@"1");
});

dispatch_group_async(group, queue, ^{
    NSLog(@"2");
});

dispatch_group_notify(group, dispatch_get_main_queue(), ^{
    NSLog(@"done");
});

dispatch_release(group);

说明：
dispatch_group_create函数生成dispatch_group_t类型的Dispatch Group，函数中含有create，所以在使用结束后需要通过dispatch_release函数释放。

dispatch_group_async函数，第一个参数为生成的Dispatch Group，第二个参数为Dispatch Queue，第三个参数是Block，将Block追加到Dispatch Queue中，Block属于指定的Dispatch Group。

Block通过dispatch_retain函数持有Dispatch Group，Block执行结束，通过dispatch_release函数释放持有的Dispatch Group。Dispatch Group使用结束，不用考虑Block，立即通过dispatch_release函数释放即可。

dispatch_group_notify函数第一个参数为要监视的Dispatch Group，第二个参数为要追加结束处理的Dispatch Queue，第三个结束处理Block。
```

- dispatch_group_wait

```objective-c
dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

dispatch_group_t group = dispatch_group_create();

dispatch_group_async(group, queue, ^{
    NSLog(@"0");
});

dispatch_group_async(group, queue, ^{
    NSLog(@"1");
});

dispatch_group_async(group, queue, ^{
    NSLog(@"2");
});

dispatch_group_wait(group, DISPATCH_TIME_FOREVER);

说明：
dispatch_group_wait函数第一个参数为等待的Dispatch Group，第二个参数为等待的时间（超时），为dispatch_time_t类型。
DISPATCH_TIME_FOREVER：永久等待。
使用forever永久等待，只要group里的处理没有结束，就会一直等待，中途不能取消
```

- 指定时间间隔，观察是否执行完毕

```objective-c
dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, 1ull * NSEC_PER_SEC);

long result = dispatch_group_wait(group, time);

if (result == 0) {
    // 全部处理执行结束
} else {
    // 某一个处理还在执行中
}

说明:
dispatch_group_wait函数返回值为0：全部处理执行结束
dispatch_group_wait函数返回值不为0：某一个处理还在执行中
等待时间为DISPATCH_TIME_FOREVER，返回值恒为0。
注意：一旦调用dispatch_group_wait函数，该函数就处于调用状态而不返回。执行该函数的当前线程停止，在经过该函数中指定的时间或该函数的所有处理全部执行结束之前，执行该函数的线程停止。
  
//  指定DISPATCH_TIME_NOW，则不用任何等待即可判断属于Dispatch Group的处理是否全部执行结束
long result = dispatch_group_wait(group, DISPATCH_TIME_NOW);
```

## dispatch_barrier_async

- 对于并行队列，由于其中有多线程，难以预计任务完成顺序先后，如果需要在确保某些任务完成后执行某项任务，就需要用到栅栏

```objective-c
dispatch_queue_t queue = dispatch_queue_create("com.example.gdc.ForBarrier", DISPATCH_QUEUE_CONCURRENT);

dispatch_async(queue, ^{
    NSLog(@"reading1");
});

dispatch_async(queue, ^{
    NSLog(@"reading2");
});

dispatch_async(queue, ^{
    NSLog(@"reading3");
});

dispatch_barrier_async(queue, ^{
    NSLog(@"writing");
});

dispatch_async(queue, ^{
    NSLog(@"reading4");
});

dispatch_async(queue, ^{
    NSLog(@"reading5");
});

输出：
2018-07-23 22:38:16.498997+0800 Demo[9100:359307] reading3
2018-07-23 22:38:16.498997+0800 Demo[9100:359308] reading1
2018-07-23 22:38:16.498997+0800 Demo[9100:359311] reading2
2018-07-23 22:38:16.500972+0800 Demo[9100:359308] writing
2018-07-23 22:38:16.501812+0800 Demo[9100:359308] reading4
2018-07-23 22:38:16.501831+0800 Demo[9100:359311] reading5
```

- dispatch_barrier_async函数会等待该函数之前追加到Concurrent Dispatch Queue上的并行执行的处理全部结束后，再将指定的处理追加到该Concurrent Dispatch Queue中，dispatch_barrier_async函数追加的处理执行完毕后，该函数之后追加Concurrent Dispatch Queue的处理才又开始并行处理。

## dispatch_sync

- dispatch_async函数的“async”意味着“非同步”，将指定的Block非同步的追加到指定的Dispatch Queue中，不用等待Block处理执行结束。
- dispatch_sync函数的“sync”意味着“同步”，将指定的Block同步的追加到指定的Dispatch Queue中，等待Block处理执行结束，“等待”意味着当前线程停止，也就是说，执行dispatch_sync函数所在的Dispatch Queue的线程停止，等待dispatch_sync函数的Block处理在第一个参数Dispatch Queue中的线程中执行结束。

### 死锁  

```
dispatch_queue_t queue = dispatch_get_main_queue();
dispatch_sync(queue, ^{
    NSLog(@"处理");
});
```

![死锁的产生](http://ww3.sinaimg.cn/large/006tNc79ly1g5tito09arj30ms0dcmyy.jpg)

- 什么意思呢，我们知道我们默认是在主线程里运行，也就是说不只是block里的内容是在主线程里跑，单单是dispatch_sync这个函数也是在主线程里的一个任务
- 我们知道队列遵循FIFO【先进先出】，现在这个队列里面有两个任务

  - 任务A：dispatch_sync函数
  - 任务B：block
- 在队列里面就是这样一个顺序，我们目前就执行到了任务A，任务A怎样算完呢？要等block结束才行，这样就A等B，B等A形成了死锁

```objective-c
dispatch_queue_t queue = dispatch_queue_create("com.example.gdc.mySerialDispatchQueue", NULL);
dispatch_async(queue, ^{
    dispatch_sync(queue, ^{
        NSLog(@"处理");
    });
});

//这个例本质上也是一样的
```

## dispatch_apply

- dispatch_apply函数按照指定的次数将指定的Block追加到指定的Dispatch Queue中，并等待全部处理执行结束。也就是说执行dispatch_apply函数所在的Dispatch Queue的线程会停止，等待全部处理执行结束，才继续向下执行。所以可以看出dispatch_apply函数和dispatch_sync函数和Dispatch Group函数是有关联的。

```objective-c
dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
dispatch_apply(10, queue, ^(size_t index) {
    NSLog(@"%zu",index);
});
NSLog(@"done");

输出：
2018-07-24 23:27:57.588905+0800 Demo[14916:512488] 0
2018-07-24 23:27:57.588909+0800 Demo[14916:512562] 3
2018-07-24 23:27:57.588905+0800 Demo[14916:512565] 2
2018-07-24 23:27:57.588906+0800 Demo[14916:512563] 1
2018-07-24 23:27:57.589128+0800 Demo[14916:512562] 4
2018-07-24 23:27:57.589128+0800 Demo[14916:512488] 5
2018-07-24 23:27:57.589132+0800 Demo[14916:512565] 6
2018-07-24 23:27:57.589220+0800 Demo[14916:512563] 7
2018-07-24 23:27:57.589284+0800 Demo[14916:512562] 8
2018-07-24 23:27:57.589311+0800 Demo[14916:512488] 9
2018-07-24 23:27:57.590148+0800 Demo[14916:512488] done

第一个参数：重复次数
第二个参数：执行处理的Diapatch Queue
第三个参数：要执行的处理，带有参数，第多少次，是为了按第一个参数重复追加Block并区分各个Block而使用的。

在Global Dispatch Queue中并行执行，全部处理都会执行，各个处理执行时间不定，。
```

## dispatch_suspend/dispatch_resume

- dispatch_suspend，dispatch_resume提供了“挂起、恢复”队列的功能，简单来说，就是可以暂停、恢复队列上的任务。但是这里的“挂起”，并不能保证可以立即停止队列上正在运行的block
  // 挂起指定队列
  dispatch_suspend(queue);
  // 恢复指定队列
  dispatchp_resume(queue);

  ```objective-c
  dispatch_queue_t queue = dispatch_queue_create("com.test.gcd", DISPATCH_QUEUE_SERIAL);
      //提交第一个block，延时5秒打印。
      dispatch_async(queue, ^{
          sleep(5);
          NSLog(@"After 5 seconds...");
      });
      //提交第二个block，也是延时5秒打印
      dispatch_async(queue, ^{
          sleep(5);
          NSLog(@"After 5 seconds again...");
      });
      //延时一秒
      NSLog(@"sleep 1 second...");
      sleep(1);
      //挂起队列
      NSLog(@"suspend...");
      dispatch_suspend(queue);
      //延时10秒
      NSLog(@"sleep 10 second...");
      sleep(10);
      //恢复队列
      NSLog(@"resume...");
      dispatch_resume(queue);
  ```

## Dispatch Semaphore

- 当并行执行的处理更新数据时，会产生数据不一致，甚至出现异常结束的情况，使用Serial Dispatch Queue和dispatch_barrier_async函数可避免，但使用Dispatch Semaphore可以进行更细颗粒的排他控制。

```objective-c
dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

NSMutableArray *array = @[].mutableCopy;

for (int i = 0; i < 1000; i++) {
    dispatch_async(queue, ^{
        [array addObject:[NSNumber numberWithInt:i]];
    });
}

说明：可能出现同时访问数组，造成异常结束的问题。
```

- Dispatch Semaphore是持有计数的信号，该计数是多线程编程中的计数类型信号。
  Dispatch Semaphore中，计数为0时等待，计数为1或者大于1时，减去1而不等待。

```objective-c
dispatch_semaphore_t semaphore = dispatch_semaphore_create(1);

说明：
dispatch_semaphore_create函数生成Dispatch Semaphore。
参数表示计数的初始值。
含有create可以看出，该函数与Dispatch Queue和Dispatch Group一样，必须通过dispatch_release函数释放，也可以通过dispatch_retain函数持有。
dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

说明：
该函数等待第一个参数semaphore的计数值达到大于等于1。
当计数值大于等于1，或者在待机（等待的时间）中计数值大于等于1，对该计数进行减1并从该函数返回。
第二个参数是dispatch_time_t类型值，与dispatch_group_wait函数相同。
DISPATCH_TIME_FOREVER：永久等待。

dispatch_semaphore_signal(semaphore);

说明：
处理结束时通过该函数将semaphore的计数值加1
dispatch_semaphore_t semaphore = dispatch_semaphore_create(1);

dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, 1ull * NSEC_PER_SEC);

long result = dispatch_semaphore_wait(semaphore, time);

if (result == 0) {
    // 由于semaphore的计数值大于等于1或在等待时间内计数值大于等于1， 所以semaphore的计数值减1 执行需要进行排他控制的处理
} else {
    // semaphore的计数值直到1秒的等待时间结束都为0。
}

说明：
dispatch_semaphore_wait函数的返回值与dispatch_group_wait函数相同，semaphore大于等于1，result为0，semaphore为0，result不为0。
```

- 对数组的安全处理

```objective-c
dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

dispatch_semaphore_t semaphore = dispatch_semaphore_create(1);

NSMutableArray *array = @[].mutableCopy;

for (int i = 0; i < 1000; i++) {
    dispatch_async(queue, ^{
        
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        
        [array addObject:[NSNumber numberWithInt:i]];
        
        // 处理结束时通过该函数将semaphore的计数值加1
        dispatch_semaphore_signal(semaphore);
    });
}

// 使用结束，释放semaphore
dispatch_release(semaphore);
```

## dispatch_once

- dispatch_once函数是保证在程序执行中只执行一次指定处理的API

```objective-c
static int initialized = NO;
if (initialized == NO) {
    // 初始化
    initialized = YES;
}

//说明：
//上面代码，在大多数情况下是安全的，但是在多核CPU中，在正在更新表示是否初始化的标志变量initialized时读取，就有可能多次执行初始化处理
static dispatch_once_t pred;
dispatch_once(&pred, ^{
    //初始化
});

//说明：
//在多线程环境下执行，也可以保证安全
```

## Dispatch I/O

Dispatch I/O和Dispatch Data可以实现输入/输出做到多个线程并列读取。
Dispatch I/O读写文件时，使用Global Dispatch Queue将一个文件按某个大小read/write。
分割读取的数据使用Dispatch Data可以更为简单的进行结合和分割。

```objective-c
// Apple System Log API用的源代码
static int
_asl_auxiliary(aslmsg msg, const char *title, const char *uti, const char *url, int *out_fd)
{
    asl_msg_t *merged_msg;
    asl_msg_aux_t aux;
    asl_msg_aux_0_t aux0;
    fileport_t fileport;
    kern_return_t kstatus;
    uint32_t outlen, newurllen, len, where;
    int status, fd, fdpair[2];
    caddr_t out, newurl;
    dispatch_queue_t pipe_q;
    dispatch_io_t pipe_channel;
    dispatch_semaphore_t sem;
    /* ..... 此处省略若干代码.....*/
    
    // 创建串行队列
    pipe_q = dispatch_queue_create("PipeQ", NULL);
    // 生成Dispatch I/O，指定发生错误时执行处理的Block，以及执行该Block的Dispatch Queue。 
    pipe_channel = dispatch_io_create(DISPATCH_IO_STREAM, fd, pipe_q, ^(int err){
        close(fd);
    });
    
    *out_fd = fdpair[1];
    
    // 该函数设定一次读取的大小（分割大小）
    dispatch_io_set_low_water(pipe_channel, SIZE_MAX);
    // 使用Global Dispatch Queue并列读取，当每个分割的文件块读取结束，将Dispatch Data传递给回调的Block.
    dispatch_io_read(pipe_channel, 0, SIZE_MAX, pipe_q, ^(bool done, dispatch_data_t pipedata, int err){
        if (err == 0) // err等于0 说明读取无误
        {
            // 读取完“单个文件块”的大小
            size_t len = dispatch_data_get_size(pipedata);
            if (len > 0)
            {
                // 定义一个字节数组bytes
                const char *bytes = NULL;
                char *encoded;
                
                dispatch_data_t md = dispatch_data_create_map(pipedata, (const void **)&bytes, &len);
                encoded = asl_core_encode_buffer(bytes, len);
                asl_set((aslmsg)merged_msg, ASL_KEY_AUX_DATA, encoded);
                free(encoded);
                _asl_send_message(NULL, merged_msg, -1, NULL);
                asl_msg_release(merged_msg);
                dispatch_release(md);
            }
        }
        
        if (done)
        {
            dispatch_semaphore_signal(sem);
            dispatch_release(pipe_channel);
            dispatch_release(pipe_q);
        }
    });
// 异步串行读取文件
NSString *desktop = @"/Users/xxxx/Desktop";
NSString *path = [desktop stringByAppendingPathComponent:@"整理.md"];
dispatch_queue_t queue = dispatch_queue_create("queue", NULL);

dispatch_fd_t fd = open(path.UTF8String, O_RDONLY, 0);
dispatch_io_t io = dispatch_io_create(DISPATCH_IO_STREAM, fd, queue, ^(int error) {
    close(fd);
});

size_t water = 1024 * 1024;
dispatch_io_set_low_water(io, water);
dispatch_io_set_high_water(io, water);

long long fileSize = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil].fileSize;
NSMutableData *totalData = [[NSMutableData alloc] init];

dispatch_io_read(io, 0, fileSize, queue, ^(bool done, dispatch_data_t  _Nullable data, int error) {
    
    if (error == 0) {
        size_t len = dispatch_data_get_size(data);
        if (len > 0) {
            [totalData appendData:(NSData *)data];
        }
    }
    
    if (done) {
        NSString *str = [[NSString alloc] initWithData:totalData encoding:NSUTF8StringEncoding];
        NSLog(@"%@", str);
    }
});
// 异步并行读取文件
NSString *desktop = @"/Users/xxx/Desktop";
NSString *path = [desktop stringByAppendingPathComponent:@"整理.md"];

dispatch_queue_t queue = dispatch_queue_create("queue", DISPATCH_QUEUE_CONCURRENT);
dispatch_fd_t fd = open(path.UTF8String, O_RDONLY);
dispatch_io_t io = dispatch_io_create(DISPATCH_IO_RANDOM, fd, queue, ^(int error) {
    close(fd);
});

off_t currentSize = 0;
long long fileSize = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil].fileSize;

size_t offset = 1024 * 1024;
dispatch_group_t group = dispatch_group_create();
NSMutableData *totalData = [[NSMutableData alloc] initWithLength:fileSize];

for (; currentSize <= fileSize; currentSize += offset) {
    dispatch_group_enter(group);
    dispatch_io_read(io, currentSize, offset, queue, ^(bool done, dispatch_data_t  _Nullable data, int error) {
        
        if (error == 0) {
            size_t len = dispatch_data_get_size(data);
            if (len > 0) {
                const void *bytes = NULL;
                (void)dispatch_data_create_map(data, (const void **)&bytes, &len);
                [totalData replaceBytesInRange:NSMakeRange(currentSize, len) withBytes:bytes length:len];
            }
        }
        
        if (done) {
            dispatch_group_leave(group);
            
            NSString *str = [[NSString alloc] initWithData:totalData encoding:NSUTF8StringEncoding];
            NSLog(@"%@", str);
        }
    });
}
```