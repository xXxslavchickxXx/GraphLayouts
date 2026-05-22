#pragma once

#include <vertex/vertex_array.hpp>
#include <map>

namespace ag::layout {
	class attribute_proxy {
		std::map<size_t, GLint> buffers;
		std::shared_ptr<gpu::vertex_array> vao;

	public:
		attribute_proxy() = default;

		attribute_proxy(const std::vector<GLint>& vec, std::shared_ptr<gpu::vertex_array> vao)
			: vao(vao) {
			for (size_t i = 0; i < vec.size(); i++) {
				buffers[i] = vec[i];
			}
		}

		vertex_buffer* operator[](size_t i) {
			if (!vao || buffers.empty()) return nullptr;
			return (*vao)[buffers[i]];
		}

		vertex_buffer* operator->() {
			if (!vao || buffers.empty()) return nullptr;
			return (*vao)[buffers[0]];
		}
	};
}