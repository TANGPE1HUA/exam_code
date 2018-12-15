//
//  main.cpp
//  second_problem
//
//  Created by 糖仔 on 2018/12/11.
//  Copyright © 2018年 糖仔. All rights reserved.
//

#include <iostream>
#include <cstring>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;

int dx[4]={0,-1,0,1};
int dy[4]={1,0,-1,0};

const int MAXN = 1000;

class Solution{
private:
    Mat src_old,src;
    int src_size;
    int *arr;  // 图
    int *path; // 不重复路的路径
    bool flag[MAXN][MAXN];//标记是否走过
    int point_count; // 当前路径上的点的数量
    int target; // 应该要走的点的数量
    
    int minish_size; //把图缩小的比例 意思是把minish*minish的一块像素 看作一块，合成之后的y颜色根据像素中黑白颜色占比
    int pic[MAXN][MAXN]; //缩小之后的图
    int rows,cols;
public:
    Solution(string);//s初始化
    void setminish_size(int); // 设置缩小的比例
    void init_pic();  //设置缩小后的图
    int getColor(int,int);//获取这个区域看作一块的时候的颜色
    void find_the_road(); //枚举起点进行dfs搜索
    void dfs(int,int); //dfs遍历以起点为起点的借空间树
    bool check_position(int,int); // 检查是否合法可走位置
    void print_the_path(); //输出路径
    ~Solution();
};

int main(){
    string filename = "pic2.png";
    Solution solution(filename);
    solution.setminish_size(8);
    solution.init_pic();
    solution.find_the_road();
    return 0;
}


Solution::Solution(string filename){
    src_old = imread(filename,1);
    
    if (src_old.empty()){
        cout<<"Failed Reading"<<endl;
        exit(1);
    }
    src_size = src_old.rows*src_old.cols;
    
    arr = new int[src_size];
    path = new int[src_size];
    uchar *p; //遍历图
    target = 0;
    src = src_old.clone();
    
    threshold(src_old, src, 240, 255, THRESH_BINARY);
    
    p=src.data;
    for (int i=0;i<src_size;++i){
        arr[i]=(int)*(p+i*3);
    }
}

Solution::~Solution(){
    delete arr;
    delete path;
}

void Solution::setminish_size(int size){
    minish_size = size;
}

int Solution::getColor(int x, int y){
    int temp=0;
    for (int i=x*minish_size;i<(x+1)*minish_size;++i){
        for (int j=y*minish_size;j<(y+1)*minish_size;++j){
            temp+=arr[i*src.cols+j];
        }
    }
    if (temp>=255*minish_size*minish_size/2)
        return 255;
    else
        return 0;
}

void Solution::init_pic(){
    rows = src.rows / minish_size;
    cols = src.cols / minish_size;
    
    for (int i=0;i<rows;++i){
        for (int j=0;j<cols;++j){
            pic[i][j]=getColor(i,j);
            if (pic[i][j]==255)
                target++;
        }
    }
}

void Solution::find_the_road(){
    cout<<"the size of new pic is "<<rows<<" * "<<cols<<endl;
    int time=0;
    for (int i=0;i<rows;++i){
        for (int j=0;j<cols;++j){
            memset(flag,0,sizeof(flag));
            if (pic[i][j]==255){
                cout<<time<<" times to try"<<endl;
                dfs(i,j);
            }
        }
    }
}

bool Solution::check_position(int x, int y){
    if (x<0||x>=rows||y<0||y>=cols||pic[x][y]==0||flag[x][y])
        return false;
    return true;
}

void Solution::dfs(int x,int y){
    flag[x][y]=true;
    path[point_count++]=x*cols+y;
    if (point_count >500){
        print_the_path();
        return;
    }
    for (int i=0;i<4;i++){
        int xx = x + dx[i];
        int yy = y + dy[i];
        if (check_position(xx, yy)){
            dfs(xx,yy);
            flag[xx][yy]=false;
            --point_count;
        }
    }
    
}

void Solution::print_the_path(){
    cout<<"this is a road when the start position is ("<<path[0]/cols<<","<<path[0]%cols<<")"<<endl;
    cout<<"("<<path[0]/cols<<","<<path[0]%cols<<")";
    for (int i=1;i<target;i++){
        cout<<" -> "<<"("<<path[i]/cols<<","<<path[i]%cols<<")";
    }
    cout<<endl;
}

