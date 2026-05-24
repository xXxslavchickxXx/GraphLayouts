
cmake_minimum_required(VERSION 3.20)
include(FetchContent)

# Задаем версию стандарта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Название проекта
project(GraphLayouts VERSION 1.0 LANGUAGES CXX)

### Определение функций
# Функция скачивания необходимых зависимостей репы(часто не оффициальные модули)
function(add_external_dependency LIB_NAME GIT_URL)
    set(LOCAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/${LIB_NAME}")
    set(NEED_DOWNLOAD FALSE)

    if(EXISTS "${LOCAL_PATH}/CMakeLists.txt")
        message(STATUS "[AUTO-DEP] ${LIB_NAME} найден локально. Проверяем актуальность версии...")
        
        # 1. Получаем хэш локального коммита
        execute_process(
            COMMAND git rev-parse HEAD
            WORKING_DIRECTORY "${LOCAL_PATH}"
            OUTPUT_VARIABLE LOCAL_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        # 2. Получаем хэш последнего коммита с сервера
        execute_process(
            COMMAND git ls-remote ${GIT_URL} refs/heads/main
            OUTPUT_VARIABLE REMOTE_LS
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        # Вырезаем чистый хэш из строки ответа ls-remote
        string(REGEX MATCH "^[0-9a-f]+" REMOTE_HASH "${REMOTE_LS}")

        # 3. Сравниваем хэши
        if(NOT "${LOCAL_HASH}" STREQUAL "${REMOTE_HASH}" AND NOT "${REMOTE_HASH}" STREQUAL "")
            message(STATUS "[AUTO-DEP] Обнаружена новая версия ${LIB_NAME} на GitHub! Перекачиваем...")
            set(NEED_DOWNLOAD TRUE)
            file(REMOVE_RECURSE "${LOCAL_PATH}")
        endif()
    else()
        set(NEED_DOWNLOAD TRUE)
    endif()

    # Логика подключения или скачивания
    if(NOT NEED_DOWNLOAD)
        message(STATUS "[AUTO-DEP] Версия ${LIB_NAME} актуальна. Подключаем...")
        add_subdirectory("${LOCAL_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}_build")
    else()
        message(STATUS "[AUTO-DEP] Скачиваем/Обновляем ${LIB_NAME} с GitHub...")

        # Настройка .gitignore
        set(GITIGNORE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/.gitignore")
        set(IGNORE_ENTRY "libs/${LIB_NAME}/")
        if(EXISTS "${GITIGNORE_FILE}")
            file(READ "${GITIGNORE_FILE}" GITIGNORE_CONTENT)
            string(FIND "${GITIGNORE_CONTENT}" "${IGNORE_ENTRY}" ENTRY_FOUND)
            if(ENTRY_FOUND EQUAL -1)
                file(APPEND "${GITIGNORE_FILE}" "\n${IGNORE_ENTRY}\n")
            endif()
        else()
            file(WRITE "${GITIGNORE_FILE}" "${IGNORE_ENTRY}\n")
        endif()

        # Скачивание через FetchContent
        include(FetchContent)
        FetchContent_Declare(
            ${LIB_NAME}
            GIT_REPOSITORY ${GIT_URL}
            GIT_TAG        main
            SOURCE_DIR     "${LOCAL_PATH}"
            BINARY_DIR     "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}_build"
        )
        FetchContent_MakeAvailable(${LIB_NAME})
    endif()
endfunction()

### Заполняем файлы проекта
# Хедеры
set("${CMAKE_PROJECT_NAME}_LIB_H"
    include/window/window.h
    include/window/window.inl
    include/graph_layouts/attribute_layout/attribute_layout.h
    "include/graph_layouts/attribute_layout/traits.h"
    "include/graph_layouts/attribute_layout/proxy/proxy.h"
    include/graph_layouts/uniform_blocks_layout/uniform_blocks_layout.h
    "include/graph_layouts/uniform_blocks_layout/iterators/template_iterator.h"
    "include/graph_layouts/uniform_blocks_layout/iterators/field.h"
)
# Еденицы трансляции
set("${CMAKE_PROJECT_NAME}_LIB_CPP"
    src/graph_layouts/attribute_layout/attribute_layout.cpp
    src/graph_layouts/uniform_blocks_layout/uniform_blocks_layout.cpp
)

# Внешние либы для библиотеки (например: Glad, GLFW)
add_external_dependency(gpuBuffers "https://github.com/xXxslavchickxXx/gpuBuffers.git")
add_external_dependency(shaderIntrospector "https://github.com/xXxslavchickxXx/ShaderIntrospector")

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
          "include/graph_layouts/uniform_blocks_layout/iterators/view.h" "include/graph_layouts/concepts/concepts.h" "include/graph_layouts/uniform_blocks_layout/iterators/data.h" "include/graph_layouts/uniform_blocks_layout/iterators/sequence.h")
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