//
//  Person.m
//  KVO-test
//
//  Created by _祀梦 on 2019/8/1.
//  Copyright © 2019 涂强尧. All rights reserved.
//

#import "Person.h"

@implementation Person

- (void)setAge:(NSInteger)age{
    
    _age = age;
    
    NSLog(@"setAge:");
}

- (void)willChangeValueForKey:(NSString *)key{
    
    [super willChangeValueForKey:key];
    
    NSLog(@"willChangeValueForKey");
}

- (void)didChangeValueForKey:(NSString *)key{
    
    NSLog(@"didChangeValueForKey - begin");
    
    [super didChangeValueForKey:key];
    
    NSLog(@"didChangeValueForKey - end");
}
@end
