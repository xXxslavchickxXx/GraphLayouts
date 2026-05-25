#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <window/window.h>

#include <graph_layouts/attribute_layout/attribute_layout.h>
#include <graph_layouts/uniform_blocks_layout/uniform_blocks_layout.h>

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
    camera_data() = default;
    camera_data(glm::mat4 uview, glm::mat4 uproj) : uView(uview), uProj(uproj) {}
};

int main()
{
    // Создаем окно
    Window win;
    glewInit();

    auto program = shader::ShaderProgram::from_path("assets/shaders/V.glsl", "assets/shaders/F.glsl");

    std::vector<glm::vec3> points = {
        glm::vec3(0.3f,  0.5f,  0.f),
        glm::vec3(-0.3f,  0.5f,  0.f),
        glm::vec3(0.3f, -0.5f,  0.f),
        glm::vec3(-0.3f, -0.5f,  0.f)
    };
    std::vector<glm::vec3> colors = {
        glm::vec3(0.3f,  0.5f,  0.f),
        glm::vec3(0.3f,  0.5f,  0.f),
        glm::vec3(0.3f,  0.5f,  0.f),
        glm::vec3(0.3f,  0.5f,  0.f)
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,
        1, 2, 3
    };

    camera_data camera_0 (
        glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),  // позиция камеры (отодвинули назад)
            glm::vec3(0.0f, 0.0f, 0.0f),  // смотрим в центр
            glm::vec3(0.0f, 1.0f, 0.0f)   // up вектор
        ),
        glm::ortho(-1.f, 1.f, -1.f, 1.f, -5.f, 5.f)
    );

    /// Апи лайаутов и того что есть
    // Рефлектор
    shader::uniform_reflector reflector(program.getId());

    reflector["model"] = glm::mat4(1.f);
    reflector["uView"] = camera_0.uView;
    reflector["uProj"] = camera_0.uProj;

    // Аттрибуты
    ag::layout::attribute_layout a_layout(program.getId());

    a_layout["aPos"]->upload(points);
    a_layout["aColor"][1]->upload(colors);
    a_layout.index_buffer()->upload(indices);

    // Юниформ блоки
    ag::layout::uniform_blocks_layout u_layout(program.getId());

    auto gameLoop = [&]() {
        program.bind();
        a_layout.bind();

        static float angle = 0.f;
        static glm::vec3 rotate_axis{ 1.f, 0.f, 0.f };
        angle += 0.0001;

        reflector["model"] = glm::rotate(glm::mat4(1.f), 3 * angle, rotate_axis);

        static glm::mat4 view_matrix = glm::mat4(1.f);
        view_matrix = glm::lookAt(
            glm::vec3(3.0f * cos(0.5 * angle), 0.f, 3.0f * sin(0.5 * angle)),  // вращаем камеру во круг центра координат
            glm::vec3(0.0f, 0.0f, 0.0f),  // смотрим в центр
            glm::vec3(0.0f, 1.0f, 0.0f)   // up вектор
        );
            
        reflector["uView"] = view_matrix;

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    };

    // Запускаем игровой цикл
    win.run(gameLoop);
}