//
//  main.cpp
//  lab2
//
//  Created by 季灵阳 on 4/9/15.
//  Copyright (c) 2015 季灵阳. All rights reserved.
//

#include "Stack.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;
vector<string> lines;   //存储读入文件每一行string
vector<int> marks;      //标记读入文件每一行是否需要输出（0/1） 1 表示输出 默认为1

/*
 * define 表
 * 分为左右两个string
 * 记录从左到右的转换
 * 若右表为 "true_" 或者 “false_” 则为纯粹记录是否定义
 */
vector<string> defineLeft;
vector<string> defineRight;

//define 处理
void manipulateDefine(string s){
    long blank=s.find(" ");   //找空格
    
    /*
     * 没有空格 说明是纯粹定义 直接定义为 “true_” 存入表中
     * 有空格   直接根据空格把左右两个string 存入表中
     */
    if (blank==-1){
        defineLeft.push_back(s.substr(0,s.size()));
        defineRight.push_back("true_");
    } else {
        defineLeft.push_back(s.substr(0,blank));
        defineRight.push_back(s.substr(blank+1,s.size()-blank-1));
    }
}

//undefine 处理
void manipulateUndifine(string s){
    
    //在表中查找 找到则改为 "false_"
    for (int i=0; i<defineLeft.size(); i++) {
        if (defineLeft[i].compare(s)==0){
            defineRight[i]="false_";
        }
    }
}

//ifdefine 处理
bool manipulateIfdefine(string s){
    for (int i=0;i<defineLeft.size();i++){
        //表中有定义 且右表非 “false_”  返回 true
        if (defineLeft[i].compare(s)==0 && defineRight[i].compare("false_")!=0) {
            return true;
        }
    }
    return false;
}

//if 处理
bool manipulateIf(string s){
    string t=s,u;
    
    //反复查表 直到不能改变
    while (true){
        u=t;
        for (int i=0;i<defineLeft.size();i++){
            if (defineLeft[i].compare(t)==0) {
                t=defineRight[i];
            }
        }
        if (u.compare(t)==0) break;
    }
    if (t.compare("0")==0)
        return false; else
            return true;
}

//ifndefine 处理
bool manipulateIfndefine(string s){
    for (int i=0;i<defineLeft.size();i++){
        //表中有定义 且右表非“false_” 返回 false
        if (defineLeft[i].compare(s)==0 && defineRight[i].compare("false_")!=0) {
            return false;
        }
    }
    return true;
}

bool manipulateInclude(string s){
    /*
     * 查找.h 并判断文件是否存在
     * 将.h文件中内容放入预处理行lines中
     */
    if (s.find(".h")!=-1){
        string fileInInclude=s.substr(1,s.size()-2);
        ifstream inInclude(fileInInclude);
        if (inInclude.is_open()){
            string lineInclude;
            while (getline(inInclude, lineInclude)) {
                lines.push_back(lineInclude);
                marks.push_back(1);
            }
            inInclude.close();
            return true;
        }
        
    }
    return false;

}

//对于define 左右替换的处理
string check(string s){
    for (int i=0;i<defineLeft.size();i++){
        
        //查找左右括号
        long x=defineLeft[i].find("("),y=defineLeft[i].find(")");
        long x2=defineRight[i].find("(");
        long userx=s.find("("),usery=s.find(")");
        string now="",used="";
        
        /*
         * 针对part 3（支持带一个参数的宏） 处理
         * 1.先将处理串中参数转为宏定义内参数
         * 2.然后进行正常转换
         * 3.最后将参数改回原来的
         */
        if (x!=-1 || x2!=-1){
            if (x!=-1 && userx!=-1){
                used=s.substr(userx+1,usery-userx-1);       //原来参数
                now=defineLeft[i].substr(x+1,y-x-1);        //改后参数
                s.replace(userx+1, usery-userx-1, now);
            }
            y=s.find(defineLeft[i]);
            if (y!=-1){
                s.replace(y, defineLeft[i].size(), defineRight[i]);
            }
            if (x!=-1 &&userx!=-1){
                y=s.find(now);
                s.replace(y, now.size(), used);
            }
            /*
             * 针对pluse (带#运算符) 处理
             * 1.去掉#
             * 2.查找（）内字符 两边加上“”
             */
            y=s.find("#");
            if (y!=-1 && userx!=-1){
                s.replace(y, 1, "");
                y=s.find(used);
                s.replace(y, used.size(), "\""+used+"\"");
            }
            
        }
    }
    return s;
}

//主函数
int main(int argc, const char * argv[]) {
    // insert code here...
    //    cout<<"Please insert this"<<endl;

    //解析读入文件和输出文件
    string filein="",fileout="";
    //读取传入参数
    filein=argv[1];
    fileout=argv[2];
    
    //读入和输出文件
    ifstream in(filein);
    ofstream out(fileout);
    
    string line,read;
    long blank;
    int head=0;                 //当前读到的文件行数

    /*
     * 用于处理 if else 的栈
     * 栈元素有3种值
     * 0  表示当前文本行不需要输出
     * 1  表示当前文本行需要输出
     * -1 表示当前文本行不需要输出
     * 0 1 在遇到 else 会进行转换 -1 不会
     * 遇到 if 根据真假 push 值（0/1） 若当前环境为0 则push -1
     * 遇到 else 转换栈顶值 （非-1）
     * 遇到 end 弹出栈顶值
     */
    Stack conditionIf;
    conditionIf.initialize();
    
    string nextLine;
    while (head<lines.size()+1){
        if (head>=lines.size()){
            getline(in, read);
            if (read=="") break;
            lines.push_back(read);  //读入每一行存入lines
            marks.push_back(1);    //默认为此行输出
        }
        blank=lines[head].find(" ");   //查找第一个空格
        //空格不存在
        if (blank==-1) {
            
            //若为 else
            if (lines[head].substr(0,lines[head].size()-1).compare("#else")==0){
                marks[head]=0;
                //修改栈顶值
                if (conditionIf.getHead()!=-1)
                    conditionIf.modify();
            }
            //若为 endif
            if (lines[head].substr(0,lines[head].size()-1).compare("#endif")==0){
                marks[head]=0;
                conditionIf.pop();  //弹出栈顶值
            }
            
        }
        
        //第一个空格存在
        if (blank!=-1) {
            
            //截取空格前后
            line=lines[head].substr(0,blank);
            nextLine=lines[head].substr(blank+1,lines[head].size()-blank-2);
            
            //空格前为 #ifdef
            if (line.compare("#ifdef")==0) {
                marks[head]=0;
                
                //当前环境为 0 直接push -1
                if (conditionIf.getHead()==0){
                    conditionIf.push(-1);
                } else{
                    //若为真则 push 1 否则 push 0
                    if (manipulateIfdefine(nextLine))
                        conditionIf.push(1); else
                            conditionIf.push(0);
                }
            }
            
            //空格前为 #ifndef
            if (line.compare("#ifndef")==0) {
                marks[head]=0;
                if (conditionIf.getHead()==0){
                    conditionIf.push(-1);
                } else{
                    if (manipulateIfndefine(nextLine)){
                        conditionIf.push(1);
                    } else
                        conditionIf.push(0);
                }
            }
            
            //空格前为 #if
            if (line.compare("#if")==0) {
                marks[head]=0;
                if (conditionIf.getHead()==0){
                    conditionIf.push(-1);
                } else{
                    if (manipulateIf(nextLine))
                        conditionIf.push(1); else
                            conditionIf.push(0);
                }
            }
            
            //若当前环境为 -1/0 则直接默认此行不用输出 处理下一行
            if ((conditionIf.getHead()==-1)||(conditionIf.getHead()==0)){
                marks[head]=0;
                head++;
                continue;
            }
            
            //空格前为 #define
            if (line.compare("#define")==0) {
                marks[head]=0;
                manipulateDefine(nextLine);
            }
            
            //空格前为 #undef
            if (line.compare("#undef")==0){
                marks[head]=0;
                manipulateUndifine(nextLine);
            }
            
            if (line.compare("#include")==0){
                marks[head]=1;
//                cout<<nextLine<<endl;
                if (manipulateInclude(nextLine))
                    marks[head]=0;
            }
            
        }
        head++;
    }
    
    //最后从头到尾 check 一遍 处理define 替换操作
    for (int i=0; i<lines.size(); i++) {
        if (marks[i]==1){
            out<<check(lines[i])<<endl;
        }
    }
    in.close();
    out.close();
    
}
