//
// Created by rikuo on 25/08/18.
//
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdlib>

constexpr float PI = 3.1415926535f;

//演算子オーバーロード用
struct Vec2 {
    float x{0}, y{0};
    Vec2() = default;
    Vec2(float X, float Y): x(X), y(Y) {}
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2 operator*(float s)       const { return {x*s, y*s}; }
    Vec2& operator+=(const Vec2& o){ x+=o.x; y+=o.y; return *this; }
    Vec2& operator-=(const Vec2& o){ x-=o.x; y-=o.y; return *this; }
};
static inline float dot(const Vec2&a,const Vec2&b){ return a.x*b.x + a.y*b.y; }
static inline float norm(const Vec2&a){ return std::sqrt(dot(a,a)); }
static inline Vec2  normalize(const Vec2&a){ float n=norm(a); return (n>1e-6f)? a*(1.0f/n):Vec2{}; }

class Agent {
public:
    // コンストラクタ：位置・速度・半径・視野半径を注入
    Agent(const Vec2& p0, const Vec2& v0, float radius, float viewRadius)
        : p_(p0), v_(v0), radius_(radius), viewRad_(viewRadius) {}

    // 物理更新：Boids（分離・整列・凝集）+ 壁 + ダンパ
    void drive(float dt, const std::vector<Agent>& all, float W, float H) {
        Vec2 F{0,0};
        Vec2 v_avg{0,0}, p_avg{0,0};
        int cnt = 0;

        for (const auto& o : all) {
            if (&o == this) continue;
            Vec2 rij = o.p_ - p_;
            float d = norm(rij);
            if (d < viewRad_) {
                if (d > 1e-4f) {
                    float overlap = viewRad_ - d;
                    // 分離は“相手と逆向き”へ（反発）
                    F -= normalize(rij) * (k_sep_ * overlap / (d + 1e-3f));
                }
                v_avg += o.v_;
                p_avg += o.p_;
                ++cnt;
            }
        }
        if (cnt > 0) {
            v_avg = v_avg * (1.0f/cnt);
            p_avg = p_avg * (1.0f/cnt);
            F += (v_avg - v_) * k_ali_;  // 整列
            F += (p_avg - p_) * k_coh_;  // 凝集
        }

        // 壁（ソフトバネ）
        const float margin = 30.f;
        if (p_.x < margin)      F.x += k_wall_ * (margin - p_.x);
        if (p_.x > W - margin)  F.x -= k_wall_ * (p_.x - (W - margin));
        if (p_.y < margin)      F.y += k_wall_ * (margin - p_.y);
        if (p_.y > H - margin)  F.y -= k_wall_ * (p_.y - (H - margin));

        // ダンパ
        F += v_ * (-c_);

        // クリップ & 半陰的オイラー
        clipVec(F, Fmax_);
        v_ = v_ + F * (dt / m_);
        clipVec(v_, Vmax_);
        p_ = p_ + v_ * dt;

        // 画面内にクランプ（半径分内側）
        if (p_.x < radius_)   p_.x = radius_;
        if (p_.x > W-radius_) p_.x = W - radius_;
        if (p_.y < radius_)   p_.y = radius_;
        if (p_.y > H-radius_) p_.y = H - radius_;
    }

    // 描画（副作用なし）
    void draw() const {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(p_.x, p_.y);
        const int seg = 18;
        for (int i=0; i<=seg; ++i) {
            float a = 3.f*PI*i/seg;
            glVertex2f(p_.x + radius_*std::cos(a), p_.y + radius_*std::sin(a));
        }
        glEnd();
    }

    // 必要最小限のゲッター
    Vec2  pos()      const { return p_; }
    Vec2  vel()      const { return v_; }
    float radius()   const { return radius_; }
    float viewRad()  const { return viewRad_; }

private:
    // 状態
    Vec2  p_{0,0};
    Vec2  v_{0,0};
    float radius_{3.f};
    float viewRad_{200.f};

    // 物理パラメータ
    float m_ = 1.0f;     // 質量
    float c_ = 1.2f;     // 減衰（ダンパ）

    // Boids ゲイン
    float k_sep_  = 5.f;
    float k_ali_  =  30.f;
    float k_coh_  =   8.f;
    float k_wall_ =  5.f;

    // 制限
    float Fmax_ = 800.f;
    float Vmax_ = 150.f;

    static void clipVec(Vec2& v, float vmax){
        float n = norm(v);
        if (n > vmax) v = v * (vmax / (n + 1e-6f));
    }
};


class Flock {
public:
    Flock(int n, int W, int H, float agentRadius, float viewRadius)
        : W_(W), H_(H)
    {
        agents_.reserve(n);
        for (int i=0; i<n; ++i) {
            float x = W * 0.5f;
            float y = H * 0.5f;
            float ang = (float)std::rand()/RAND_MAX * 2.f*PI;
            float spd = 40.f + (std::rand()%60);
            Vec2 p{x,y};
            Vec2 v{std::cos(ang)*spd, std::sin(ang)*spd};
            agents_.emplace_back(p, v, agentRadius, viewRadius);
        }
    }

    void step(float dt){
        const std::vector<Agent> snapshot = agents_;            // 同時刻参照用
        for (auto& a : agents_) a.drive(dt, snapshot, (float)W_, (float)H_);
    }

    void render() const {
        glColor3f(0.9f, 0.1f, 0.1f);
        for (const auto& a : agents_) a.draw();
    }
private:
    int W_, H_;
    std::vector<Agent> agents_;
};


int main(){
    const int W = 500, H = 500;
    if(!glfwInit()) return -1;
    GLFWwindow* win = glfwCreateWindow(W, H, "Boids (Agent class)", nullptr, nullptr);
    if(!win){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0, W, H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // 条件：台数10・視野半径50（Flock 内部）・半径3
    Flock flock(10, W, H, 3.f, 200.f);

    const double dt = 0.01; // 100Hz
    auto next = std::chrono::steady_clock::now();

    while(!glfwWindowShouldClose(win)){
        flock.step((float)dt);

        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        flock.render();

        glfwSwapBuffers(win);
        glfwPollEvents();

        next += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double>(dt));
        std::this_thread::sleep_until(next);
    }
    glfwTerminate();
    return 0;
}
