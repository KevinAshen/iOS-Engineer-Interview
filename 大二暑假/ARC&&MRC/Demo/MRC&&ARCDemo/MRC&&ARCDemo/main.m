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
        
        id obj = [[NSObject alloc] init];
        NSLog(@"objretainCount: = %ld", obj.retainCount);
    }
    return 0;
}
