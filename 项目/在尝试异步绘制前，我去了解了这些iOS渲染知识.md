[TOC]

# 前言

- 这一块主要为了面试讲项目准备，尽量理论与实践相结合吧，能把搜到的资料都实现最好
- 目前主要是为tableView的优化进行研究，把涉及到的异步绘制【图层问题】，网络请求中的cell中空间高度缓存这些就用到最好

# FPS

- FPS是图像领域中的定义，是指画面每秒传输帧数，通俗来讲就是指动画或视频的画面数。FPS是测量用于保存、显示动态视频的信息数量。每秒钟帧数越多，所显示的动作就会越流畅。通常，要避免动作不流畅的最低是30。
- iPhone的FPS为60，也就是说它在一秒钟内会刷新60次，每次间隔大概16.7ms，也就是说我们的iPhone每次有16.7ms的时间来处理事件
- 而如果在这个时间内没能成功刷新，就会直接丢弃这一帧，下一次一起整上去【也就是说，不会原地刷新】
- 这样子帧的丢失反映到用到用户体验上就 = APP卡顿了

# CPU

- 中央处理器（CPU，central processing unit）作为计算机系统的运算和控制核心，是信息处理、程序运行的最终执行单元
- 放在我们的iOS中，可以理解为CPU负责软件的操作，对象创建、对象调整、对象销毁、布局计算、文本计算、文本渲染、图片解码、图像的绘制等操作是放在CPU上执行的

# GPU

- 图形处理器（英语：Graphics Processing Unit，缩写：GPU），又称显示核心、视觉处理器、显示芯片，是一种专门在个人电脑、工作站、游戏机和一些移动设备（如平板电脑、智能手机等）上做图像和图形相关运算工作的微处理器。
- 放在iOS中，GPU处理的是硬件层面的事，纹理的渲染、视图混合、离屏渲染都是在GPU上执行的

# 位图

- 位图是由称作像素（图片元素）的单个点组成的。这些点可以进行不同的排列和染色以构成图样。当放大位图时，可以看见赖以构成整个图像的无数单个方块。扩大位图尺寸的效果是增大单个像素，从而使线条和形状显得参差不齐。然而，如果从稍远的位置观看它，位图图像的颜色和形状又显得是连续的。用数码相机拍摄的照片、扫描仪扫描的图片以及计算机截屏图等都属于位图。位图的特点是可以表现色彩的变化和颜色的细微过渡，产生逼真的效果，缺点是在保存时需要记录每一个像素的位置和颜色值，占用较大的存储空间。常用的位图处理软件有Photoshop（同时也包含矢量功能）、Painter和Windows系统自带的画图工具等，Adobe Illustrator则是矢量图软件

# 像素点如何出现在屏幕上

- 先来看一张全部流程图：

![pixels-software-stack](https://tva1.sinaimg.cn/large/006y8mN6ly1g8my2kdghhj30ht06rgli.jpg)

- 这张图从从右到左就是软件到硬件，从App到硬件屏幕上出现界面的全过程
- 这里再介绍下前面没介绍的模块

## 渲染参与者

- GPU Driver：GPU驱动软件，直接和 GPU 交流的代码块
- OpenGL：提供了 2D 和 3D 图形渲染的 API，高GPU的能力，并实现硬件加速渲染，是第一个和图形硬件(GPU)交流的标准化方式
- Core Graphics：Quartz 2D的一个高级绘图引擎，常用与iOS，tvOS，macOS的图形绘制应用开发。Core Graphics是对底层C语言的一个简单封装，其中提供大量的低层次，轻量级的2D渲染API。前缀为CG，比如常见的CGPath，CGColor
- Core Animation：是苹果提供的一套基于绘图的动画框架，但不止是动画，他同样是绘图的根本【因为其前缀是CA，CALayer有多重要就不用多说了吧】

![6379271-f8367726caa19e8a](https://upload-images.jianshu.io/upload_images/6379271-f8367726caa19e8a.png)

- 从图中可以看出，最底层是图形硬件(GPU)；上层是OpenGL和CoreGraphics，提供一些接口来访问GPU；再上层的CoreAnimation在此基础上封装了一套动画的API。最上面的UIKit属于应用层，处理与用户的交互
- Core Image：iOS处理图像的框架，主要用处可以给图片添加滤镜效果和图像识别功能

## we are A！R！G！B！

- 这里讲下对于像素需要知道的知识：
- 屏幕上的像素是由红，绿，蓝三种颜色组件构成的。因此，位图数据有时也被叫做 RGB 数据
- ARGB：32bits-per-pixel(bpp), 8bits-per-componet(bpc),透明度会首先被乘以到像素值上【也就是说对于透明度的处理我们直接就是百分比乘到RGB值里面】

```
  A   R   G   B   A   R   G   B   A   R   G   B  
| pixel 0       | pixel 1       | pixel 2   
  0   1   2   3   4   5   6   7   8   9   10  11 ...
  
这个格式经常被叫做 ARGB。每个像素占用 4 字节(32bpp),每一个颜色组件是1字节(8bpc).每个像素有一个 alpha 值，这个值总是最先得到的(在RGB值之前)，最终红、绿、蓝的值都会被预先乘以 alpha 的值

如果我们有一个橙色，他们各自的 8bpc 就像这样: 240,99,24.一个完全不透明的橙色像素拥有的 ARGB 值为: 255，240，99，24，它在内存中的布局就像上面图示那样。如果我们有一个相同颜色的像素，但是 alpha 值为 33%，那么他的像素值便是:84，80，33，8.
```

- xRGB：像素并没有任何 alpha 值(他们都被假定为100%不透明)，但是内存布局是一样的，这样子将会节约内存【？？？：这种格式容易被现代的 CPU 和绘图算法消化，因为每一个独立的像素都对齐到 32-bit 的边界】

```
  x   R   G   B   x   R   G   B   x   R   G   B  
| pixel 0       | pixel 1       | pixel 2   
  0   1   2   3   4   5   6   7   8   9   10  11 ...
```

- xRGB即没有alpha通道的图片，它无法选择透明度，虽然我们我们说有alpha通道的像素点其实就是将alpha预乘到了RGB里面，但这不意味着alpha没用，在合成像素点的时候我们依然需要用alpha值计算混合后的值，所以对于这种图片就是无法设置透明度

## 合成

- 在有多个图层重叠的情况下，屏幕上的人看到的一个像素点可能是好几个像素点合成后的结果，这就需要我们去计算出现的像素应该是什么造型
- 下面是理想状态下的像素合成公式：

```
R = S + D * ( 1 – Sa )

结果的颜色 = 源色彩(顶端纹理) + 目标颜色(低一层的纹理) * (1 - 源颜色的透明度)
```

- 但理想状态很少见，更多情况下，我们需要进行更为复杂的计算

### 透明与不透明

- 这里先区分三个概念

#### Alpha，Hidden与Opaque区别

- alpha是不透明度，属性为浮点类型的值，取值范围从0到1.0，表示从完全透明到完全不透明，其特性有当前UIView的alpha值会被其所有subview继承。alpha值会影响到UIView跟其所有subview，alpha具有动画效果。当alpha为0时，跟hidden为YES时效果一样，但是alpha主要用于实现隐藏的动画效果，在动画块中将hidden设置为YES没有动画效果
- 设置backgroundColor的alpha值只影响当前UIView的背景，并不会影响其所有subview。Clear Color就是backgroundColor的alpha为1.0。alpha值会影响backgroundColor最终的alpha,假设UIView的alpha为0.8，backgroundColor的alpha为0.5，那么backgroundColor最终的alpha为0.4(0.8*0.5)
- Hidden表示UIView是否隐藏，Hidden设置为YES表示当前UIView的所有subview也会被隐藏，忽略subview的hidden属性。Hidden只要设置为YES，所有的subview都会隐藏。UIView隐藏之后也会从当前的响应者事件中移除
- opaque表示当前的UIView的是否不透明【BOOL值】，但它就是涉及到了像素渲染问题，对于opaque为1的图层，将会直接显示，不会再去计算合成

#### 合成区别

- 显然，尽量多用opaque 为 YES的图层可以节约GPU的消耗，加快渲染时间
- 如果你加载一个没有 alpha 通道的图片，并且将它显示在 UIImageView 上，会自动设置opaque 为 YES
- 一个图片没有 alpha 通道和一个图片每个地方的 alpha 都是100%，这将会产生很大的不同。在后一种情况下，Core Animation 需要假定是否存在像素的 alpha 值不为100%，也就是说依然需要进行运算

### 对齐与不对齐

- 如果几个图层的模版都是完美重合，那我们只要从第一个像素到最后一个像素都计算合成一下
- 但是如果像素没有对齐好，我们还需要额外进行额外的移位操作，合并原纹理上的像素
- 两种情况会导致不对齐出现：
  - 第一个便是缩放；当一个纹理放大缩小的时候，纹理的像素便不会和屏幕的像素排列对齐
  - 另一个原因便是当纹理的起点不在一个像素的边界上

# 离屏渲染(Offscreen Rendering)

- 在屏幕中，我们要显示的位图直接存在当前屏幕的后备存储区，同时这一块内容将会很快被刷新到屏幕上呈现
- 但加入我们需要写一个简单的平移动画，假设有60帧动画，那我们就需要重复生成60个图层，并应用
- 这是因为屏幕就这么大，我们放在当前屏幕的后备存储区必须得是即将放上去的内容
- 这样的操作方式显然很浪费GPU，所以遇到动画Core Animation会自动触发离屏渲染

## 什么是离屏渲染(Offscreen Rendering)

### CRT显示器

- CRT显示器是靠电子束激发屏幕内表面的荧光粉来显示图像的，由于荧光粉被点亮后很快会熄灭，所以电子枪必须循环地不断激发这些点

- 首先，在荧光屏上涂满了按一定方式紧密排列的红、绿、蓝三种颜色的荧光粉点或荧光粉条，称为荧光粉单元，相邻的红、绿、蓝荧光粉单元各一个为一组，学名称之为像素。每个像素中都拥有红、绿、蓝(R、G、B)三基色

### 显示原理

- 显示器显示出来的图像是经过 CRT电子枪一行一行的扫描.(可以是横向的也可以是纵向),扫描出来就呈现了一帧画面,随后电子枪又会回到初始位置循环扫描,为了让显示器的显示跟视频控制器同步,当电子枪新扫描一行的时候.准备扫描的时候,会发送一个 水平同步信号(HSync信号),而当一帧画面绘制完成后,电子枪回复到原位，准备画下一帧前，显示器会发出一个垂直同步信号（vertical synchronization简称 VSync）
- 显示器一般是固定刷新频率的,这个刷新的频率其实就是VSync信号产生的频率. 然后CPU计算好frame等属性,就将计算好的内容提交给GPU去渲染,GPU渲染完成之后就会放入帧缓冲区,然后视频控制器会按照VSync信号逐行读取帧缓冲区的数据,经过可能的数模转换传递给显示器.就显示出来了

### 离屏渲染原理

- 离屏渲染也就是说在屏幕外划出一片缓存区作为屏幕外缓存区，用来存储需要渲染又不能马上放入屏幕内缓存区的纹理，这块大小大约有屏幕大小两倍的空间
- 这种做法的实质就是将CPU拉来做GPU的事，让CPU在屏幕外进行渲染工作【虽然CPU性能比GPU强得多，但是在渲染这方面还不如GPU，用杀鸡焉用牛刀的感觉】
- 离屏渲染这个机制出现的原因在于Apple对于流畅度要求很高，宁愿用空间【性能】换时间
- 但这是一种消耗极大的方式，不仅是CPU渲染有开销，还包括两次昂贵的环境转换(转换环境到屏幕外缓冲区，然后转换环境到帧缓冲区)
- 这样的好处就是可以复用，避免不必要的GPU渲染
- 另外复用也不是随意复用的，这个可以被复用的位图同样是只能存在一段时间，之后会被卸载掉，你需要计算GPU 的利用率和帧的速率来判断这个位图是否有用

# 调试方式

- 调试主推荐Instrument里面的Core Animation工具
- 它可以查看项目的fps，GPU占用率

![5FAD78E6BB6CC85AD123A7F166D02A18](https://tva1.sinaimg.cn/large/006y8mN6ly1g7z5soq5fej30zo0nljuo.jpg)

- 但是我遇到了一个问题就是这个工具只能在真机上用，在模拟机上都说该设备不支持这个操作，不知道是哪一步整错了
- 另一个是Xcode -> Debug > View Debugging > Rendering大法

![截屏2019-10-15下午8.33.40](https://tva1.sinaimg.cn/large/006y8mN6ly1g7z5wmfdt6j30o50k1770.jpg)

- 这个请注意要在运行的时候才能点开
- Color Blended Layers  出现图层混合的地方会标注为红色，没有图层混合的地方会显示为绿色，方向是红色越少越好，绿色越多越好
- Color Hits Green and Misses Red  当使用光栅化渲染(shouldRasterize)的时候，如果图层是绿色，表示这些缓存被复用，如果图层是红色表示缓存没有被复用会重复创建，这时候会造成性能问题。
- 一般这两个用的比较多
- 还有一个Color Offscreen-rendered Yellow 用来检测是否离屏渲染【off-screen Rendering】
  - GPU的渲染有两种，On-screen Rendering当前屏幕渲染，是指GPU的渲染在当前屏幕的缓冲区内进行。off-screen Rendering是指在GPU的渲染发生在当前屏幕之外新开辟的缓冲区。开辟新的缓冲区，切换缓冲区等会对性能有较大的影响
  - 同时，有七种情况会触发离屏渲染
    1. cornerRadius以及masksToBounds同时使用时会触发离屏渲染，单独使用时不会触发
    2. 设置shadow，而且shodowPath = nil时会触发
    3. mask 设置蒙版会触发
    4. layer.shouldRasterize的不适当使用会触发离屏渲染
    5. layer.allowsGroupOpacity iOS7以后默认开启；当layer.opacity != 1.0且有subLayer或者背景图时会触发
    6. layer.allowsEdgeAntialiasing 在iOS8以后的系统里可能已经做了优化，并不会触发离屏渲染，不会对性能造成影响
    7. 重写了drawRect

# iOS绘制

## CALayer与UIView

<img src="https://tva1.sinaimg.cn/large/006y8mN6ly1g8jqfuqu0wj30eg03paam.jpg" alt="img" style="zoom:200%;" />

- 每一个UIView都有一个layer，每一个layer都有个content，这个content指向的是一块缓存，叫做backing store
- 在没有重写drawRect方法的情况下，CALayer的content为空，重写后系统会为该layer的content开辟一块缓存，大小`size = width * height * scale`，用来存放drawRect绘制的内容【就算重写内容为空也会新建缓存，注意】

### 代码验证

```objective-c
//
//  MyView.m
//  TableView-Optimizing-ChemeDemo
//
//  Created by Kevin.J on 2019/11/2.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "MyView.h"

@implementation MyView

-(void)drawRect:(CGRect)rect{
    NSLog(@"4MyView.before.drawRect.layer.contents %@",self.layer.contents);

//   CGContextRef context = UIGraphicsGetCurrentContext();
//   draw something

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSLog(@"3MyView.after.drawRect.layer.contents %@",self.layer.contents);
    });
}

@end

  
//
//  ViewController.m
//  TableView-Optimizing-ChemeDemo
//
//  Created by Kevin.J on 2019/10/8.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "ViewController.h"
#import "AsyncLabel.h"
#import "MyView.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //AsyncLabel *label = [[UILabel alloc] init];
    MyView* myView = [[MyView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSLog(@"1MyView.after.drawRect.layer.contents %@",myView.layer.contents);
    });
    [self.view addSubview:myView];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSLog(@"2MyView.after.drawRect.layer.contents %@",myView.layer.contents);
    });
}


@end
```

```objective-c
输出的顺序为：
4MyView.before.drawRect.layer.contents (null)
1MyView.after.drawRect.layer.contents <CABackingStore 0x7f9aa8f00ff0>
2MyView.after.drawRect.layer.contents <CABackingStore 0x7f9aa8f00ff0>
3MyView.after.drawRect.layer.contents <CABackingStore 0x7f9aa8f00ff0>
```

### 绘制细节

- CALayer提供了三种绘制内容的方式，若采用优先级高的方式绘制了内容，则低优先级的则不会执行

![img](https://tva1.sinaimg.cn/large/006y8mN6ly1g8jrisnsqmj30op0b5ta9.jpg)

## 绘制时机

- 当在操作 UI 时，比如改变了 Frame、更新了 UIView/CALayer 的层次时，或者手动调用了 UIView/CALayer 的 setNeedsLayout/setNeedsDisplay方法后，这个 UIView/CALayer 就被标记为待处理，并被提交到一个全局的容器去。
- 苹果注册了一个 Observer 监听 BeforeWaiting(即将进入休眠) 和 Exit (即将退出Loop) 事件，回调去执行一个很长的函数：
-  _ZN2CA11Transaction17observer_callbackEP19__CFRunLoopObservermPv()。这个函数里会遍历所有待处理的 UIView/CAlayer 以执行实际的绘制和调整，并更新 UI 界面
- 当UIView被绘制时（从 CA::Transaction::commit:以后），CPU执行drawRect，通过context将数据写入backing store。当backing store写完后，通过render server交给GPU去渲染，将backing store中的bitmap数据显示在屏幕上 

# 参考文章

1. [iOS性能优化（初级）](https://www.jianshu.com/p/2622adf31ec3)
2. [iOS性能优化（中级）](https://www.jianshu.com/p/ee67b17dbf19)
3. [iOS性能优化（中级+）: 异步绘制](https://www.jianshu.com/p/6634dbdf2964)
4. [iOS绘制与渲染--CPU绘制](https://www.jianshu.com/p/bd560ec64b7c)
5. [iOS-CPU&&GPU分别做什么？](https://www.jianshu.com/p/1b06071f56bc)
6. [CoreText是如何绘制文本的](https://www.jianshu.com/p/e0277ac633bc)
7. [【重读iOS】认识CALayer](https://www.jianshu.com/p/e3c118e56c9a)
8. [为什么必须在主线程操作UI](https://blog.csdn.net/zhangweiacm/article/details/87603428)
9. [异步绘制](https://blog.csdn.net/jks456/article/details/50483158)
10. [位图：百度百科]([https://baike.baidu.com/item/%E4%BD%8D%E5%9B%BE/1017781?fr=aladdin](https://baike.baidu.com/item/位图/1017781?fr=aladdin))
11. [绘制像素到屏幕上](https://objccn.io/issue-3-1/)
12. [iOS开发-Alpha，Hidden与Opaque区别](https://www.jianshu.com/p/7839178ca249)
13. [[技巧]UIView的hidden和alpha的妙用](https://blog.csdn.net/hherima/article/details/75049973)
14. [关于离屏渲染](https://www.jianshu.com/p/24dac847cfc4)
15. [iOS-离屏渲染详解](https://www.jianshu.com/p/57e2ec17585b)
16. [离屏渲染优化详解：实例示范+性能测试](https://www.jianshu.com/p/ca51c9d3575b)
17. [浅谈iOS中的视图优化](http://kuailejim.com/2016/04/22/浅谈iOS中的视图优化)