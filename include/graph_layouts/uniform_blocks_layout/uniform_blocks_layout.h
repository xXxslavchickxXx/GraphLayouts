#pragma once

#include <AGreflection.h>
#include <graph_layouts/uniform_blocks_layout/iterators/template_iterator.h>

#include <graph_layouts/uniform_blocks_layout/iterators/sequence.h>

#include <unordered_map>
#include <string>
#include <memory>

namespace ag::layout {
	class uniform_blocks_layout {
		shader::uniform_block_reflector reflector;
		std::unordered_map<std::string, iterators::uniform_block_sequence> blocks;

	public:
		uniform_blocks_layout(GLuint program, GLenum draw_mode = GL_STATIC_DRAW);

		// правило пяти
		uniform_blocks_layout(const uniform_blocks_layout&) = delete;
		uniform_blocks_layout& operator=(const uniform_blocks_layout&) = delete;
		uniform_blocks_layout(uniform_blocks_layout&&) = default;
		uniform_blocks_layout& operator=(uniform_blocks_layout&&) = default;
		~uniform_blocks_layout() = default;

		iterators::uniform_block_sequence operator[](const std::string& name) {
			auto it = blocks.find(name);

			if (it == blocks.end()) {
				throw std::runtime_error("this block doesn't exists");
			}

			return it->second;
		}

		size_t size() const { return blocks.size(); }

		friend std::ostream& operator<<(std::ostream& os, uniform_blocks_layout& layout);

	private:
		void link(GLenum draw_mode);
	};
}