//
//  main.m
//  weak编译
//
//  Created by Kevin.J on 2019/7/17.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "Student.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
//        Student *newStudent = [[Student alloc] init];
//        [newStudent dayin];
        Person *newPerson = [[Person alloc] init];
        [newPerson dayin];
    }
    return 0;
}
