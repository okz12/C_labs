/* 
 * File:   main.cpp
 * Author: Osman
 *
 * Created on 21 January 2014, 10:26
 */

#include<iostream>

using namespace std;

struct point{
    double x;
    double y;
};

int main(){
    int i, asize;
    point* pp;
    
    cout <<"enter dynamic arrays size"<<endl;
    cin>>asize;
    
    pp = new point[asize];
    
    for(i=0; i<asize; i++){
        cout<<"enter element"<<i<<": "<<endl;
        cin>>pp[i].x>>pp[i].y;
    }
    
    for(int i=0; i<asize; i++){
        cout<<"element"<<i<<": ("<<pp[i].x<<", "<<pp[i].y<<")"<<endl;
    }
    
    delete[] pp;
    return 0;
}

