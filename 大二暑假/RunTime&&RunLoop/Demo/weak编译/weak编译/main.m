//
//  main.m
//  weak编译
//
//  Created by Kevin.J on 2019/7/17.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSObject *obj = [[NSObject alloc] init];
        id __weak obj1 = obj;
    }
    return 0;
}
