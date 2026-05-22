#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <window/window.h>

#include <graph_layouts/attribute_layout/attribute_layout.h>

// Сабдирректории
#include <shaderProgram/ShaderProgram.h>
#include <AGreflection.h>
#include <vertex/vertex_array.hpp>
#include <uniform/uniform_buffer.hpp>

#include <glm/gtc/type_ptr.hpp>

using namespace ag::gpu;

struct camera_data {
    glm::mat4 uView;
    glm::mat4 uProj;
};

int main()
{
    // Создаем окно
    Window win;
    glewInit();

    // Создаем убо
    ag::uniform_buffer ubo;
    ubo.bind_base(0);

    std::vector<glm::vec3> points = {
        glm::vec3(0.3f,  0.5f,  0.f),
        glm::vec3(-0.3f,  0.5f,  0.f),
        glm::vec3(0.3f, -0.5f,  0.f),
        glm::vec3(-0.3f, -0.5f,  0.f)
    };
    std::vector<glm::vec3> colors = {
        glm::vec3(0.3f,  0.5f,  0.f),
        glm::vec3(-0.3f,  0.5f,  0.f),
        glm::vec3(0.3f, -0.5f,  0.f),
        glm::vec3(-0.3f, -0.5f,  0.f)
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,
        1, 2, 3
    };

    camera_data camera{
        glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),  // позиция камеры (отодвинули назад)
            glm::vec3(0.0f, 0.0f, 0.0f),  // смотрим в центр
            glm::vec3(0.0f, 1.0f, 0.0f)   // up вектор
        ),
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f)
    };

    ubo.upload(camera);
    // Создаем шейдерную программу
    auto program = shader::ShaderProgram::from_path("assets/shaders/V.glsl", "assets/shaders/F.glsl");
    ag::uniform_buffer::bind_block(program.getId(), "CameraBlock", 0);

    shader::uniform_reflector reflector(program.getId());
    ag::layout::attribute_layout a_layout(program.getId());

    a_layout["aPos"]->upload(points);
    a_layout["aColor"][1]->upload(colors);
    a_layout.index_buffer()->upload(indices);

    // Настриваем то, что будет происходить в игровом цикле
    auto gameLoop = [&]() {
        program.bind();
        a_layout.bind();

        static float angle = 0.f;
        static glm::vec3 rotate_axis{ 0.f, 0.f, 1.f };
        angle += 0.0001;

        reflector["model"] = glm::rotate(glm::mat4(1.f), angle, rotate_axis);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        };

    // Запускаем игровой цикл
    win.run(gameLoop);
}