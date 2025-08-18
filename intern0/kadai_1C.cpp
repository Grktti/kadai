//
// Created by rikuo on 25/08/18.
//
#include <fstream>
#include <iostream>

int main(){
    std::ofstream ofs("log.csv");
    if(!ofs){ std::cerr << "open failed\n"; return 1; }
    ofs << "step,value\n";
    for(int step=0; step<=10; ++step){
        int value = step*step; // 例: 二乗
        ofs << step << "," << value << "\n";
    }
    std::cout << "wrote log.csv\n";
}