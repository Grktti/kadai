//
// Created by rikuo on 25/08/17.
//
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <iostream>

// 平均: 読み取り専用の参照で無駄なコピーを避ける
double meanOf(const std::vector<int>& v){
    // TODO: 平均を計算する関数を作成
    if (v.empty()) throw std::invalid_argument("meanOf: empty vector");
    long long sum = std::accumulate(v.begin(), v.end(), 0LL);
    return static_cast<double>(sum) / static_cast<double>(v.size());
}

// 最大: イテレータから値を取り出す
int maxOf(const std::vector<int>& v){
    // TODO: 最大値を計算する関数を作成
    if (v.empty()) throw std::invalid_argument("maxOf: empty vector");
    return *std::max_element(v.begin(), v.end());
}

// 中央値: 呼び出し側のデータを壊さないよう値渡しでコピー
double medianOf(std::vector<int> v){
    // TODO: 中央値を計算する関数を作成
    if (v.empty()) throw std::invalid_argument("medianOf: empty vector");
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    if (n % 2 == 1) return static_cast<double>(v[n/2]);
    return (static_cast<double>(v[n/2 - 1]) + static_cast<double>(v[n/2])) / 2.0;
}

int main(){
    {
        std::vector<int> a{2,9,4,1,7};
        std::cout << "max=" << maxOf(a)
                  << ", mean=" << meanOf(a)
                  << ", median=" << medianOf(a) << "\n"; // => 9, 4.6, 4
    }
    {
        std::vector<int> b{1,2,3,4};
        std::cout << "median(b)=" << medianOf(b) << "\n"; // => 2.5
    }
}