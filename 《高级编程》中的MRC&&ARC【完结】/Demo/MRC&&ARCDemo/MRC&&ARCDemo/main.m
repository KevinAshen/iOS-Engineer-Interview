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
        NSObject *obj0 = [[NSObject alloc] init];
        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj0)));
        NSObject * __weak obj1 = obj0;
        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj1)));
        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj0)));
        
//        retain count = 1
//        retain count = 2
//        retain count = 1
    }
    return 0;
}
