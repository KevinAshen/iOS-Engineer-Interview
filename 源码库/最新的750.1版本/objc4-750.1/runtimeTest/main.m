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
#import "TestObject.h"

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
        
//        NSObject *obj0 = [[NSObject alloc] init];
//        //NSLog(@"XXX%p", obj0);
//        id __weak obj1 = obj0;
        
        Person *person = [[Person alloc] init];
        [person say];
//        NSLog(@"%p", obj0);
//        NSObject *obj1 __weak = obj0;
//        NSObject *obj3 __weak = obj1;
//        NSObject *obj2 = obj1;
//        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef d)(obj0)));
//        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj1)));
//        printf("retain count = %ld\n",CFGetRetainCount((__bridge CFTypeRef)(obj2)));
        
//        Person *p = [[Person alloc] init];
//        [p fly];
//
//        Class pcls = [Person class];
//        NSLog(@"p address = %p",pcls);
    }
    
    return 0;
}
