#pragma once

#include <type_traits>
#include <concepts>

namespace ag::concepts {
    // является ли тип контейнером
    template<typename T>
    concept Container = requires(T loc) {
        typename T::value_type; // У контейнера должен быть тип элементов
        loc.begin();            // У контейнера должен быть метод begin()
        loc.end();              // У контейнера должен быть метод end()
    };

    // является ли тип обычным одиночным значением
    template<typename T>
    concept Scalar = !Container<T> && std::is_trivially_copyable_v<T>;
}