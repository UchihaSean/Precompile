//
//  Stack.cpp
//  lab2
//
//  Created by 季灵阳 on 4/9/15.
//  Copyright (c) 2015 季灵阳. All rights reserved.
//

#include "Stack.h"

//栈初始化
void Stack::initialize(){
    head=0;
    data.push_back(-2);    //处理边界
}

//弹出头元素
void Stack::pop(){
    if (head>0) {
    head--;
    data.pop_back();
    }
}

//压入新元素
void Stack::push(int dat){
    head++;
    data.push_back(dat);
}

//获取栈顶值
int Stack::getHead(){
    return data[head];
}

//修改栈顶值
void Stack::modify(){
    data[head]=1-data[head];
}
