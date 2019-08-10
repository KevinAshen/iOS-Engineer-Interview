//
//  RTTouchView.m
//  Responder-Test
//
//  Created by Kevin.J on 2019/8/6.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "RTTouchView.h"

@implementation RTTouchView

//- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
//    NSLog(@"Touch:%p", self);
//    [super touchesBegan:touches withEvent:event];
//}

- (BOOL)canBecomeFirstResponder {
    return YES;
}

//- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
//
//
//
//    UIView *tmpView = [super hitTest:point withEvent:event];
//    NSLog(@"当前hit-TestView:%p 当前所在View:%p", tmpView, self);
//    return tmpView;
//}


//- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
//{
//    //NSLog(@"%p", event);
//    if (self.hidden || !self.userInteractionEnabled || self.alpha < 0.01 || ![self pointInside:point withEvent:event] || ![self isUserInteractionEnabled]) {
//        return nil;
//    } else {
//        for (UIView *subview in [self.subviews reverseObjectEnumerator]) {
//            NSLog(@"当前hsubView:%p 当前所在View:%p", subview, self);
//            UIView *hitView = [subview hitTest:[subview convertPoint:point fromView:self] withEvent:event];
//            if (hitView) {
//                NSLog(@"确认hit-TestView:%p 当前所在View:%p", hitView, self);
//                return hitView;
//            }
//        }
//        return self;
//    }
//}

@end
