//
//  Student.m
//  messageTransmit
//
//  Created by Kevin.J on 2019/7/30.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "Student.h"
#import <objc/runtime.h>

@implementation Student

//- (void)eat:(NSString *)str {
//    NSLog(@"Student:eat:(NSString *)str:%@", str);
//}

+ (BOOL)resolveInstanceMethod:(SEL)sel
{
    if (sel == @selector(eat:)) {
        class_addMethod(self, sel, (IMP)Ceat, "v@:I");
        return YES;
    }


    return [super resolveInstanceMethod:sel];
}

void Ceat(id self, SEL cmd, NSInteger num) {
    NSLog(@"void Ceat(id self, SEL cmd, int num):");
    NSLog(@"%@, %s, %ld", [self class], sel_getName(cmd), num);
}

//- (void)testJKW {
//    NSLog(@"TQYSD");
//}

- (void)printJKW {
    SEL sel = @selector(testJKW);
    IMP IMP1 = [self methodForSelector:sel];
    NSLog(@"Student:%p", IMP1);
}


@end
