#include <vertex/vertex_array.hpp>
#include <AGreflection.h>

namespace ag::layout {
	class attribute_layout {
		gpu::vertex_array vao;
		shader::AttributeReflector reflector;

	public:
		attribute_layout(GLuint program);
	};
}