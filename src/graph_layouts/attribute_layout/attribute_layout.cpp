#include <graph_layouts/attribute_layout/attribute_layout.h>
#include <graph_layouts/attribute_layout/traits.h>

#include <iostream>

namespace ag::layout {
	attribute_proxy attribute_layout::operator[](const std::string& name) {
		return attributes[name];
	}

	attribute_layout::attribute_layout(GLuint program, GLenum draw_mode) :
	vao(std::make_shared<gpu::vertex_array>()), reflector(program), attributes()
	{
		link(draw_mode);
		vao->add_index_buffer(draw_mode);
	}

	void attribute_layout::link(GLenum draw_mode) {
		for (auto [name, entry] : reflector) {
			std::vector<GLint> locations;
			for (int i = 0; i < entry.size(); i++) {
				auto descriptor = type_to_descriptor(entry[i].type);
				
				vao->add_buffer(draw_mode, entry[i].location, descriptor.components, descriptor.byte_size);
				locations.push_back(entry[i].location);
			}

			attributes.emplace(name, attribute_proxy(locations, vao));
		}
	}
}