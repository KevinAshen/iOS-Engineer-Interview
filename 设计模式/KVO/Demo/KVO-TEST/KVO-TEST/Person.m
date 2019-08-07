//
//  Person.m
//  KVO-TEST
//
//  Created by Kevin.J on 2019/8/7.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import "Person.h"

@implementation Person

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.changeStr = @"init";
    }
    return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    if ([keyPath isEqual:@"testStr"]) {
        NSLog(@"oldValue:%@", [change objectForKey:@"old"]);
        NSLog(@"newValue:%@", [change objectForKey:@"new"]);
    }
}

@end
