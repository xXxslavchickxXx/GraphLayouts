#pragma once

#include <GL/glew.h>

namespace ag::layout {
	struct component_descriptor {
		GLenum type;
		GLint components;
		GLuint byte_size;
	};

	inline component_descriptor type_to_descriptor(GLenum gl_type) {
		switch (gl_type) {
			// Скаляры
		case GL_FLOAT:        return { GL_FLOAT, 1 };
		case GL_INT:          return { GL_INT, 1 };
		case GL_UNSIGNED_INT: return { GL_UNSIGNED_INT, 1 };
		case GL_BOOL:         return { GL_BOOL, 1 };

					// Векторы 2
		case GL_FLOAT_VEC2:   return { GL_FLOAT, 2 };
		case GL_INT_VEC2:     return { GL_INT, 2 };
		case GL_UNSIGNED_INT_VEC2: return { GL_UNSIGNED_INT, 2 };
		case GL_BOOL_VEC2:    return { GL_BOOL, 2 };

						 // Векторы 3
		case GL_FLOAT_VEC3:   return { GL_FLOAT, 3 };
		case GL_INT_VEC3:     return { GL_INT, 3 };
		case GL_UNSIGNED_INT_VEC3: return { GL_UNSIGNED_INT, 3 };
		case GL_BOOL_VEC3:    return { GL_BOOL, 3 };

						 // Векторы 4
		case GL_FLOAT_VEC4:   return { GL_FLOAT, 4 };
		case GL_INT_VEC4:     return { GL_INT, 4 };
		case GL_UNSIGNED_INT_VEC4: return { GL_UNSIGNED_INT, 4 };
		case GL_BOOL_VEC4:    return { GL_BOOL, 4 };

						 // Матрицы (каждая состоит из float)
		case GL_FLOAT_MAT2:   return { GL_FLOAT, 4 };      // 2x2 = 4 компонента
		case GL_FLOAT_MAT3:   return { GL_FLOAT, 9 };      // 3x3 = 9 компонентов
		case GL_FLOAT_MAT4:   return { GL_FLOAT, 16 };     // 4x4 = 16 компонентов

		case GL_FLOAT_MAT2x3: return { GL_FLOAT, 6 };      // 2x3 = 6 компонентов
		case GL_FLOAT_MAT2x4: return { GL_FLOAT, 8 };      // 2x4 = 8 компонентов
		case GL_FLOAT_MAT3x2: return { GL_FLOAT, 6 };      // 3x2 = 6 компонентов
		case GL_FLOAT_MAT3x4: return { GL_FLOAT, 12 };     // 3x4 = 12 компонентов
		case GL_FLOAT_MAT4x2: return { GL_FLOAT, 8 };      // 4x2 = 8 компонентов
		case GL_FLOAT_MAT4x3: return { GL_FLOAT, 12 };     // 4x3 = 12 компонентов

							// Сэмплеры (как int — номер текстурного юнита)
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_CUBE_MAP_ARRAY:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_1D_SHADOW:
			return { GL_INT, 1 };

			// Image типы (тоже как int)
		case GL_IMAGE_1D:
		case GL_IMAGE_2D:
		case GL_IMAGE_3D:
		case GL_IMAGE_CUBE:
		case GL_IMAGE_2D_ARRAY:
		case GL_IMAGE_1D_ARRAY:
		case GL_IMAGE_BUFFER:
			return { GL_INT, 1 };

			// Atomic counters (как unsigned int)
		case GL_UNSIGNED_INT_ATOMIC_COUNTER:
			return { GL_UNSIGNED_INT, 1 };

			// Double (если поддерживается)
		case GL_DOUBLE:       return { GL_DOUBLE, 1 };
		case GL_DOUBLE_VEC2:  return { GL_DOUBLE, 2 };
		case GL_DOUBLE_VEC3:  return { GL_DOUBLE, 3 };
		case GL_DOUBLE_VEC4:  return { GL_DOUBLE, 4 };
		case GL_DOUBLE_MAT2:  return { GL_DOUBLE, 4 };
		case GL_DOUBLE_MAT3:  return { GL_DOUBLE, 9 };
		case GL_DOUBLE_MAT4:  return { GL_DOUBLE, 16 };

		default:              return { GL_FLOAT, 0 };  // неизвестный тип
		}
	}
}