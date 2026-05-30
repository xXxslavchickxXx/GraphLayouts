cmake_minimum_required(VERSION 3.20)

# Задаем версию стандарта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# НАЗВАНИЕ ПРОЕКТА
project(GraphLayouts VERSION 1.0 LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDependency.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildLib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildExample.cmake)

# может вы хотите какие то файлы в папках автоматически находить
file(GLOB_RECURSE ${PROJECT_NAME}_FILES
    "include/*.h"
    "include/*.inl"
)

message(STATUS "Found files: ${${PROJECT_NAME}_FILES}")

# 1. Путь к файлу-точке входа для приложения-примера (например: main.cpp)
set(${PROJECT_NAME}_EXAMPLE_FILEPATH main.cpp)

# 2. Ручное указание файлов твоей библиотеки (если хочешь перечислить конкретные файлы)
set(${PROJECT_NAME}_LIB_H
    include/graph_layouts/attribute_layout/attribute_layout.h
    "include/graph_layouts/attribute_layout/traits.h"
    "include/graph_layouts/attribute_layout/proxy/proxy.h"
    include/graph_layouts/uniform_blocks_layout/uniform_blocks_layout.h
    "include/graph_layouts/uniform_blocks_layout/iterators/template_iterator.h"
    "include/graph_layouts/uniform_blocks_layout/iterators/field.h"
    include/graph_layouts/shader_layout/shader_layout.h
    "include/graph_layouts/uniform_blocks_layout/iterators/view.h"
    "include/graph_layouts/concepts/concepts.h"
    "include/graph_layouts/uniform_blocks_layout/iterators/data.h"
    "include/graph_layouts/uniform_blocks_layout/iterators/sequence.h"
)
set(${PROJECT_NAME}_LIB_CPP
    src/graph_layouts/attribute_layout/attribute_layout.cpp
    src/graph_layouts/uniform_blocks_layout/uniform_blocks_layout.cpp
)

# 3. Подключение инклудов
set(${PROJECT_NAME}_INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/graph_layouts/shader_layout
)

# 3. Внешние либы для линковки к твоей основной библиотеке (например: gpuBuffers)
find_package(glm)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
add_external_dependency(gpuBuffers "https://github.com/xXxslavchickxXx/gpuBuffers.git")
add_external_dependency(shaderIntrospector "https://github.com/xXxslavchickxXx/ShaderIntrospector")
set(${PROJECT_NAME}_LIBS
    glm::glm
    gpuBuffers
    shaderIntrospector
    GLEW::GLEW
    glfw
)

# 4. Либы, нужные ТОЛЬКО для запуска приложения-примера (например: shaderLib)
add_external_dependency(shaderLib "https://github.com/xXxslavchickxXx/shader_lib.git")
set(${PROJECT_NAME}_EXAMPLE_LIBS
    shaderLib
)

# Опции сборки
option(${PROJECT_NAME}_BUILD_EXAMPLE "Построить пример для проекта" OFF)
option(${PROJECT_NAME}_ASSET_TO_BUILD "Копировать ассеты в папку сборки" OFF)

# Собираем либу
create_library(${PROJECT_NAME}
    HEADERS ${${PROJECT_NAME}_LIB_H}
    SOURCES ${${PROJECT_NAME}_LIB_CPP}
    INCLUDE_DIRS ${${PROJECT_NAME}_INCLUDE}
    LIBS ${${PROJECT_NAME}_LIBS}
)

if(${PROJECT_NAME}_BUILD_EXAMPLE)
    create_executable(${PROJECT_NAME}_example
        SOURCE ${${PROJECT_NAME}_EXAMPLE_FILEPATH}
        COPY_ASSETS ${PROJECT_NAME}_ASSET_TO_BUILD
        LIBS ${PROJECT_NAME} ${${PROJECT_NAME}_EXAMPLE_LIBS}
    )
endif()