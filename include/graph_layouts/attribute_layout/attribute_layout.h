#pragma once

#include <vertex/vertex_array.h>
#include <AGreflection.h>
#include <graph_layouts/attribute_layout/proxy/proxy.h>

#include <unordered_map>
#include <string>
#include <memory>

namespace ag::layout {

	class attribute_layout {
		shader::attribute_reflector reflector;
		std::shared_ptr<gpu::vertex_array> vao;

		std::unordered_map<std::string, attribute_proxy> attributes;

	public:
		attribute_layout(GLuint program, GLenum draw_mode = GL_STATIC_DRAW);

		// правило пяти
		attribute_layout(const attribute_layout&) = delete;
		attribute_layout& operator=(const attribute_layout&) = delete;
		attribute_layout(attribute_layout&&) = default;
		attribute_layout& operator=(attribute_layout&&) = default;
		~attribute_layout() = default;
		
		attribute_proxy operator[](const std::string& name);
		indexed_buffer* index_buffer() {return vao->get_index_buffer(); }

		void bind() { vao->bind(); }
		size_t size() const { return attributes.size(); }

	private:
		void link(GLenum draw_mode);

	};
}