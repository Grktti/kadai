//
// Created by rikuo on 25/08/18.
//

#ifndef AGENTCORE_H
#define AGENTCORE_H

#include <iostream>
#include <vector>
#include <random>
#include <cmath>

template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    //    os << "[";
    for (int i = 0; i < (int)v.size(); i++) {
        //os << std::defaultfloat << v[i];
        os << std::fixed << std::showpos << v[i];
        if (i != (int)v.size() - 1) os << ", ";
    }
    //    os << " ]";
    os << std::defaultfloat;
    return os;
}

// 2つのベクトルの加算（要素ごと）
template<class T>
std::vector<T> operator+(const std::vector<T> &v1, const std::vector<T> &v2) {
    if (v1.size() != v2.size()) std::cerr << "#error: size() is not equal. @vector operator-" << std::endl;
    std::vector<T> ans = v1;
    for (size_t i = 0, size = ans.size(); i < size; ++i)
        ans[i] += v2[i];
    return ans;
}

// 2つのベクトルの減算（要素ごと）
template<class T>
std::vector<T> operator-(const std::vector<T> &v1, const std::vector<T> &v2) {
    if (v1.size() != v2.size()) std::cerr << "#error: size() is not equal. @vector operator-" << std::endl;
    std::vector<T> ans(v1.size());
    for (size_t i = 0, size = ans.size(); i < size; ++i)
        ans[i] = v1[i] - v2[i];
    return ans;
}

// ベクトルの各要素をスカラーで割る
template<class T>
std::vector<T> operator/(const std::vector<T> &v1, const double v2) {
    std::vector<T> ans(v1.size());
    for (size_t i = 0, size = ans.size(); i < size; ++i)
        ans[i] = v1[i] / v2;
    return ans;
}

// ベクトルの各要素にスカラーを掛ける
template<class T>
std::vector<T> operator*(const std::vector<T> &v1, const double v2) {
    std::vector<T> ans(v1.size());
    for (size_t i = 0, size = ans.size(); i < size; ++i)
        ans[i] = v1[i] * v2;
    return ans;
}

// スカラー × ベクトル（順序を逆にしたバージョン）
template<class T>
std::vector<T> operator*(const double v1, const std::vector<T> &v2) {
    return v2 * v1;
}

#endif //AGENTCORE_H
