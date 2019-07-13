//
//  main.m
//  runtimeTest
//
//  Created by lonlife on 2019/3/15.
//

#import <Foundation/Foundation.h>
#import "objc-runtime.h"
#import "Person.h"
#import "Person+Fly.h"

// 把一个十进制的数转为二进制
NSString * binaryWithInteger(NSUInteger decInt){
    NSString *string = @"";
    NSUInteger x = decInt;
    while(x > 0){
        string = [[NSString stringWithFormat:@"%lu",x&1] stringByAppendingString:string];
        x = x >> 1;
    }
    return string;
}

int main(int argc, const char * argv[]) {
    // 整个程序都包含在一个@autoreleasepool中
    @autoreleasepool {
        // insert code here...
        Person *p = [[Person alloc] init];
        [p fly];

        Class pcls = [Person class];
        NSLog(@"p address = %p",pcls);
    }
    return 0;
}
