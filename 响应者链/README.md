[TOC]
# 关于我的仓库
- 这篇文章是我为面试准备的iOS基础知识学习中的一篇
- 我将准备面试中找到的所有学习资料，写的Demo，写的博客都放在了这个仓库里[iOS-Engineer-Interview](https://github.com/KevinAshen/iOS-Engineer-Interview)
- 欢迎star👏👏
- 其中的博客在简书，CSDN都有发布
- 博客中提到的相关的代码Demo可以在仓库里相应的文件夹里找到

# 前言

- 本文会涉及事件传递，响应者链，手势操作等等
- 响应者链这一块也是面试会问到的一个知识点，同时也是我认为最亲民，项目里用到的可能性最大的一个知识点，值得好好学习
- 对于事件传递，响应者链这一块主要来自于Apple官方文档《Event Handling Guide for iOS》，这是一份74页的PDF，这里主要研究关于这两个部分的内容
- 个人认为博客这个东西在我这个实力，最多算个学习笔记，其实没啥特别值得一看的，而且被博客坑过后还是觉得啃官方文章比较踏实
- PDF在我的仓库里对应部分的学习资料里有，使用的翻译版本是[Event-Handling-Guide-for-iOS](https://github.com/BladeTail/Event-Handling-Guide-for-iOS)值得star！

# 好多概念

## “响应链”(responder chain)

- 当iOS捕获到某个事件时，就会将此事件传递给某个看上去最适合处理该事件的对象，比 如触摸事件传递给手指刚刚触摸位置的那个视图(view)，如果这个对象无法处理该事 件，iOS系统就继续将该事件传递给更深层的对象，直到找到能够对该事件作出响应处理 的对象为止。这一连串的对象序列被称作为“响应链”(responder chain)，iOS系统就是沿 着此响应链，由最外层逐步向内存对象传递该事件，亦即将处理该事件的责任进行传递。 iOS的这种机制，使得事件处理具有协调性和动态性。

## UIEvent

- UIEvent是一个描述单个用户与应用程序交互的对象。
- 点开其定义能看到它定义了事件类型，时间等等内容

```objective-c
NS_CLASS_AVAILABLE_IOS(2_0) @interface UIEvent : NSObject

@property(nonatomic,readonly) UIEventType     type NS_AVAILABLE_IOS(3_0);
@property(nonatomic,readonly) UIEventSubtype  subtype NS_AVAILABLE_IOS(3_0);

@property(nonatomic,readonly) NSTimeInterval  timestamp;

#if UIKIT_DEFINE_AS_PROPERTIES
@property(nonatomic, readonly, nullable) NSSet <UITouch *> *allTouches;
#else

@end
```



- 很多讲事件传递，响应链的文章都会把UIEvent抽象成点击，但实际上一个 UIEvent 既是一个触摸(touch)、一个摇晃(shake-motion)，或者一个远程控制 (remote-control)事件

![DF79B032-A77C-4F89-9DFE-2E57A96738F4](http://ww4.sinaimg.cn/large/006tNc79ly1g5p47vsk37j319s0is0y8.jpg)



- 而对于最常用触摸事件，我们先结合UITouch来了解

### UITouch

- 在 iOS 中，每一个“触摸”(touch)行为对象就代表单根手指在屏幕上的一次运动操 作。一个“手势”(gesture)可以有一个或者多个“触摸”行为对象，在 iOS 中以UITouch 类对象进行抽象表示。例如，一个捏合缩小手势就有两个“触摸”行为对象:两 根手指在屏幕上以相反运动方向相互靠拢运动。

![87D4D165-92CF-4F35-96DD-5986C0FCC7E0](http://ww4.sinaimg.cn/large/006tNc79ly1g5p4hzfpqwj30n908mgnm.jpg)

- 每一个“触摸”对象都只追踪一根手指的轨迹，其生命周期也仅限于整个触摸序列的起止 期间。在此这段时间内，UIKit 跟踪手指的轨迹并及时更行其对象的属性。这些属性包括 触摸的行为的方式、在当前视图对象中的位置、之前的位置以及时间戳。
- 触摸行为的方式指的是触摸行为的开始(运动或者静止)和结束(也就是手指离开屏幕 了)。如图 1-4 所示，App 在任意触摸行为方式的过程中接收到每一个触摸事件。
- **备注:一根手指相对于鼠标就没有那精确了。当用户触摸屏幕时，接触的区域实际上是个椭圆形 的，且精确度往往比用户预期的稍稍要更低一些。“接触面”大小会因手指的大小和方位、按压 的力道、哪根手指以及其他一些因素不一样而变化。设备内置的多点触摸系统会分析这些信息， 并计算单个“触摸”点，从而开发者不需要自己编写代码进行处理。**

### UIEvent与UITouch

- 一个事件(Event)包含了这次多点触摸行为序列的所有“触摸”对象。一个多点触摸行 为序列开始于用户第一根手指触摸到屏幕、终止于用户的最后一根手指抬起并离开屏幕。 当一根手指进行运动时，iOS 系统就会实例化 UITouch 对象并发送给对应的事件对象。一 个多点触摸事件会被抽象成一个类型为 UIEventTypeTouches 的 UIEvent 对象。

## 手势识别器【**GESTURE RECOGNIZER**】

- 一般对于非常常见的的手势都已经封装在UIKit框架中，开发者可以直接调用，发者对这些控件进行配置，这些控件就会给特定的目标实例 发送用户触摸事件的消息。
- 但同时也可以直接使用手势识别类让iOS视图类(view)具有target-action机制(以对用户的特定的操作进行响应)。开发者 将手势识别类(gesture recognizer)实例和某个视图实例进行绑定，就可以像control(控 制类)任意指定的用户手势操作进行响应。
- 手势识别类(gesture recognizer)为开发者提供了复杂事件处理逻辑的高度抽象接又，功 能强大、可复用、适配性，自然是开发者实现对用户触摸事件处理的首选。开发者可以直 接使用已集成(内建)的手势识别类，并自定义其处理动作，当然也可以自定义手势识别 类来处理新的用户手势操作。

# UIView，UIGestureRecognizer以及UIControl
- 我们日常使用的UI控件基本都继承与UIView，【UIButton继承于UIControl，但UIControl也同样继承于UIView】，UIView会继承于UIResponder，UIResponder在继承于NSObject
- UIResponder是我们真正进行响应的地方，比如下面这几个方法你肯定会经常见到，他们就是UIResponder

```objective-c
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
```

- 而UIControl相当于就是对UIResponder进行了一次封装，已经将手势与View进行了封装绑定，比如我们的UIButton为什么可以检测到双击，单击等等操作，也都是在UIControl里写好的

```objective-c
typedef NS_OPTIONS(NSUInteger, UIControlEvents) {
    UIControlEventTouchDown                                         
    UIControlEventTouchDownRepeat                                   
    UIControlEventTouchDragInside                                   
    UIControlEventTouchDragOutside                                  
    UIControlEventTouchDragEnter                                    
    UIControlEventTouchDragExit                                     
    UIControlEventTouchUpInside                                     
    UIControlEventTouchUpOutside                                    
    UIControlEventTouchCancel                                       
    UIControlEventValueChanged                                      
    UIControlEventPrimaryActionTriggered NS_ENUM_AVAILABLE_IOS(9_0) 
    UIControlEventEditingDidBegin                                   
    UIControlEventEditingChanged                                    
    UIControlEventEditingDidEnd                                     
    UIControlEventEditingDidEndOnExit                               
    UIControlEventAllTouchEvents                                    
    UIControlEventAllEditingEvents                                  
    UIControlEventApplicationReserved                               
    UIControlEventSystemReserved                                    
    UIControlEventAllEvents                                        
};
```



- 而我们的UIGestureRecognizer就是直接继承于NSObject，显然和UIResponder是两条路子，后面会探讨在UIResponder与UIGestureRecognizer同时起作用的时候会是什么情况

# 响应者链全貌

- 当用户触摸事件发生时，UIKit 框架就会为此创建一个事件对象，该对象就包含了能够处 理该事件对象所必要的信息。然后 UIKit 框架将此对象列入 active app 的事件队列。对于 触摸事件，事件对象就是组装的一系列 UIEvent 对象。对于动作事件，所对应的事件对象 因开发者所使用的框架、开发者感兴趣的动作事件类型不同而异。
- 每一个事件都会沿着特定的路径传递下去，直到某个对象能处理为止。首先，单例对象UIApplication 会从事件队列的顶端取出来一个事件并进行派遣，以便开始处理。通常，该 事件会被派送给 app 的主窗体(window)对象，然后由此 window 对象将事件传递给最 初事件发生所在的“初始对象”进行处理，这个初始对象是什么，就依赖于事件的类型
- Touch Event(触摸事件)。对于触摸事件，主窗体对象首先尝试将事件传递给事 件发生所在的视图 view 对象。该视图对象 view 就是所谓的 hit-test 视图对象。这 个寻找 hit-test 视图对象的过程被称作 hit-testing
- 事件对象路径的最终目标，就是找到能够处理事件并进行响应的对象。因此，UIKit 先会 将事件发送给最适合处理该事件的对象。对于触摸事件，该对象就是 hit-test 视图对象， 对于其他的事件，该对象就是第一响应器。以下部分会更详细地对 hit-test 视图对象和所 确定的第一响应器进行解释。

# hit-test

- iOS 系统使用 Hit-Testing 去查找到底触摸事件发生在哪个视图对象上。Hit-Testing 先检查 触摸对象所在的位置是否在对应任意屏幕上的视图对象的区域范围内。如果在的话，就开 始对此视图对象的子视图对象进行同样的检查。视图树中最底层那个包含此触摸点位置的 视图对象，就是要查找的 hit-test 视图对象。iOS 一旦确定 hit-test 视图对象，就会把触摸 事件传递给它进行处理。
- 举个例子，假设用户触摸了视图 E，如图 2-1 所示。iOS 就会按照以下顺序对子视图进行 检查来查找 hit-test 视图

![5955953E-1062-4001-839E-48A60B9B2DCE](http://ww4.sinaimg.cn/large/006tNc79ly1g5po9eq6qrj30pc0clwfa.jpg)

1. 触摸点在视图 A 的区域范围内，然后开始检查子视图 B 和 C

2. 触摸点不在 B 的范围而在 C 的范围，于是就开始检查 D 和 E 视图

3. 触摸点不在 D 的范围而在 E 的范围，而 E 视图是视图树最底层的并包含触摸点的

   视图对象，所以 E 就成为了 hit-test 视图。

- 下面我们尝试实现下这个官方demo
## 小实验：hittest调用顺序

![CF9A1FF4-9776-4D7D-AECE-F9423B46C77D](http://ww3.sinaimg.cn/large/006tNc79ly1g5pqloloogj30br0naq3a.jpg)

- 对于hittest方法，网上有伪代码实现

```objective-c
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
    if (self.hidden || !self.userInteractionEnabled || self.alpha < 0.01 || ![self pointInside:point withEvent:event]) {
        return nil;
    } else {
        for (UIView *subview in [self.subviews reverseObjectEnumerator]) {
            NSLog(@"当前hsubView:%p 当前所在View:%p", subview, self);
            UIView *hitView = [subview hitTest:[subview convertPoint:point fromView:self] withEvent:event];
            if (hitView) {
                NSLog(@"确认hit-TestView:%p 当前所在View:%p", hitView, self);
                return hitView;
            }
        }
        return self;
    }
}
//我们在里面添加其打印，方便我们验证这个流程
//userInteractionEnabled属性为YES，该属性表示允许控件同用户交互。
//Hidden属性为NO。控件都看不见，自然不存在触摸
//opacity属性值0 ～0.01。
//触摸点在这个UIView的范围内。
```




```objective-c
//在最开始直接打印每个View的地址
viewA:0x7fddea408c70, viewB:0x7fddea4061f0, viewC:0x7fddea40b160, viewD:0x7fddea40e090, viewE:0x7fddea40e480

//点击viewD
当前hsubView:0x7fddea40b160 viewC 当前所在View:0x7fddea408c70 viewA
当前hsubView:0x7fddea40e480 viewE 当前所在View:0x7fddea40b160 viewC
当前hsubView:0x7fddea40e090 viewD 当前所在View:0x7fddea40b160 viewC
确认hit-TestView:0x7fddea40e090 viewD 当前所在View:0x7fddea40b160 viewC
确认hit-TestView:0x7fddea40e090 viewD 当前所在View:0x7fddea408c70 viewA
  
当前hsubView:0x7fddea40b160 viewC 当前所在View:0x7fddea408c70 viewA
当前hsubView:0x7fddea40e480 viewE 当前所在View:0x7fddea40b160 viewC
当前hsubView:0x7fddea40e090 viewD 当前所在View:0x7fddea40b160 viewC
确认hit-TestView:0x7fddea40e090 viewD 当前所在View:0x7fddea40b160 viewC
确认hit-TestView:0x7fddea40e090 viewD 当前所在View:0x7fddea408c70 viewA
  
//点击viewE
当前hsubView:0x7fddea40b160 当前所在View:0x7fddea408c70
当前hsubView:0x7fddea40e480 当前所在View:0x7fddea40b160
确认hit-TestView:0x7fddea40e480 当前所在View:0x7fddea40b160
确认hit-TestView:0x7fddea40e480 当前所在View:0x7fddea408c70
 
当前hsubView:0x7fddea40b160 当前所在View:0x7fddea408c70
当前hsubView:0x7fddea40e480 当前所在View:0x7fddea40b160
确认hit-TestView:0x7fddea40e480 当前所在View:0x7fddea40b160
确认hit-TestView:0x7fddea40e480 当前所在View:0x7fddea408c70
```

## hittest调用顺序

- 我们点击viewD的时候，首先UIApplication会把这个触摸事件添加到事件队列里【接收到事件的是UIApplication】
- UIApplication负责处理用户事件，它会起一个队列，把所有用户事件都放入队列，逐个处理，在处理的时候，它会发送当前事件到一个合适的处理事件的目标控件【具体发送时机不明】
- UIApplication会从事件队列中取出最前面的事件，把事件传递给应用程序的主窗口（keyWindow）
- 接着window找到我们当前viewController的self.view，在下一步找到viewA
- 这就进入我们的打印部分，由于是逆序遍历，因此在viewA中第一个锁定的子视图是viewC，检测到viewC中有接触
- 进入viewC，继续逆序，先看的是viewD，发现里面没有，继续看viewC
- 此时发现viewC是的，并且已经没有子视图，直接返回自己
- 同时在viewA那一层的递归也结束，同样返回viewD
- 尽管没有打印，但我们知道接下来的viewControlller同样返回viewD，这样一级一级返回，直到最后的UIApplication，完成整个递归过程

## 问题：为什么我们的hittest走了两次，打印了两遍？

- 这个问题搜了下，发现也有人遇到过
- 目前我找到的原因是[Apple工程师官方回复](https://lists.apple.com/archives/cocoa-dev/2014/Feb/msg00118.html)
- 这里讲了也跟没讲一样，就是说hittest走了两次，每次是在不同的方法栈里被调用，系统会在两次call之间调整hit的点
- 同时他也说道，既然hittest是个无副作用的函数，这个调两次没什么危害
- 另外有意思的是，这个解答我是在[简书](https://www.jianshu.com/p/e08b80856e1a)里的评论区找到的【虽然我认为翻译的不对】
- 之后我在stack overflow里搜过这个问题，虽然也有人问，但没有人解答，第一次发现有stack overflow里没解决，国内解决的
- 反正说白了，知道又这玩意，且无伤大雅就行

## 总结流程

![img](http://ww1.sinaimg.cn/large/006tNc79ly1g5psu30hh0j30gp0cgq5f.jpg)

![img](http://ww3.sinaimg.cn/large/006tNc79ly1g5py5l1bikj30yy0gp0vp.jpg)

# 事件响应

- hitTest等同于是找到了事件发生的初始对象，我们先不讨论设置了第一响应者的情况

![BF9E55A3-A047-4B47-871B-9F68C48C3658](http://ww1.sinaimg.cn/large/006tNc79ly1g5pyl3jfxfj30om0d4gnv.jpg)

- 左边APP响应链：

  1. 初始视图对象尝试对事件进行处理，如果无法处理，就会将事件传递给其父视图对 象，因为视图树中，初始视图对象也并不是最顶端的对象。
  2. 父视图也进行同样的尝试，因为同样的原因也只能将事件继续向上传递。
  3. 视图控制器中最顶层的视图也进行同样的尝试，结果发现也处理不了，于是就传递给了视图控制器。
  4. 视图控制器也一样无法处理，于是继续向上传递给了主窗体对象(window)。
  5. 主窗体也无法处理，于是就继续传递给 app 的单例实体对象。
  6. 如果最后单例实体对象还无法处理，此事件就被丢弃了。
- 右边APP响应链：
  1. 视图将事件沿着其视图控制器的视图树向上传递，直到最顶端的视图。
  2. 顶端视图无法处理，就直接传递给视图控制器。
  3. 视图控制器无法处理，就会将事件传递给其顶端视图所在的父视图。重复 1-3，直到到达最顶端的根视图控制器(root view controller)。
  4. 跟视图控制器将事件传递给主窗体对象。
  5. 主窗体对象传递给 app 的单例实体对象。
- 响应者链的事件传递机制（即触摸事件的响应过程）如下：
  1. 如果 view 是一个 viewController 的 root view，nextResponder 是这个 viewController.如果 view 不是 viewController 的 root view，nextResponder 则是这个 view 的 superview
  2. 如果 viewController 的 view 是 window 的 root view, viewController 的 nextResponder 是这个 window。如果 view controller 是被其他 view controller presented调起来的，那么 view controller 的 nextResponder 就是发起调起的那个 view controller
  3. window 的 nextResponder 是 UIApplication 对象.
  4. 如果UIApplication也不能处理该事件或消息，则将其丢弃

## 实验

- 我们在刚才实验的基础上在自定义的view中重写下touchesBegan方法

```objective-c
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    NSLog(@"Touch:%p", self);
    [super touchesBegan:touches withEvent:event];
}
```

```objective-c
viewA:0x7fdc664061f0, viewB:0x7fdc664072f0, viewC:0x7fdc664074d0, viewD:0x7fdc664076b0, viewE:0x7fdc66407aa0

//点击viewD
当前hsubView:0x7fdc664074d0 当前所在View:0x7fdc664061f0
当前hsubView:0x7fdc66407aa0 当前所在View:0x7fdc664074d0
当前hsubView:0x7fdc664076b0 当前所在View:0x7fdc664074d0
确认hit-TestView:0x7fdc664076b0 当前所在View:0x7fdc664074d0
确认hit-TestView:0x7fdc664076b0 当前所在View:0x7fdc664061f0
当前hsubView:0x7fdc664074d0 当前所在View:0x7fdc664061f0
当前hsubView:0x7fdc66407aa0 当前所在View:0x7fdc664074d0
当前hsubView:0x7fdc664076b0 当前所在View:0x7fdc664074d0
确认hit-TestView:0x7fdc664076b0 当前所在View:0x7fdc664074d0
确认hit-TestView:0x7fdc664076b0 当前所在View:0x7fdc664061f0
Touch:0x7fdc664076b0 viewD
Touch:0x7fdc664074d0 viewC
Touch:0x7fdc664061f0 viewB
```

- 显然，这和我们总结的规律是一样的

## 思考

- 现在我们来思考一个问题，假如我们没有重写自定义view里的touchBegin，而是重写在viewController里

```objective-c
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    NSLog(@"viewContrller");
}
```

- 点击viewD，会不会打印这个viewContrller呢？依然会
- 文档里有这么一段话：如果(事件发生所在的)初始对象(要么是 hit-test 视图，要么是第一响应器)无法对事 件进行处理，UIKit 就会把事件传递给响应器链的下一个响应器对象。每个响应器对象都 可以决定是自己进行事件处理，还是将事件通过方法 nextResponder 的调用，传递给下一 个事件响应器。此过程一直进行下去，直到找到了处理该事件的对象，或者到达了响应器 链的最后一个响应器了。
- 而这个事件怎么看有没有被处理，我的理解就是，这个链其实一直存在，当你在底部view里点击后，他就会顺着链一直往上跑，我们可以通过重写这个方法去阻断这个链，比如我们重写view里的touchBegin，不写[super touchesBegan:touches withEvent:event];，这个链就被阻断了，走不到viewController
- 我们现在来探讨button会不会这样

## button的奇妙冒险

- 我们在红色的view上再添加一个白色的button，【加不加点击事件都行】

![90130DE8-599E-4A1A-8AA1-CDC5ABFF3105](http://ww3.sinaimg.cn/large/006tNc79ly1g5pzvi8jibj30br0najrr.jpg)

- 我们进行点击，其他打印都不变【打印内容太复杂，不贴了】
- 结果就是不管是view里的touchBegan还是controller里的touchBegan都没走【hittest那一部分都走了】
- 因此，UIControl就等于是讲touch进行了包装，他的点击就会直接把事件传递链截断
- 这时，假如我们是在这个button上面添加了一个view又会发生什么呢？

![F996501B-478E-49EC-813D-8879FCBF5E95](http://ww3.sinaimg.cn/large/006tNc79ly1g5q0dhrwocj30br0naq3a.jpg)

- 点击红色的小 view，只会走这个小view的touchBagin，下面的响应依然会被button截断
- 呼，没什么差错
- 当然UIControl只会影响事件响应过程，最开始的hittest没有影响

# 键盘收回

- 在学习UITextField的时候，我们肯定都研究过收回键盘的操作
- 比如：点击空白处收回键盘

```objective-c
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self.view endEditing:YES];
}
```

- 当时就有出现键盘无法收回的情况，当时是说是被别的控件挡住了就不行，现在更正为：如果点击的view到viewController之间有继承于UIControl的控件就会导致该方法失效，实现被截断
- 但当时，为了实现点击键盘上的return，还会写这么一段

```objective-c
//点击键盘上的return，收回键盘
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    // 必须辞去第一响应者后,键盘才会回缩.
    [textField resignFirstResponder];
    return YES;
}
```

- 这个FirstResponder就是啥子东东，和hitTestView有什么关系呢，我们再来研究下

# 第一响应器

- 事件对象路径的最终目标，就是找到能够处理事件并进行响应的对象。因此，UIKit 先会 将事件发送给最适合处理该事件的对象。对于触摸事件，该对象就是 hit-test 视图对象， 对于其他的事件，该对象就是第一响应器。

- Motion and remote control events(运动和远程控制事件)。对于这些事件，主窗 体对象会将摇晃事件或者远程控制事件发送给第一响应器(the first responder)进 行处理。

- 第一响应器被指定第一个接收事件。通常来讲，第一响应器是一个视图 view 对象。通过 做两件事，一个对象就变成了第一响应器:

  1.重写 canBecomeFirstResponder 使其返回 YES;
  2.接收 becomeFirstResponder 消息。如果有必要，对象本身可以自己发送此消息。

- **再将某个对象赋值为第一响应器之前，一定要确保** ***APP*** **已经建立好了对象图谱。比如，通 常应该在重写的** ***viewDidAppear:*****方法中调用** ***becomeFirstResponder*** **方法，但是如果写在了*****viewWillAppear*** **里面，此时因为对象图谱还没有建立起来，*****becomeFirstResponder*** **的返回值就** ***NO*****了。**

- 当用户点击某个 UITextField 或者 UITextView 的时候，UIKit 会自动把将对应的对象设置为 第一响应器。对于其他的第一响应器，App 必须使用 becomeFirstResponder 方法显示地进 行设置。

- 我们一般用到第一响应者就是在UITextField，作为第一响应者，他会展示键盘为，辞去第一响应者，他会收起键盘，下面围绕第一响应者，textField来做一些实验【textField同样继承于UIControl】

## 小实验：这就是我的触发路线，第一响应者！

- 我们先在我们自定义的view中重写canBecomeFirstResponder方法，返回YES，并且讲viewC设置为第一响应者

```objective-c
[viewC becomeFirstResponder];

- (BOOL)canBecomeFirstResponder {
    return YES;
}
```

- 这时再点击上面的viewD，那么按照我的理解，既然viewC是第一响应者，那么hittestView就应该是viewC了
- 可实际上整个流程没有任何变化，还是从viewD开始一直往上
- 这其实就让人非常困惑了，从这个实验结果来看，第一响应者对于触摸事件，没有什么用
- 可能这个第一响应者对于UI最大的用处还是在UItextField里，当用户点击某个 UITextField 或者 UITextView 的时候，UIKit 会自动把将对应的对象设置为 第一响应器。对于其他的第一响应器，App 必须使用 becomeFirstResponder 方法显示地进 行设置。

# 手势控制器

- 有时候开发者可能想要在手势识别器之前，先让视图对象接收触摸事件。但是，在开发者 想要修改视图上的触摸事件传输路径之前，首先得理解事件传输默认的行为。举一个简单 的实例，当触摸事件发生时，UITouches 触摸对象会先由 UIApplication 对象传递给UIWindow 对象，然后，在传递给最底层的视图对象之前，UIWindow 对象会逐层向下， 将“触摸”对象传递给触摸事件发生位置所在的视图对象所绑定的手势识别器进行识别处 理。

![page22image4374528.png](http://ww2.sinaimg.cn/large/006tNc79ly1g5q2n0lrlej313w0ak0vf.jpg) 

- 手势识别器拥有第一个识别处理触摸事件的机会
- window 对象会延迟将“触摸”对象发送给视图对象，从而让手势识别器最先对“触摸” 进行分析处理。在延迟期间，如果识别器识别出来触摸手势，window 对象就不会将“触 摸对象”传递给视图对象了，并将识别出来的手势序列中其他之前本该发送给视图对象的 触摸对象取消掉。
- 举个例子，假如有一个手识别器用来识别非持续的“双击”操作，这个操作会被转换为两 个 UITouch 对象。当触摸事件发生时，这两个对象首先会被传递(触摸事件发生所在的) 视图对象对应的 UIApplication 对象，然后就会触发以下序列消息
- ![page22image4377024.png](http://ww1.sinaimg.cn/large/006tNc79ly1g5q2p0a0abj31400930xo.jpg) 

  1. 在开始(Began)阶段，window 通过方法 touchedBegan:withEvent:将两个触摸对象 发送给手势识别器，手势识别器还没有开始进行识别处理，其状态值为 Possible。 window 将同样的 UITouch“触摸”对象发送给视图绑定的手势识别器。
  2. 在移动(Moved)阶段，window 通过 touchesMoved:withEvent: 将两个触摸对象发 送给手势识别器，手势识别器依然没有检测到手势操作，其状态值为 Possible。 window 将这些的 UITouch“触摸”一个对象发送给视图绑定的手势识别器。
  3. 在某根手指离开屏幕时，就有一个 UITouch 对象处于结束(Ended)阶段，window 通过 touchesEnded:withEvent: 将该触摸对象发送给手势识别器，手势识别器还没有 获取到足够的信息来处理手势操作，其状态值为 Possible。window 自己保留从对 应的视图上抓取的“触摸”对象。
  4. 在另一根手指也离开屏幕，另外一个 UITouch 也处于结束(Ended)阶段，window 将此 UITouch 对象也发送出去。此时，手势识别器(收集到了足够的信息)成功识 别到手势。就在第一个动作消息发送出去之前，视图对象就调用 touchesCancelled:withEvent:方法，就作废掉了 Began 和 Moved 阶段发送过来的触 摸对象。而在 Ended 阶段的触摸操作也就被取消了。
- 现在假定某个手势识别器在最后一步才发现其所分析处理的多点触摸手势无法识别的话， 手势识别器就会将自己的状态设置为 UIGestureRecognizerStateFailed.然后 window 对象再 会在 touchesEnded:withEvent:消息中，将这两个触摸操作对象在结束阶段发送给绑定的视 图对象。
- 对于连续性手势操作的手势识别器也遵照相同的步骤，尽管看起来貌似在触摸对象到达结 束(Ended)阶段之前就能识别出来对应的手势。在识别出对应的手势之前，手势识别器 先将自己的状态设置为 UIGestureRecognizerStateBegan(不是 Recognized)。window 会将 这次多点触摸事件中后续的所有触摸对象都发送给识别器，而不是被绑定的对象。
- 对手势分析器进行修改
  1. delaysTouchesBegan(默认值是 NO)。通常情况下，window 会在 Began 和 Moved 阶 段将触摸事件发送给 view 和手势识别器对象。如果将此属性值设置成 NO， window 就不会在 Began 阶段将“触摸”(UITouch)对象发送给视图对象，这样可 以保证当手势识别器识别到某个手势时，就不有任何相关的 UITouch 对象被发送给 绑定的视图。注意对该属性进行设置时，可能会让界面看起来没有什么被“触摸” 的视觉效果。
该属性值的设置类似于 UIScrollView 的 delaysContentTouches 属性;在这种情况 下，UIScrollView 就立即随着用户“触摸”动作的进行滚动，而不会将“触摸” (UITouch)对象发送给 SrollView 的子视图对象，所以也就不会有视觉上的反馈效 果。
  2. delaysTouchesEnded(默认值为 YES)。当该属性被设置成 YES 时，可以保证视图对象 的动作处理不会结束，这样一来该手势动作还有机会被取消。当手势识别器对触摸 事件进行分析时，window 对象不会将 UITouch 对象发送给在 End 阶段发送给绑定
的视图。如果识别器成功识别出来手势操作，UITouch 对象会被取消掉;若是识别 失败，window 对象就会将它们通过消息 touchesEnded:wihtEvent:发送给视图对 象。如果将该属性值设置成 NO，就会把这些“触摸”(UITouch)对象发送给手势 识别器的同时，也发送给视图对象进行分析处理。
设想一下，某个视图对象 view 有一个“点击”(tap)手势识别器可以识别“双 击”手势，而用户也刚好双击了。该属性值为 YES 时，视图对象按顺序收到以下几 个消息方法都会被调用 touchesBegan:withEvent:, touchesBegan:withEvent:,touchesCancelled:withEvent:, touchesCancelled:withEvent:,如 果被设置成了 NO，对应按顺序收到的消息序列就变成:touchesBegan:withEvent:, touchesEnded:withEvent:, touchesBegan:withEvent:, touchesCancelled:withEvent:,也就 意味着，在 touchesBegan:withEvent:消息处理事件中，视图对象就可以识别出来双 击”手势事件。
  
- 如果手势识别器检测到对应的“触摸”(UITouch)对象不属于对应“手势”操作的一部 分，该对象就会被直接发送给对应的视图对象。要达到这种效果，识别器会调用ignoreTouch:forEvent: 方法消息，把对应的“触摸”对象传递出去。

# CALayer的hitTest方法

- 是的，没想到吧，连继承于NSObject，专门负责做展示工作的的CALayer浓眉大眼也有hittest方法

```objective-c
- (CALayer *)hitTest:(CGPoint)thePoint

//Returns the farthest descendant of the receiver in the layer hierarchy (including itself) that contains a specified point.
```

- 和View的hittest方法不同，它是用来返回calayer层的最远子
- 具体实操可以看这篇[第二篇：CALayer能力之hitTest响应事件](https://www.jianshu.com/p/d7c6bcbc2312)

# 異議あり！

- 我们来看下某位大神的观点

![11DD55685B256F707DB1501410FB6F5D](http://ww2.sinaimg.cn/large/006tNc79ly1g5q8afvnrxj30oo04kwg1.jpg)

## Q&A

Q：UIView默认不会响应事件？

A：UIView等控件都继承于UIResponder，只要继承于UIResponder的都是响应者，绝壁都能响应事件。

Q：UIView没有实现touchesBegan方法？

A：。。。touches四兄弟都是UIResponder里的方法，只要继承了，肯定就实现了呀？？？【感觉不太聪明的亚子】

Q：我丢，照你这么说这么理解呀！

A：其实问题在于你怎么理解响应事件。翻译下这位大神想要表示的意思：UIControl在响应了事件后，会终止响应链，不再往下传。事实确实如此，但对于UIView，其实每一个都响应接收到事件了，只是它没有进行相应的处理，直接往下传了而已。

# 参考文章

**赞美太阳！**

- [第二篇：CALayer能力之hitTest响应事件](https://www.jianshu.com/p/d7c6bcbc2312)
- [一篇搞定事件传递、响应者链条、hitTest和pointInside的使用](https://www.jianshu.com/p/2f664e71c527)
- [史上最详细的iOS之事件的传递和响应机制-原理篇](https://www.jianshu.com/p/2e074db792ba)
- [iOS Touch以及事件响应链（一）hit-testing](https://www.jianshu.com/p/4f1a8e60bed4)
- [浅谈iOS中的事件处理和响应者链](https://blog.csdn.net/qq_42347755/article/details/98523801)




