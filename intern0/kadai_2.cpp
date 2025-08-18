#include <iostream>


class Ball {
public:
    float x, y;   // 座標
    float vx, vy; // 速度

    Ball(float _x, float _y, float _vx, float _vy) {
        x = _x; y = _y;
        vx = _vx; vy = _vy;
    }

    void update() {
        x += vx;
        y += vy;
    }

    void print() {
        std::cout << "(" << x << "," << y << ")" << std::endl;
    }
};

int main() {
    Ball b(0, 0, 1, 1); // (0,0)から速度(1,1)で動く
    for (int i=0; i<5; i++) {
        b.update();
        b.print();
    }
}
