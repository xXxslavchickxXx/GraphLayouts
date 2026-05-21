Window::Window() : Window(800, 600) {}
Window::Window(int width, int height) : Window(width, height, "window") {}
Window::Window(int width, int height, const std::string& title) :
    _title(title)
{
    glfwInit();

    windowPtr = glfwCreateWindow(width, height, "jopa", nullptr, nullptr);
    glfwMakeContextCurrent(windowPtr);
    glClearColor(0.3, 0.4, 0.6, 1);

}

void Window::setClearColor(int R, int G, int B, int A) {
    glClearColor(R, G, B, A);
}

template<typename F>
void Window::run(F&& lambda) {
    while (!glfwWindowShouldClose(windowPtr)) {
        glClear(GL_COLOR_BUFFER_BIT);

        std::forward<F>(lambda)();

        glfwSwapBuffers(windowPtr);
        glfwPollEvents();
    }
}