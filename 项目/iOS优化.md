[TOC]

# 前言

- 这一块主要为了面试讲项目准备，尽量理论与实践相结合吧，能把搜到的资料都实现最好
- 目前主要是为tableView的优化进行研究，把涉及到的异步绘制【图层问题】，网络请求中的cell中空间高度缓存这些就用到最好

# 基础知识

## FPS

- FPS是图像领域中的定义，是指画面每秒传输帧数，通俗来讲就是指动画或视频的画面数。FPS是测量用于保存、显示动态视频的信息数量。每秒钟帧数越多，所显示的动作就会越流畅。通常，要避免动作不流畅的最低是30。
- iPhone的FPS为60，也就是说它在一秒钟内会刷新60次，每次间隔大概16.7ms，也就是说我们的iPhone每次有16.7ms的时间来处理事件
- 而如果在这个时间内没能成功刷新，就会直接丢弃这一帧，下一次一起整上去【也就是说，不会原地刷新】
- 这样子帧的丢失反映到用到用户体验上就 = APP卡顿了

## CPU

- 中央处理器（CPU，central processing unit）作为计算机系统的运算和控制核心，是信息处理、程序运行的最终执行单元
- 放在我们的iOS中，可以理解为CPU负责软件的操作，对象创建、对象调整、对象销毁、布局计算、文本计算、文本渲染、图片解码、图像的绘制等操作是放在CPU上执行的

## GPU

- 图形处理器（英语：Graphics Processing Unit，缩写：GPU），又称显示核心、视觉处理器、显示芯片，是一种专门在个人电脑、工作站、游戏机和一些移动设备（如平板电脑、智能手机等）上做图像和图形相关运算工作的微处理器。
- 放在iOS中，GPU处理的是硬件层面的事，纹理的渲染、视图混合、离屏渲染都是在GPU上执行的

## 调试方式

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

## 异步绘制

- 我们从异步绘制这一块开始，因为我对这个比较感兴趣
- 还是先来波知识储备吧，不然都不知道自己在干嘛 

# 参考文章

1. [iOS性能优化（初级）](https://www.jianshu.com/p/2622adf31ec3)
2. [iOS性能优化（中级）](https://www.jianshu.com/p/ee67b17dbf19)
3. [iOS性能优化（中级+）: 异步绘制](https://www.jianshu.com/p/6634dbdf2964)
4. [iOS绘制与渲染--CPU绘制](https://www.jianshu.com/p/bd560ec64b7c)
5. [iOS-CPU&&GPU分别做什么？](https://www.jianshu.com/p/1b06071f56bc)
6. [CoreText是如何绘制文本的](https://www.jianshu.com/p/e0277ac633bc)