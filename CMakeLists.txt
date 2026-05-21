cmake_minimum_required(VERSION 3.20)

# Задаем версию стандарта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# НАЗВАНИЕ ПРОЕКТА
project(GraphLayouts VERSION 1.0 LANGUAGES CXX)

### Заполняем файлы проекта
# Хедеры
set("${CMAKE_PROJECT_NAME}_LIB_H"
    include/window/window.h
    include/window/window.inl
    include/graph_layouts/attribute_layout/attribute_layout.h
)
# Еденицы трансляции
set("${CMAKE_PROJECT_NAME}_LIB_CPP"
    src/graph_layouts/attribute_layout/attribute_layout.cpp
)

# Внешние либы для библиотеки (например: Glad, GLFW)
include(FetchContent)

add_subdirectory(libs/introspector)

find_package(gpuBuffers QUIET)

if(NOT gpuBuffers_FOUND)
    message(STATUS "gpuBuffers не найден локально. Качаем с GitHub...")

    FetchContent_Declare(
        gpuBuffers
        GIT_REPOSITORY https://github.com/xXxslavchickxXx/gpuBuffers.git
        GIT_TAG        main
        
        # --- ВОТ ЭТИ ДВЕ СТРОКИ ДЕЛАЮТ МАГИЮ ---
        # Указываем качать исходники прямо в корень твоего проекта в папку libs
        SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/libs/gpuBuffers"
        
        # Временные файлы сборки (объектники, логи) пусть остаются внутри out/build
        BINARY_DIR     "${CMAKE_CURRENT_BINARY_DIR}/gpuBuffers_build"
    )

    FetchContent_MakeAvailable(gpuBuffers)
endif()

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
set("${CMAKE_PROJECT_NAME}_LIBS"
    gpuBuffers
    shaderIntrospector
    GLEW::GLEW
    glfw
)

# Внешние либы только для примера (например: либа с вашим модулем)
add_subdirectory(libs/shader)
set("${CMAKE_PROJECT_NAME}_EXAMPLE_LIBS"
    shaderLib
)

# Настройка опции создания исполняемого файла
option(${CMAKE_PROJECT_NAME}_BUILD_EXAMPLE "Построить пример для проекта" ON)
option(${CMAKE_PROJECT_NAME}_ASSET_TO_BUILD "Копировать ассеты в папку сборки" ON)


### Проверка, надо ли вообще либу собирать
if(${CMAKE_PROJECT_NAME}_LIB_H OR ${CMAKE_PROJECT_NAME}_LIB_CPP)
    
    # Автоматический выбор типа
    if(NOT ${CMAKE_PROJECT_NAME}_LIB_CPP)
        set(LIB_TYPE "INTERFACE")
        set(SCOPE_TYPE "INTERFACE")
    else()
        set(LIB_TYPE "STATIC")
        set(SCOPE_TYPE "PUBLIC")
    endif()
    
    message(STATUS "библиотека ${CMAKE_PROJECT_NAME} признана как: ${LIB_TYPE}")

    # Создание таргета
    if(LIB_TYPE STREQUAL "INTERFACE")
        add_library(${CMAKE_PROJECT_NAME} ${LIB_TYPE})
        # Разыменовываем список хедера через двойной знак $
        target_sources(${CMAKE_PROJECT_NAME} ${LIB_TYPE} ${${CMAKE_PROJECT_NAME}_LIB_H})
    else()
        add_library(${CMAKE_PROJECT_NAME} ${LIB_TYPE}
            ${${CMAKE_PROJECT_NAME}_LIB_H}
            ${${CMAKE_PROJECT_NAME}_LIB_CPP}
        )
    endif()

    # Настройка инклудов
    target_include_directories(${CMAKE_PROJECT_NAME} ${SCOPE_TYPE}
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    # Включение в либу доп либ      
    if(${CMAKE_PROJECT_NAME}_LIBS)
        target_link_libraries(${CMAKE_PROJECT_NAME} 
            ${${CMAKE_PROJECT_NAME}_LIBS}
        )
    endif()
    
endif()

### Исполняемый файл
if(${CMAKE_PROJECT_NAME}_BUILD_EXAMPLE)
    message(STATUS "Сборка примера для ${CMAKE_PROJECT_NAME} включена!")
        
    add_executable(${CMAKE_PROJECT_NAME}_example main.cpp)
        
    # Линкуем саму нашу созданную либу
    if(TARGET ${CMAKE_PROJECT_NAME})
        target_link_libraries(${CMAKE_PROJECT_NAME}_example PUBLIC ${CMAKE_PROJECT_NAME})
    endif()
        
    # Линкуем дополнительные либы для примера, если они заданы
    if(${CMAKE_PROJECT_NAME}_EXAMPLE_LIBS)
        target_link_libraries(${CMAKE_PROJECT_NAME}_example PUBLIC ${${CMAKE_PROJECT_NAME}_EXAMPLE_LIBS})
    endif()

    # Логика копирования ассетов
    if(${CMAKE_PROJECT_NAME}_ASSET_TO_BUILD)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
            add_custom_command(TARGET ${CMAKE_PROJECT_NAME}_example POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                    "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}_example>/assets"
            )
        else()
            message(WARNING "Папка assets не найдена в ${CMAKE_CURRENT_SOURCE_DIR}, копирование отменено!")
        endif()
    endif()    
endif()