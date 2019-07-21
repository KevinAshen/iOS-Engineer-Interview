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
    
//        NSArray *arr = [NSArray array];
//        NSLog(@"%ld", arr.retainCount);
        
        
        NSObject *obj0 = [[NSObject alloc] init];
        //[obj0 autorelease];
        NSObject *obj1 = obj0;
        
        //[obj1 autorelease];
        NSLog(@"%lu", (unsigned long)obj0.retainCount);

        //NSLog(@"%p", obj);
        
        //[obj release];
        
        //[obj dealloc];
        
        //NSLog(@"%p", obj);
        
        //[obj release];
        
        //NSLog(@"%p", obj);
        //在MRC时代，创建对象时，会把引用计数自动设成1，也就是说，创建时两者其实是一致的，都是引用计数从0变成1
//        [obj retain];
//
//        [Print printRetainCount:obj];
        
       // [obj release];
        
        //id array = [NSMutableArray arrayWithCapacity:1];
        
//        [obj retain];
//        [obj release];
//        [obj retain];
//        [obj retain];
//        [Print printRetainCount:obj];
//        [Print printRetainCount:obj];
        
        
        
    
        
        //[obj dealloc];
        
        //[Print printRetainCount:obj];
        
        
        //NSLog(@"%ld", obj.retainCount);
        
       // NSLog(@"%p", obj);
        
        //[Print printRetainCount:obj];
        
//        [obj release];
//
//        [Print printRetainCount:obj];
        
        //[obj retain];
        
        //[Print printRetainCount:obj];//这里和书上描述的一致，我们进行alloc，计数是1，进行release操作，发现打印还是1，
        
        //[obj release];
        
        //[Print printRetainCount:obj];
        
        //[obj retain];
        
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
