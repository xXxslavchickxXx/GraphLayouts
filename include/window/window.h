#pragma once

#include <memory>
#include <functional>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
    std::string _title;
    GLFWwindow* windowPtr;

public:
    Window();
    Window(int width, int height);
    Window(int width, int height, const std::string& title);

    void setClearColor(int R, int G, int B, int A = 1.f);

    template<typename F>
    void run(F&& lambda);

};

#include "window.inl"