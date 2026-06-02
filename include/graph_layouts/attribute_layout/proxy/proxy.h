#pragma once

#include <vertex/vertex_array.h>
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

		auto operator[](size_t i) {
			if (!vao || buffers.empty()) std::cerr << "этот буффер мертв";
			return (*vao)[buffers[i]];
		}

		auto operator->() {
			if (!vao || buffers.empty()) std::cerr << "этот буффер мертв";
			return (*vao)[buffers[0]];
		}
	};
}