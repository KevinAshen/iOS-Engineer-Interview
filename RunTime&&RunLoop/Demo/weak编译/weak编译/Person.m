//
//  Person.m
//  weak编译
//
//  Created by Kevin.J on 2019/7/29.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "Person.h"

@implementation Person

- (void)eat {
    NSLog(@"Person EAT");
}

//- (void)eat:(NSString *)str {
//    NSLog(@"Person EATSTR");
//}

- (void)sleep:(NSInteger)num {
    NSLog(@"%ld", num);
}

- (void)dayin {
    NSLog(@"dayin");
    SEL sell1 = @selector(eat);
    NSLog(@"sell1:%p", sell1);
    IMP imp1 = [self methodForSelector:sell1];
    NSLog(@"imp1:%p", imp1);
    SEL sell2 = @selector(eat:);
    NSLog(@"sell2:%p", sell2);
    IMP imp2 = [self methodForSelector:sell2];
    NSLog(@"imp2:%p", imp2);
    SEL sell3 = @selector(drink:);
    NSLog(@"sell3:%p", sell3);
    IMP imp3 = [self methodForSelector:sell3];
    NSLog(@"imp3:%p", imp3);
}
//2019-07-29 21:09:22.053485+0800 weak编译[12090:711981] sell1:0x100000f51
//2019-07-29 21:09:22.053501+0800 weak编译[12090:711981] imp1:0x7fff7c89c300
//2019-07-29 21:09:22.053508+0800 weak编译[12090:711981] sell2:0x100000f68
//2019-07-29 21:09:22.053516+0800 weak编译[12090:711981] imp2:0x7fff7c89c300
//2019-07-29 21:09:22.053522+0800 weak编译[12090:711981] sell3:0x100000f6d
//2019-07-29 21:09:22.053528+0800 weak编译[12090:711981] imp3:0x7fff7c89c300
@end
