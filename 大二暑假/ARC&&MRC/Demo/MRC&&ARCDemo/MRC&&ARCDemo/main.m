//
//  main.m
//  MRC&&ARCDemo
//
//  Created by Kevin.J on 2019/7/6.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        
        NSObject *obj0 = [[NSObject alloc] init];
        NSObject *obj1 = [[NSObject alloc] init];
        NSObject *obj2 = nil;
        obj0 = obj1;//obj0强引用对象B；而对象A不再被ojb0引用，被废弃
        obj2 = obj0;//obj2强引用对象B（现在obj0，ojb1，obj2都强引用对象B）
        //obj1 = nil;//obj1不再强引用对象B
        //obj0 = nil;//obj0不再强引用对象B
        //obj2 = nil;
        NSLog(@"obj0:%p obj1:%p obj2:%p", obj0, obj1, obj2);
    }
    return 0;
}
