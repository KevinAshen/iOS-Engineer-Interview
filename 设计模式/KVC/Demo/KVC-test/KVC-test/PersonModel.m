//
//  PersonModel.m
//  KVC-test
//
//  Created by _祀梦 on 2019/7/30.
//  Copyright © 2019 涂强尧. All rights reserved.
//

#import "PersonModel.h"

@implementation PersonModel

-(void)setValue:(id)value forUndefinedKey:(NSString *)key{
    //2.2时
    //这里我们不需要写任何内容
    //2.3时
    if ([key isEqualToString:@"key2"]) {
        self.id = value;
    }
}

@end
