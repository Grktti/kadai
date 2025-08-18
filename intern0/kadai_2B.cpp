#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <thread>   // std::this_thread::sleep_for
#include <chrono>   // std::chrono::milliseconds
#define M_PI 3.14159265

class Ball {
public:
    float x, y;   // 位置
    float vx, vy; // 速度
    float r = 10.0f;

    Ball(float _x, float _y, float _vx, float _vy)
        : x(_x), y(_y), vx(_vx), vy(_vy) {}

    void update(float dt, float width, float height) {
        x += vx * dt * 100; // ← 速度にdtを掛ける（100はスケール調整）
        y += vy * dt * 100;

        // 壁で反射
        if (x < 0 || x > width) vx = -vx;
        if (y < 0 || y > height) vy = -vy;
    }

    void draw() {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // 中心
        int segments = 20;
        for (int i = 0; i <= segments; i++) {
            float theta = 2 * M_PI * i / segments;
            glVertex2f(x + r * cos(theta), y + r * sin(theta));
        }
        glEnd();
    }
};

int main() {
    int width = 1000, height = 1000;

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(width, height, "Ball Demo", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // 座標系をピクセルベースに設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    Ball ball(500, 500, 1, 1); // 中心から動き出すボール

    // サンプリングタイム（秒）
    const double dt = 0.01; // 10msごとに更新
    auto prev = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        // 経過時間を計算
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - prev;
        prev = now;

        // モデル更新
        ball.update(elapsed.count(), width, height);

        // 描画
        glClearColor(1, 1, 1, 1); // 白背景
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1, 0, 0); // 赤
        ball.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        // サンプリングタイムまでスリープ
        std::this_thread::sleep_for(std::chrono::duration<double>(dt));
    }

    glfwTerminate();
    return 0;
}
