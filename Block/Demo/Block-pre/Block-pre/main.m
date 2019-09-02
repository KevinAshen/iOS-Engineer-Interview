//
//  main.m
//  OCLearning
//
//  Created by Kevin.J on 2019/6/18.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#import <Foundation/Foundation.h>

//int buttonID = 0;
//
//void buttonCallback(int event) {
//
//    NSLog(@"button:%d event = %d", buttonID, event);
//}
//
//void setButtonCallback(int offset, void (*p)(int)) {
//
//    p(offset);
//}
//
//void setButtonCallbacks() {
//
//    for (int i = 0; i < 10; i++) {
//        buttonID = i;
//        setButtonCallback(0 + i, &buttonCallback);
//    }
//}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        //setButtonCallbacks();
        int dmy = 256;
        int val = 10;
        const char  *fmt = "val = %d\n";
        void (^blk)(void) = ^{
            
            printf(fmt, val);
        };
        
        val = 2;
        fmt = "THESE VALUES WERE CHANGED. val = %d\n";
        
        blk();
        
        return 0;
    }
    return 0;
}
