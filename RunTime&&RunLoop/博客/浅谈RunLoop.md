[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文对于RunLoop不做过多的源码解析，主要总结RunLoop工作流程以及面试需要知道的点
- RunLoop我感觉要比RunTime还要难一点，资料少且抽象，我总结的也不一定对，请读者斧正
- 建议对RunLoop没有概念的可以先看下这个视频[iOS线下分享《RunLoop》by 孙源@sunnyxx](https://v.youku.com/v_show/id_XODgxODkzODI0.html)

# 准备工作

- 请准备好750.1版本的objc4源码一份【目前最新的版本】，打开它，找到文章中提到的方法，类型，对象
- 一切请以手中源码为准，不要轻信任何人，任何文章，包括本篇博客
- 文章中的源码都请过了我的删改，建议还是先看看源码
- 源码建议从Apple官方开源网站获取[obj4](https://opensource.apple.com/tarballs/objc4/objc4-750.1.tar.gz)
- 官网上下载下来需要自己配置才能编译运行，如果不想配置，可以在[RuntimeSourceCode](https://github.com/acBool/RuntimeSourceCode)中clone

# RunLoop概览

- 我们先要对RunLoop有个基本的概念

## RunLoop = 跑圈

- RunLoop直译就是跑圈的意思，在外面刚学OC的时候写的命令行程序可能会是这样的

```objective-c
int main(int argc, char * argv[]) {
  NSLog(@"Hello world");
  return 0;
}
```

- 这样的程序在打印完Hello world就会直接程序结束
- 而我们的手机APP显然不可能会结束，要保证一直存活，我们需要一个东西来让线程随时都能处理事件，暂时没事件也不会退出
- 这种模型被称之为Event Loop，在其他系统，例如Windows上都有类似的机制

```c++
function loop() {
    initialize();
    do {
        var message = get_next_message();
        process_message(message);
    } while (message != quit);
}
```

- RunLoop其实就可以想象成一个不停歇的while循环

![94958072116452860144CA9EED179745](http://ww2.sinaimg.cn/large/006tNc79ly1g606j7n1ahj31hc0u0am5.jpg)

![15518271BA6B83CE4A9F02ED7E625ABD](http://ww4.sinaimg.cn/large/006tNc79ly1g606pombthj31hc0u0th6.jpg)

## RunLoop也是一个对象

- RunLoop也是一个对象，它主要做了两件事：
  - 在循环中处理运行过程中的各种事件（触摸事件、UI刷新事件、定时器事件、Selector事件）
  - 没有事件时进入睡眠模式，节约CPU

![FC1654FB7370672ED2ECCB7CB3E7FA51](http://ww4.sinaimg.cn/large/006tNc79ly1g606p7q0g1j31hc0u0thy.jpg)

## RunLoop与线程的关系

- RunLoop与线程息息相关，RunLoop使得线程不会在执行完当前任务后直接退出

### 四句箴言

- RunLoop与线程一一对应，保存在一个字典里
- 我们只在当前线程操作当前线程的RunLoop，不去操作其他线程的RunLoop
- RunLoop创建是在第一次获取时创建，销毁是在线程结束的时候销毁
- 主线程的RunLoop系统自动创建，其余的需要我们手动创建

### 验证

- 苹果不允许直接创建 RunLoop，它只提供了两个自动获取的函数：CFRunLoopGetMain() 和 CFRunLoopGetCurrent()。 这两个函数内部的逻辑大概是下面这样:

```objective-c
/// 全局的Dictionary，key 是 pthread_t， value 是 CFRunLoopRef
static CFMutableDictionaryRef loopsDic;
/// 访问 loopsDic 时的锁
static CFSpinLock_t loopsLock;
 
/// 获取一个 pthread 对应的 RunLoop。
CFRunLoopRef _CFRunLoopGet(pthread_t thread) {
    OSSpinLockLock(&loopsLock);
    
    if (!loopsDic) {
        // 第一次进入时，初始化全局Dic，并先为主线程创建一个 RunLoop。
        loopsDic = CFDictionaryCreateMutable();
        CFRunLoopRef mainLoop = _CFRunLoopCreate();
        CFDictionarySetValue(loopsDic, pthread_main_thread_np(), mainLoop);
    }
    
    /// 直接从 Dictionary 里获取。
    CFRunLoopRef loop = CFDictionaryGetValue(loopsDic, thread));
    
    if (!loop) {
        /// 取不到时，创建一个
        loop = _CFRunLoopCreate();
        CFDictionarySetValue(loopsDic, thread, loop);
        /// 注册一个回调，当线程销毁时，顺便也销毁其对应的 RunLoop。
        _CFSetTSD(..., thread, loop, __CFFinalizeRunLoop);
    }
    
    OSSpinLockUnLock(&loopsLock);
    return loop;
}
 
CFRunLoopRef CFRunLoopGetMain() {
    return _CFRunLoopGet(pthread_main_thread_np());
}
 
CFRunLoopRef CFRunLoopGetCurrent() {
    return _CFRunLoopGet(pthread_self());
}
```

- 从上面的代码可以看出，线程和 RunLoop 之间是一一对应的，其关系是保存在一个全局的 Dictionary 里。线程刚创建时并没有 RunLoop，如果你不主动获取，那它一直都不会有。RunLoop 的创建是发生在第一次获取时，RunLoop 的销毁是发生在线程结束时。你只能在一个线程的内部获取其 RunLoop（主线程除外）。

### 主线程RunLoop的开启

- UIApplicationMain函数内启动了Runloop
```objective-c
//原来的main
int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}

//UIApplicationMain
UIKIT_EXTERN int UIApplicationMain(int argc, char *argv[], NSString * __nullable principalClassName, NSString * __nullable delegateClassName);

//修改后的main
int main(int argc, char * argv[]) {
    @autoreleasepool {
        NSLog(@"开始");
        int re = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        NSLog(@"结束");
        return re;
    }
}
```

- 上述的re不会被打印，说明这个UIApplicationMain就类似于死循环，不会走出来
- 伪代码：

```objective-c
int main(int argc, char * argv[]) {        
    BOOL running = YES;
    do {
        // 执行各种任务，处理各种事件
        // ......
    } while (running);

    return 0;
}
```

## RunLoop封装

- OSX/iOS 系统中，提供了两个这样的对象：NSRunLoop 和 CFRunLoopRef。

- CFRunLoopRef 是在 CoreFoundation 框架内的，它提供了纯 C 函数的 API，所有这些 API 都是线程安全的。

- NSRunLoop 是基于 CFRunLoopRef 的封装，提供了面向对象的 API，但是这些 API 不是线程安全的。

  ![E61AF5682717A45CBA3F4E16F7F10D6E](http://ww1.sinaimg.cn/large/006tNc79ly1g606ybwpy9j31hc0u0qcn.jpg)

# RunLoop相关类

- RunLoop相关的一共有五个类

## 五类无泪

- CFRunLoopRef：代表RunLoop的对象  【RunLoop】
- CFRunLoopModeRef：RunLoop的运行模式  【Mode】
- CFRunLoopSourceRef：就是RunLoop模型图中提到的输入源/事件源。【Source】
- CFRunLoopTimerRef：就是RunLoop模型图中提到的定时源  【Observer】
- CFRunLoopObserverRef：观察者，能够监听RunLoop的状态改变  【Timer】

![iOS-Complete-learning-RunLoop-002](http://ww4.sinaimg.cn/large/006tNc79ly1g6075rqmxaj30m90hgacd.jpg)

![D0FAE6553C0A4B91E066995227CCF1C9](http://ww4.sinaimg.cn/large/006tNc79ly1g60fpfjmrzj31hc0u0n6y.jpg)

- 大概关系和套娃一样，RunLoop里装Mode，Mode里装Source/Observer/Timer

- 一个RunLoop对象（CFRunLoopRef）中包含若干个运行模式（CFRunLoopModeRef）。而每一个运行模式下又包含若干个输入源（CFRunLoopSourceRef）、定时源（CFRunLoopTimerRef）、观察者（CFRunLoopObserverRef）。
  - 每次RunLoop启动时，只能指定其中一个运行模式（CFRunLoopModeRef），这个运行模式（CFRunLoopModeRef）被称作CurrentMode。
  - 如果需要切换运行模式（CFRunLoopModeRef），只能退出Loop，再重新指定一个运行模式（CFRunLoopModeRef）进入。
  - 这样做主要是为了分隔开不同组的输入源（CFRunLoopSourceRef）、定时源（CFRunLoopTimerRef）、观察者（CFRunLoopObserverRef），让其互不影响 。

## CFRunLoopRef

- CFRunLoopRef就是Core Foundation框架下RunLoop对象类
- 对于这个没什么好讲的，知道下获取方法就行

```objective-c
//Core Foundation
CFRunLoopGetCurrent(); // 获得当前线程的RunLoop对象
CFRunLoopGetMain(); // 获得主线程的RunLoop对象

//Foundation
[NSRunLoop currentRunLoop]; // 获得当前线程的RunLoop对象
[NSRunLoop mainRunLoop]; // 获得主线程的RunLoop对象
```

## CFRunLoopModeRef

- 这个Mode等于就是指定了RunLoop的执行模式，但这里请理清概念，我们的RunLoop里可以装多个Mode，只是我们在指定运行的时候要指定一个Mode
- Mode的大致结构如下：

 ```c++
struct __CFRunLoopMode {
    CFStringRef _name;            // Mode Name, 例如 @"kCFRunLoopDefaultMode"
    CFMutableSetRef _sources0;    // Set
    CFMutableSetRef _sources1;    // Set
    CFMutableArrayRef _observers; // Array
    CFMutableArrayRef _timers;    // Array
    ...
};
 
struct __CFRunLoop {
    CFMutableSetRef _commonModes;     // Set
    CFMutableSetRef _commonModeItems; // Set<Source/Observer/Timer>
    CFRunLoopModeRef _currentMode;    // Current Runloop Mode
    CFMutableSetRef _modes;           // Set
    ...
};
 ```

- 整个结构和我们之前分析的差不多，唯一不同的是多了一个common Mode，具体的我们到下面再分析

1. **kCFRunLoopDefaultMode**：App的默认运行模式，通常主线程是在这个运行模式下运行
2. **UITrackingRunLoopMode**：跟踪用户交互事件（用于 ScrollView 追踪触摸滑动，保证界面滑动时不受其他Mode影响），这也是为什么iOS滑动顺滑的重要原因之一
3. UIInitializationRunLoopMode：在刚启动App时第进入的第一个 Mode，启动完成后就不再使用
4. GSEventReceiveRunLoopMode：接受系统内部事件，通常用不到
5. **kCFRunLoopCommonModes**：伪模式，不是一种真正的运行模式

- 这几个mode都很好了理解【不好理解的我们也用不到😂】

- 这里把commonMode主要讲解一下，commonModel不是一个真正的mode，不像tracking，Default这些有其适用范围，它只是一个标记
- 当把mode标记为common时【将ModeName添加到RunLoop中的“commodModes”中；**添加到commonMode**里的Source/Observer/Timer添加到_commonModeItems】。每当RunLoop中的东西改变，RunLoop 都会自动将 _commonModeItems 里的 Source/Observer/Timer 同步到具有 “Common” 标记的所有Mode里。

### 一个运用common的常见例子

- 如果我们在界面上写了一个tableview，同时像这样添加了一个定时器

```objective-c
// 定义一个定时器，约定两秒之后调用self的run方法
    NSTimer *timer = [NSTimer timerWithTimeInterval:2.0 target:self selector:@selector(run) userInfo:nil repeats:YES];
    
    // 将定时器添加到当前RunLoop的NSDefaultRunLoopMode下,一旦RunLoop进入其他模式，定时器timer就不工作了
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
```

- run方法会每隔两秒走一次，但如果我们拖动tableview就不会再走
- 因为我们的NSTimer添加在了defaultMode里面，当我们拖动时，runloop会先退出，进入trackingMode，由于里面没有该timer，所以不会走这个回调
- 而TrackingModel和DefaultModel都已经默认设置为CommonModel，我们如果添加timer的时候是添加在commonModel上就等于给default和trakcing都添加了，这样我们拖动的时候也会走回调

```objective-c
[[NSRunLoop currentRunLoop] addTimer:timer forMode:UITrackingRunLoopMode];
```

## **CFRunLoopSourceRef**

- 事件产生的地方，分为两类【前方高能！准备吐槽！】Source0以及Source1【啊！多么优美的命名！风骚的1和0象征着二进制的本质，01即是万物的象征，这个命名太妙了！👏👏👏】
  - Source0只包含回调，不能主动触发事件。使用时，要先CFRunLoopSourceSignal(source)，将这个 Source 标记为待处理，然后手动调用 CFRunLoopWakeUp(runloop) 来唤醒 RunLoop，让其处理这个事件。
  - Source1 包含了一个 mach_port 和一个回调（函数指针），被用于通过内核和其他线程相互发送消息。这种 Source 能主动唤醒 RunLoop 的线程。
## **CFRunLoopTimerRef**
- CFRunLoopTimerRef是基于时间的触发器基本上就是NSTimer
- 其包含一个时间长度和一个回调（函数指针）。当其加入到 RunLoop 时，RunLoop会注册对应的时间点，当时间点到时，RunLoop会被唤醒以执行那个回调。
- 这个有我们上面讲的例子应该很好理解了

## **CFRunLoopObserverRef**

- 观察者，每个 Observer 都包含了一个回调（函数指针），当 RunLoop 的状态发生变化时，观察者就能通过回调接受到这个变化。可以观测的时间点有以下几个：

```c++
typedef CF_OPTIONS(CFOptionFlags, CFRunLoopActivity) {
    kCFRunLoopEntry         = (1UL << 0), // 即将进入Loop 1
    kCFRunLoopBeforeTimers  = (1UL << 1), // 即将处理 Timer 2
    kCFRunLoopBeforeSources = (1UL << 2), // 即将处理 Source 4
    kCFRunLoopBeforeWaiting = (1UL << 5), // 即将进入休眠 32
    kCFRunLoopAfterWaiting  = (1UL << 6), // 刚从休眠中唤醒 64
    kCFRunLoopExit          = (1UL << 7), // 即将退出Loop 128
};
```

- 上面的 Source/Timer/Observer 被统称为 **mode item**，一个 item 可以被同时加入多个 mode例如上面的NSTimer。但一个 item 被重复加入同一个 mode 时是不会有效果的。如果一个 mode 中一个 item 都没有，则 RunLoop 会直接退出，不进入循环。

# RunLoop的内部逻辑

![RunLoop_1](http://ww4.sinaimg.cn/large/006tNc79ly1g60h76byy2j30zy0rodls.jpg)

## 代码梳理版本

```C++
/// 用DefaultMode启动
void CFRunLoopRun(void) {
    CFRunLoopRunSpecific(CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 1.0e10, false);
}
 
/// 用指定的Mode启动，允许设置RunLoop超时时间
int CFRunLoopRunInMode(CFStringRef modeName, CFTimeInterval seconds, Boolean stopAfterHandle) {
    return CFRunLoopRunSpecific(CFRunLoopGetCurrent(), modeName, seconds, returnAfterSourceHandled);
}
 
/// RunLoop的实现
int CFRunLoopRunSpecific(runloop, modeName, seconds, stopAfterHandle) {
    
    /// 首先根据modeName找到对应mode
    CFRunLoopModeRef currentMode = __CFRunLoopFindMode(runloop, modeName, false);
    /// 如果mode里没有source/timer/observer, 直接返回。
    if (__CFRunLoopModeIsEmpty(currentMode)) return;
    
    /// 1. 通知 Observers: RunLoop 即将进入 loop。
    __CFRunLoopDoObservers(runloop, currentMode, kCFRunLoopEntry);
    
    /// 内部函数，进入loop
    __CFRunLoopRun(runloop, currentMode, seconds, returnAfterSourceHandled) {
        
        Boolean sourceHandledThisLoop = NO;
        int retVal = 0;
        do {
 
            /// 2. 通知 Observers: RunLoop 即将触发 Timer 回调。
            __CFRunLoopDoObservers(runloop, currentMode, kCFRunLoopBeforeTimers);
            /// 3. 通知 Observers: RunLoop 即将触发 Source0 (非port) 回调。
            __CFRunLoopDoObservers(runloop, currentMode, kCFRunLoopBeforeSources);
            /// 执行被加入的block
            __CFRunLoopDoBlocks(runloop, currentMode);
            
            /// 4. RunLoop 触发 Source0 (非port) 回调。
            sourceHandledThisLoop = __CFRunLoopDoSources0(runloop, currentMode, stopAfterHandle);
            /// 执行被加入的block
            __CFRunLoopDoBlocks(runloop, currentMode);
 
            /// 5. 如果有 Source1 (基于port) 处于 ready 状态，直接处理这个 Source1 然后跳转去处理消息。
            if (__Source0DidDispatchPortLastTime) {
                Boolean hasMsg = __CFRunLoopServiceMachPort(dispatchPort, &msg)
                if (hasMsg) goto handle_msg;
            }
            
            /// 通知 Observers: RunLoop 的线程即将进入休眠(sleep)。
            if (!sourceHandledThisLoop) {
                __CFRunLoopDoObservers(runloop, currentMode, kCFRunLoopBeforeWaiting);
            }
            
            /// 7. 调用 mach_msg 等待接受 mach_port 的消息。线程将进入休眠, 直到被下面某一个事件唤醒。
            /// • 一个基于 port 的Source 的事件。
            /// • 一个 Timer 到时间了
            /// • RunLoop 自身的超时时间到了
            /// • 被其他什么调用者手动唤醒
            __CFRunLoopServiceMachPort(waitSet, &msg, sizeof(msg_buffer), &livePort) {
                mach_msg(msg, MACH_RCV_MSG, port); // thread wait for receive msg
            }
 
            /// 8. 通知 Observers: RunLoop 的线程刚刚被唤醒了。
            __CFRunLoopDoObservers(runloop, currentMode, kCFRunLoopAfterWaiting);
            
            /// 收到消息，处理消息。
            handle_msg:
 
            /// 9.1 如果一个 Timer 到时间了，触发这个Timer的回调。
            if (msg_is_timer) {
                __CFRunLoopDoTimers(runloop, currentMode, mach_absolute_time())
            } 
 
            /// 9.2 如果有dispatch到main_queue的block，执行block。
            else if (msg_is_dispatch) {
                __CFRUNLOOP_IS_SERVICING_THE_MAIN_DISPATCH_QUEUE__(msg);
            } 
 
            /// 9.3 如果一个 Source1 (基于port) 发出事件了，处理这个事件
            else {
                CFRunLoopSourceRef source1 = __CFRunLoopModeFindSourceForMachPort(runloop, currentMode, livePort);
                sourceHandledThisLoop = __CFRunLoopDoSource1(runloop, currentMode, source1, msg);
                if (sourceHandledThisLoop) {
                    mach_msg(reply, MACH_SEND_MSG, reply);
                }
            }
            
            /// 执行加入到Loop的block
            __CFRunLoopDoBlocks(runloop, currentMode);
            
 
            if (sourceHandledThisLoop && stopAfterHandle) {
                /// 进入loop时参数说处理完事件就返回。
                retVal = kCFRunLoopRunHandledSource;
            } else if (timeout) {
                /// 超出传入参数标记的超时时间了
                retVal = kCFRunLoopRunTimedOut;
            } else if (__CFRunLoopIsStopped(runloop)) {
                /// 被外部调用者强制停止了
                retVal = kCFRunLoopRunStopped;
            } else if (__CFRunLoopModeIsEmpty(runloop, currentMode)) {
                /// source/timer/observer一个都没有了
                retVal = kCFRunLoopRunFinished;
            }
            
            /// 如果没超时，mode里没空，loop也没被停止，那继续loop。
        } while (retVal == 0);
    }
    
    /// 10. 通知 Observers: RunLoop 即将退出。
    __CFRunLoopDoObservers(rl, currentMode, kCFRunLoopExit);
}
```

- 可以看到，实际上 RunLoop 就是这样一个函数，其内部是一个 do-while 循环。当你调用 CFRunLoopRun() 时，线程就会一直停留在这个循环里；直到超时或被手动停止，该函数才会返回。

# RunLoop回调

- 当APP启动时，系统会默认注册五个Mode【就是上面那五个】
- 当 RunLoop 进行回调时，一般都是通过一个很长的函数调用出去 (call out), 当你在你的代码中下断点调试时，通常能在调用栈上看到这些函数。这就是RunLoop的流程：

```objective-c
{
    /// 1. 通知Observers，即将进入RunLoop
    /// 此处有Observer会创建AutoreleasePool: _objc_autoreleasePoolPush();
    __CFRUNLOOP_IS_CALLING_OUT_TO_AN_OBSERVER_CALLBACK_FUNCTION__(kCFRunLoopEntry);
    do {
 
        /// 2. 通知 Observers: 即将触发 Timer 回调。
        __CFRUNLOOP_IS_CALLING_OUT_TO_AN_OBSERVER_CALLBACK_FUNCTION__(kCFRunLoopBeforeTimers);
        /// 3. 通知 Observers: 即将触发 Source (非基于port的,Source0) 回调。
        __CFRUNLOOP_IS_CALLING_OUT_TO_AN_OBSERVER_CALLBACK_FUNCTION__(kCFRunLoopBeforeSources);
        __CFRUNLOOP_IS_CALLING_OUT_TO_A_BLOCK__(block);
 
        /// 4. 触发 Source0 (非基于port的) 回调。
        __CFRUNLOOP_IS_CALLING_OUT_TO_A_SOURCE0_PERFORM_FUNCTION__(source0);
        __CFRUNLOOP_IS_CALLING_OUT_TO_A_BLOCK__(block);
 
        /// 6. 通知Observers，即将进入休眠
        /// 此处有Observer释放并新建AutoreleasePool: _objc_autoreleasePoolPop(); _objc_autoreleasePoolPush();
        __CFRUNLOOP_IS_CALLING_OUT_TO_AN_OBSERVER_CALLBACK_FUNCTION__(kCFRunLoopBeforeWaiting);
 
        /// 7. sleep to wait msg.
        mach_msg() -> mach_msg_trap();
        
 
        /// 8. 通知Observers，线程被唤醒
        __CFRUNLOOP_IS_CALLING_OUT_TO_AN_OBSERVER_CALLBACK_FUNCTION__(kCFRunLoopAfterWaiting);
 
        /// 9. 如果是被Timer唤醒的，回调Timer
        __CFRUNLOOP_IS_CALLING_OUT_TO_A_TIMER_CALLBACK_FUNCTION__(timer);
 
        /// 9. 如果是被dispatch唤醒的，执行所有调用 dispatch_async 等方法放入main queue 的 block
        __CFRUNLOOP_IS_SERVICING_THE_MAIN_DISPATCH_QUEUE__(dispatched_block);
 
        /// 9. 如果如果Runloop是被 Source1 (基于port的) 的事件唤醒了，处理这个事件
        __CFRUNLOOP_IS_CALLING_OUT_TO_A_SOURCE1_PERFORM_FUNCTION__(source1);
 
 
    } while (...);
 
    /// 10. 通知Observers，即将退出RunLoop
    /// 此处有Observer释放AutoreleasePool: _objc_autoreleasePoolPop();
    __CFRUNLOOP_IS_CALLING_OUT_TO_AN_OBSERVER_CALLBACK_FUNCTION__(kCFRunLoopExit);
}
```

# RunLoop在Apple中的实际运用

## AutoreleasePool

- App启动后，苹果在主线程 RunLoop 里注册了两个 Observer，其回调都是 _wrapRunLoopWithAutoreleasePoolHandler()。
- 第一个 Observer 监视的事件是 Entry(即将进入Loop)，其回调内会调用 _objc_autoreleasePoolPush() 创建自动释放池。其 order 是-2147483647，优先级最高，保证创建释放池发生在其他所有回调之前。
- 第二个 Observer 监视了两个事件： BeforeWaiting(准备进入休眠) 时调用_objc_autoreleasePoolPop() 和 _objc_autoreleasePoolPush() 释放旧的池并创建新池；Exit(即将退出Loop) 时调用 _objc_autoreleasePoolPop() 来释放自动释放池。这个 Observer 的 order 是 2147483647，优先级最低，保证其释放池子发生在其他所有回调之后。
- 在主线程执行的代码，通常是写在诸如事件回调、Timer回调内的。这些回调会被 RunLoop 创建好的 AutoreleasePool 环绕着，所以不会出现内存泄漏，开发者也不必显示创建 Pool 了。

- 自动释放池考的比较多，其余的可以看[深入理解RunLoop](https://blog.ibireme.com/2015/05/18/runloop/)