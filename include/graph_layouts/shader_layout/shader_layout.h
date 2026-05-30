#pragma once

#include <graph_layouts/attribute_layout/attribute_layout.h>
#include <graph_layouts/uniform_blocks_layout/uniform_blocks_layout.h>
#include <uniformReflector/uniformReflector.h>

namespace ag::layout {
	class shader_layout {
	public:
		shader::uniform_reflector uniform;
		uniform_blocks_layout block;
		attribute_layout attribute;

		shader_layout() = delete;
		shader_layout(GLint program) : uniform(program), block(program), attribute(program) {}

		shader_layout(shader_layout&&) = default;
		shader_layout& operator=(shader_layout&&) = default;

		void bind() {
			attribute.bind();
		}
	};
}