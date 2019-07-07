//
//  main.m
//  MRC-SHOW-TIME
//
//  Created by Kevin.J on 2019/7/6.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Print.h"



int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        
        NSObject *obj = [[NSObject alloc] init];
        //printf("retain count = %ld\n", obj.retainCount);
        [Print printRetainCount:obj];
        //在MRC时代，创建对象时，会把引用计数自动设成1，也就是说，创建时两者其实是一致的，都是引用计数从0变成1
//        [obj retain];
//
//        [Print printRetainCount:obj];
        
        [obj release];
        
        [Print printRetainCount:obj];
        
        [obj release];
        
        //[Print printRetainCount:obj];
        
        //[Print printRetainCount:obj];
        
        
        
//        //[obj dealloc];
//
//        [Print printRetainCount:obj];
//
//        [obj release];
//
//        [Print printRetainCount:obj];
//
//        [obj autorelease];
        
        
    }
    return 0;
}
