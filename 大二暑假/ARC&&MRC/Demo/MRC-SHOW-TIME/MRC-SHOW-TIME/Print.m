//
//  Print.m
//  MRC-SHOW-TIME
//
//  Created by Kevin.J on 2019/7/6.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "Print.h"

@implementation Print

+ (void)printRetainCount:(NSObject *)obj {
    NSLog(@"RetainCount:%ld", obj.retainCount);
}

@end
