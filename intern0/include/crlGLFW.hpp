/**
    @file crlGLFW.hpp
    @brief  GLFW base class for GLFW sample program
    http://www.sanko-shoko.net/note.php?id=3xnt is referred w/ thanks.
    @author Hiroshi Igarashi
    @author Tokyo Denki Univ.
    @license  MIT License
    @date 2023/09/28
    @version 1.0
 */

#ifndef __CRL_GLFW_HPP__
#define __CRL_GLFW_HPP__

#include <stdio.h>
#include "GLFW/glfw3.h"

class crlGLFW {
public:

    virtual void windowSize(int width, int height) {
        printf("windowSize %d %d\n", width, height);
    }
    virtual void mouseButton(int button, int action, int mods) {
        printf("mouseButton %d %d %d\n", button, action, mods);
    }
    virtual void mousePos(double x, double y) {
        printf("mousePos %.1lf %.1lf\n", x, y);
    }
    virtual void mouseScroll(double x, double y) {
        printf("mouseScroll %.1lf %.1lf\n", x, y);
    }
    virtual void keyFun(int key, int scancode, int action, int mods) {
        printf("keyFun %d %d %d %d\n", key, scancode, action, mods);
    }
    virtual void charFun(unsigned int charInfo) {
        printf("charFun %d\n", charInfo);
    }
    virtual void drop(int num, const char **paths) {
        printf("drop %d\n", num);
        for (int i = 0; i < num; i++) {
            printf("%s\n", paths[i]);
        }
    }

    virtual void display() {
        glColor3d(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2d(+0.0, +0.9);
        glVertex2d(-0.9, -0.9);
        glVertex2d(+0.9, -0.9);
        glEnd();
    }

public:

    virtual void execute(const char *name, const int width, const int height) {

        if (!glfwInit()) return;

        GLFWwindow *window = glfwCreateWindow(width, height, name, NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        // 作成したウィンドウにコールバック関数を設定する
        setCallback(window);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            display();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }

protected:

    virtual void _windowSize(int width, int height) {
        windowSize(width, height);
    }
    void _mouseButton(int button, int action, int mods) {
        mouseButton(button, action, mods);
    }
    void _mousePos(double x, double y) {
        mousePos(x, y);
    }
    void _mouseScroll(double x, double y) {
        mouseScroll(x, y);
    }
    void _keyFun(int key, int scancode, int action, int mods) {
        keyFun(key, scancode, action, mods);
    }
    void _charFun(unsigned int charInfo) {
        charFun(charInfo);
    }
    void _dropCB(int num, const char **paths) {
        drop(num, paths);
    }

protected:

    void setCallback(GLFWwindow *window) {
        // ウィンドウとクラスのポインタを関連付ける
        glfwSetWindowUserPointer(window, this);

        glfwSetWindowSizeCallback(window, windowSizeCB);

        glfwSetMouseButtonCallback(window, mouseButtonCB);
        glfwSetCursorPosCallback(window, mousePosCB);
        glfwSetScrollCallback(window, mouseScrollCB);

        glfwSetKeyCallback(window, keyFunCB);
        glfwSetCharCallback(window, charFunCB);

        glfwSetDropCallback(window, dropCB);
    }

    static crlGLFW* getThisPtr(GLFWwindow *window) {
        return static_cast<crlGLFW*>(glfwGetWindowUserPointer(window));
    }
    static void windowSizeCB(GLFWwindow *window, int width, int height) {
        getThisPtr(window)->_windowSize(width, height);
    }
    static void mouseButtonCB(GLFWwindow *window, int button, int action, int mods) {
        getThisPtr(window)->_mouseButton(button, action, mods);
    }
    static void mousePosCB(GLFWwindow *window, double x, double y) {
        getThisPtr(window)->_mousePos(x, y);
    }
    static void mouseScrollCB(GLFWwindow *window, double x, double y) {
        getThisPtr(window)->_mouseScroll(x, y);
    }
    static void keyFunCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
        getThisPtr(window)->_keyFun(key, scancode, action, mods);
    }
    static void charFunCB(GLFWwindow* window, unsigned int charInfo) {
        getThisPtr(window)->_charFun(charInfo);
    }
    static void dropCB(GLFWwindow *window, int num, const char **paths) {
        getThisPtr(window)->_dropCB(num, paths);
    }
};

#endif  // __CRL_GLFW_HPP__
