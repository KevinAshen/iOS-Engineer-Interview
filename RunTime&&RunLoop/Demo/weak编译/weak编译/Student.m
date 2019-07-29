//
//  Student.m
//  weak编译
//
//  Created by Kevin.J on 2019/7/29.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "Student.h"

@implementation Student

//- (void)eat:(NSInteger)num {
//    NSLog(@"EATNUM");
//}

- (void)dayin {
    SEL sell1 = @selector(sleep:);
    NSLog(@"%p", sell1);
    
    IMP imp1 = [self methodForSelector:sell1];
    
    
}



@end
