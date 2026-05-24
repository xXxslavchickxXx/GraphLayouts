#include <graph_layouts/uniform_blocks_layout/uniform_blocks_layout.h>

#include <format>

namespace ag::layout {
    std::ostream& operator<<(std::ostream& os, const glm::mat4& mat) {
        for (size_t i = 0; i < mat.length(); i++)
            os << std::format(" {}, {}, {}, {} \n", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
        return os;
    }
	uniform_blocks_layout::uniform_blocks_layout(GLuint program, GLenum draw_mode) 
	: reflector(program), blocks()
	{
        using namespace iterators;

        for (auto [block_name, datas] : reflector) {
            auto buffer_ = std::make_shared<ag::uniform_buffer>(draw_mode);
            buffer_->allocate(datas.byte_size());
            buffer_->bind_base(datas.binding);

            uniform_block_sequence sequence(buffer_, datas, program, block_name);
            for (auto data : datas) {
                uniform_block_data dater(buffer_, data);
                for (auto [view_name, view] : data) {
                    uniform_block_view viewer(buffer_, view);
                    for (auto handle : view) {
                        viewer.add_entry(uniform_block_field(buffer_, handle));
                    }
                    dater.add_member(std::move(viewer));
                }
                sequence.add_entry(std::move(dater));
            }

            blocks.insert_or_assign(block_name, sequence);
        }
	}

    std::ostream& operator<<(std::ostream& os, uniform_blocks_layout& layout) {
        os << "==================================================\n";
        os << "          OPENGL UNIFORM BLOCKS LAYOUT            \n";
        os << "==================================================\n";

        if (layout.blocks.empty()) {
            os << " [Layout is empty / No active uniform blocks]\n";
            return os;
        }

        // Просто бежим по нашей мапе прокси-объектов
        for (const auto& [block_name, proxy] : layout.blocks) {
            // Вызываем метод print, который ты реализовал в uniform_proxy.
            // Он сам по цепочке вызовет принт у sequence, data, view и field!
            proxy.print(os, 0);
            os << "--------------------------------------------------\n";
        }

        return os;
    }
}