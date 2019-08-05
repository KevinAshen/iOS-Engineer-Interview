//
//  main.cpp
//  Offer-Exercise
//
//  Created by Kevin.J on 2019/8/3.
//  Copyright © 2019 姜凯文. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cstdio>
using namespace std;

class Solution {
public:
    void replaceSpace(char *str,int length) {
        int num = 0;
        int i = 0;
        while (str[i] != '\0') {
            if (str[i++] ==' ') {
                num++;
            }
        }
        int before = length - 1;
        int after = length - 1 + 2 * num;
        str[after + 1] = '\0';
        while (before != -1) {
            if (str[before] == ' ') {
                before--;
                str[after--] = '0';
                str[after--] = '2';
                str[after--] = '%';
            } else {
                str[after--] = str[before--];
            }
        }
        str[length + 2 * num] = '\0';
    }
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";

    Solution f;
    char a[] = " we are jkw";
    f.replaceSpace(a, 11);
    printf("%s", a);
//    cout << a << endl;
    return 0;
}
