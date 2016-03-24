//
//  Stack.h
//  lab2
//
//  Created by 季灵阳 on 4/9/15.
//  Copyright (c) 2015 季灵阳. All rights reserved.
//

#ifndef __lab2__Stack__
#define __lab2__Stack__

#include <stdio.h>
#include <vector>
using namespace std;
//栈
class Stack{
    
private:
    vector<int> data;   //栈元素指针
    int head;           //栈顶序号
    
    
public:
    void push(int dat);
    void initialize();
    void  pop();
    int  getHead();    //获取栈顶值
    void modify();     //修改栈顶值
};
#endif /* defined(__lab2__Stack__) */
