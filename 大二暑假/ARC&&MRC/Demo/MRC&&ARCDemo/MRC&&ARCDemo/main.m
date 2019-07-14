//
//  main.m
//  MRC&&ARCDemo
//
//  Created by Kevin.J on 2019/7/6.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Test.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        
        id test0 = [[Test alloc] init];
        id test1 = [[Test alloc] init];
        [test0 setObject:test1];
        [test1 setObject:test0];
        
        NSLog(@"124");
        
    }
    return 0;
}
