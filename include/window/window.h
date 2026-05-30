#pragma once

#include <memory>
#include <functional>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

#include <consoleTools.h>
/// <summary>
/// Просто возможность вывода матрицы
/// </summary>
template<typename T>
concept IsMat = requires(T mat) {
    { mat.length() } -> std::convertible_to<int>;
    typename T::col_type;
    typename T::row_type;
};

// Вспомогательные трейты для размеров
template<IsMat T>
struct mat_dimensions;

template<> struct mat_dimensions<glm::mat2> { static constexpr int rows = 2, cols = 2; };
template<> struct mat_dimensions<glm::mat3> { static constexpr int rows = 3, cols = 3; };
template<> struct mat_dimensions<glm::mat4> { static constexpr int rows = 4, cols = 4; };
template<> struct mat_dimensions<glm::mat2x3> { static constexpr int rows = 2, cols = 3; };
template<> struct mat_dimensions<glm::mat3x2> { static constexpr int rows = 3, cols = 2; };

template<IsMat T>
std::ostream& operator<<(std::ostream& os, const T& data) {
    constexpr int rows = mat_dimensions<T>::rows;
    constexpr int cols = mat_dimensions<T>::cols;

    auto current_stride = get_cursor_position().x;
    std::string str_stride(current_stride, ' ');

    os << str_stride << "glm::mat" << rows << 'x' << cols << '\n';
    for (int i = 0; i < rows; i++) {
        os << str_stride;
        for (int j = 0; j < cols; j++) {
            os << data[i][j];
            if (j < cols - 1) os << " ";
        }
        if (i < rows - 1) os << '\n';
    }
    return os;
}

#include "window.inl"