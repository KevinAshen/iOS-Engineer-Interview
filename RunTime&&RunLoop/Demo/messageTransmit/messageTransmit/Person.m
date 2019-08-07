//
//  Person.m
//  messageTransmit
//
//  Created by Kevin.J on 2019/7/30.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "Person.h"
#import <objc/runtime.h>

@implementation Person

- (void)eat:(NSInteger)num {
NSLog(@"Person:eat:(NSInteger)num:%ld", num);
}

//+ (BOOL)resolveInstanceMethod:(SEL)sel
//{
//    if (sel == @selector(eat:)) {
//        class_addMethod(self, sel, (IMP)Ceat, "v@:I");
//        return YES;
//    }
//   
//    
//    return [super resolveInstanceMethod:sel];
//}
//
//void Ceat(id self, SEL cmd, NSInteger num) {
//    NSLog(@"void Ceat(id self, SEL cmd, int num):");
//    NSLog(@"%@, %s, %ld", [self class], sel_getName(cmd), num);
//}

//- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
//    NSString *selStr = NSStringFromSelector(aSelector);
//    if ([selStr isEqualToString:@"eat"]) {
//        return [NSMethodSignature signatureWithObjCTypes:"v@:"];
//    }
//    
//    return [super methodSignatureForSelector:aSelector];
//}

@end
