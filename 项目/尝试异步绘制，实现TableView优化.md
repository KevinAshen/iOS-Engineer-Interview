[TOC]

# 异步绘制实战

## 简单一个View

- 本段代码来自于：[iOS性能优化（中级+）: 异步绘制](https://www.jianshu.com/p/6634dbdf2964)

```objective-c
// 新建一个View叫做AsyncLabel
// 重写他的displayLayer方法，在他里面进行异步绘制
- (void)displayLayer:(CALayer *)layer {
    
    NSLog(@"是不是主线程呢？ %d", [[NSThread currentThread] isMainThread]);
    // 输出1代表是主线程
    // 异步绘制，所以我们在使用了全局子队列，实际使用中，最好使用自创队列
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        
        __block CGSize size = CGSizeZero;
        __block CGFloat scale = 1.0;
        dispatch_sync(dispatch_get_main_queue(), ^{
          	// 获取bounds必须在主线程中才能获取
            size = self.bounds.size;
            scale = [UIScreen mainScreen].scale;
        });
        UIGraphicsBeginImageContextWithOptions(size, NO, scale);
        CGContextRef context = UIGraphicsGetCurrentContext();
       //draw
        [self draw:context size:size];

        UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        dispatch_async(dispatch_get_main_queue(), ^{
        self.layer.contents = (__bridge id)(image.CGImage);
        });
    });
}

- (void)draw:(CGContextRef)context size:(CGSize)size {
    //将坐标系上下翻转。因为底层坐标系和UIKit的坐标系原点位置不同。
    CGContextSetTextMatrix(context, CGAffineTransformIdentity);
    CGContextTranslateCTM(context, 0, size.height);
    CGContextScaleCTM(context, 1.0, -1.0);
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathAddRect(path, NULL, CGRectMake(0, 0, size.width, size.height));
    
    //设置内容
    NSMutableAttributedString * attString = [[NSMutableAttributedString alloc] initWithString:self.text];
    //设置字体
    [attString addAttribute:NSFontAttributeName value:self.font range:NSMakeRange(0, self.text.length)];
    
    CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString((CFAttributedStringRef)attString);
    CTFrameRef frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, attString.length), path, NULL);
    
    //把frame绘制到context里
    CTFrameDraw(frame, context);
}
```



## 简单一个cell



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