//
//  first_problem_oop.cpp
//  test
//
//  Created by 糖仔 on 2018/12/10.
//  Copyright © 2018年 糖仔. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <queue>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


int dx[4]={0,-1,0,1};
int dy[4]={1,0,-1,0};

const int MAXN = 1000;
class Solution{
private:
    Mat src;
    long long src_size;  //三通道合一之后的像素数量
    int room_count;     // 房间的数量
    bool v[MAXN][MAXN]; // 房间之间的关系的r二维数组
    int dis[MAXN][MAXN];// 任意两点之间的距离
    long long *arr;     // 三通道合一后的图像数组
    int *num;           // 像素编号数组
    bool *flag;         // 标记像素是否去过
    int min_sum_distace_start;//出发到其他房间的总距离最短的房间号
    
public:
    Solution(string filename);    //读图，初始化
    void make_tag_to_room();      //给房间编号
    void find_the_edge_room();    //获取房间相邻的信息
    void print_the_relation_of_room(); // 输出房间之间的关系
    void dfs(int position);       //找联通块
    bool check_board(int x,int y);//检查边界
    void print_topo_queue_of_room();//输出房间之间的拓扑顺序
    void find_min_sum_distance_start(); //找到出发到其他房间的总距离最短的房间号
    void cal_room_to_room_shortest_path(); //计算任意房间之间的最短距离
    void print_the_min_sum_distace_start();//输出出发到其他房间的总距离最短的房间号
    void print_the_distance_between_rooms();//输出房间之间的最短距离
    int get_the_distance_between_roomx_and_rooy(int x,int y); // 获取任意两个房间之间的距离
    ~Solution();
};


int main(){ // 主函数
    
    string filename="pic1.png";
    Solution solution(filename);
    
    solution.make_tag_to_room();  //给房间编号
    solution.find_the_edge_room();//获取房间相邻的信息
    
    solution.print_the_relation_of_room();  // 输出房间之间的关系
    solution.print_topo_queue_of_room();     //输出房间之间的拓扑顺序
    
    solution.cal_room_to_room_shortest_path();  //计算任意房间之间的最短距离
    solution.find_min_sum_distance_start(); //找到出发到其他房间的总距离最短的房间号
    
    solution.print_the_min_sum_distace_start(); //输出出发到其他房间的总距离最短的房间号
    solution.print_the_distance_between_rooms(); //输出房间之间的最短距离
    
    return 0;
}



Solution::Solution(string filename){ //初始化
    src = imread(filename);
    uchar *p = src.data;//遍历像素数组
    src_size = src.rows*src.cols;
    arr = new long long[src_size];
    
    if (src.empty()){
        cout<<"Failed Reading"<<endl;
        exit(1);
    }
    
    memset(v,0,sizeof(v));
    memset(dis,-1,sizeof(dis));
    
    for (int i=0;i<src_size;++i)   // 三通道合一
        arr[i]=(((long long)*(p+i*3))<<16) + (((long long)*(p+i*3+1))<<8) + ((long long)*(p+i*3+2));
}

Solution::~Solution(){
    delete arr;
    delete num;
    delete flag;
}
void Solution::make_tag_to_room(){ //给房间编号
    room_count=0;
    flag = new bool[src_size];
    num = new int[src_size];
    for (int i=0;i<src_size;++i){
        if (!flag[i]&&arr[i]!=0){
            ++room_count;
            dfs(i);
        }
    }
}

bool Solution::check_board(int x,int y){// 判断边界
    if (x<0||x>=src.rows||y<0||y>=src.cols)
        return false;
    return true;
}

void Solution::dfs(int position){//找到所有和当前像素的联通块
    flag[position]=true;     //标记
    num[position]=room_count;//编号

    int x = position / src.cols;  // 行
    int y = position % src.cols;  // 列

    for (int i=0;i<4;++i){
        int xx = x + dx[i];
        int yy = y + dy[i];
        if (check_board(xx,yy)){
            int after = xx*src.cols+yy; //转换成数组的位置
            if (!flag[after]&&arr[after]==arr[position]){
                dfs(after);
            }
        }
    }
}

void Solution::find_the_edge_room(){    //获取房间相邻的信息
    for (int i=0;i<src_size;i++){
        if (arr[i]==0){  //如果是黑色的像素就看看是否在两个房间之间
            int x = i / src.cols; //row
            int y = i % src.cols; // col

            if (check_board(x-1, y)&&check_board(x+1, y)){ //上 下
                long long up = num[(x-1)*src.cols+y];
                long long down = num[(x+1)*src.cols+y];
                if (up>0&&down>0&&up!=down)
                    v[up][down]=v[down][up]=true;
            }

            if (check_board(x, y-1)&&check_board(x, y+1)){ //左 右
                long long left = num[x*src.cols+y-1];
                long long right = num[x*src.cols+y+1];
                if (left>0&&right>0&&left!=right)
                    v[left][right]=v[right][left]=true;
            }
        }
    }
}

void Solution::print_the_relation_of_room(){ //输出房间之间的相邻关系
    for (int i=1;i<=room_count;i++){
        for (int j=1;j<=i;j++){
            if (v[i][j]){
                cout<<"room "<<i<<" is  beside the room "<<j<<endl;
            }
        }
        cout<<endl;
    }
}

void Solution::print_topo_queue_of_room(){  //输出所有房间的一个拓扑顺序
    int *to = new int[MAXN];    // 标记每个房间的入度
    for (int i=1;i<=room_count;++i){ //  取 i=1~room_count j=i~room_count 这个半区的关系作为这个图的有向路
        for (int j=i+1;j<=room_count;++j){
            to[j]+=v[i][j];
        }
    }
    queue<int> q;
    for (int i=1;i<=room_count;++i)
        if (to[i]==0)
            q.push(i);
    cout<<"this is the topu queue of the rooms:"<<endl;
    bool p=false;
    while(!q.empty()){
        int now = q.front();
        q.pop();
        if (p)
            cout<<" -> ";
        else
            p=!p;
        cout<<now;
        for (int i=1;i<=now;++i){
            if (v[now][i]){
                to[i]--;
                if (to[i]==0)
                    q.push(i);
            }
        }
    }
    cout<<endl;
}



void Solution::cal_room_to_room_shortest_path(){
    for (int i=1;i<=room_count;++i){
        for (int j=1;j<=i;++j){
            if (v[i][j])
                dis[i][j]=1;
        }
    }
    for (int k=1;k<=room_count;++k){
        for (int i=1;i<=room_count;++i){
            for (int j=1;j<=room_count;++j){
                if (dis[i][k]!=-1&&dis[k][j]!=-1){
                    if (dis[i][j]==-1||dis[i][j]>dis[i][k]+dis[k][j])
                        dis[i][j]=dis[j][i]=dis[i][k]+dis[k][j];
                }
            }
        }
    }
}

void Solution::find_min_sum_distance_start(){
    int min_sum_distance=-1;
    int temp_sum_distance;
    
    for (int i=1;i<=room_count;++i){
        temp_sum_distance=0;
        for (int j=1;j<=room_count;++j){
            if (i!=j){
                if (dis[i][j]>0)
                    temp_sum_distance+=dis[i][j];
            }
        }
        if (min_sum_distance==-1||temp_sum_distance<min_sum_distance){
            min_sum_distace_start=i;
            min_sum_distance=temp_sum_distance;
        }
    }
}

void Solution::print_the_min_sum_distace_start(){
    cout<<"the sum distance from room "<<min_sum_distace_start<<" to other rooms is the shortest"<<endl;
}

void Solution::print_the_distance_between_rooms(){
    for (int i=1;i<=room_count;++i){
        for (int j=i+1;j<=room_count;++j){
            if(dis[i][j]!=-1){
                cout<<"the shortest distance of room "<<i<<" and room "<<j<<" is "<<dis[i][j]<<endl;
            }
        }
    }
}

int Solution::get_the_distance_between_roomx_and_rooy(int x, int y){
    return dis[x][y];
}
