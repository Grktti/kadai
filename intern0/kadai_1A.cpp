#include <iostream>
#include <string>

std::string renderGrid(int W, int H) {
    // TODO: 文字列バッファを作って for で埋める
    if (W <= 1 || H <= 1) return ""; // 最低限のサイズチェック
    std::string buf;
    buf.reserve(static_cast<size_t>((W + 1) * H));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            bool border = (x == 0 || y == 0 || x == W - 1 || y == H - 1);
            buf.push_back(border ? '#' : '?');
        }
        buf.push_back('\n');
    }
    return buf;
}

int main(){
    std::cout << renderGrid(10,6);
}