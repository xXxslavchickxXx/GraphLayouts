#include <graph_layouts/uniform_blocks_layout/uniform_blocks_layout.h>
#include <uniform/uniform_buffer.h>

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

        GLuint currentBinding = 1;  // текущий binding point

        for (const auto& [block_name, datas] : reflector) {
            // Получаем общий размер для массива блоков
            GLint totalSize = 0;
            for (const auto& data : datas) {
                totalSize += data.byte_size;
            }

            // Создаем ОДИН буфер на весь массив блоков
            auto buffer_ = std::make_shared<ag::uniform_buffer>(draw_mode);
            buffer_->allocate(totalSize);

            // Создаем последовательность для этого массива блоков
            uniform_block_sequence sequence(buffer_, datas, block_name);

            GLint offset = 0;  // смещение для текущего блока в буфере

            for (auto data : datas) {
                if (data.binding == 0) {
                    // Привязываем диапазон буфера к binding point
                    buffer_->bind_range(currentBinding, offset, data.byte_size);
                    // Привязываем uniform block в шейдере к тому же binding point
                    ag::uniform_buffer::bind_block(program, data.name, currentBinding);
                    // Сохраняем binding в данных
                    data.binding = currentBinding;
                    currentBinding++;
                }

                // Создаем прокси для блока
                uniform_block_data dater(buffer_, data, data.name);

                // Добавляем все мемберы
                for (const auto& [view_name, view] : data) {
                    uniform_block_view viewer( buffer_, view, view_name);
                    for (const auto& handle : view) {
                        viewer.add_entry(uniform_block_field(buffer_, handle));
                    }
                    dater[view_name] = (std::move(viewer));
                }

                // Сохраняем binding в прокси
                dater.get_raw().binding = data.binding;
                sequence.add_entry(std::move(dater));

                // Сдвигаем offset и binding для следующего элемента
                offset += data.byte_size;
            }
            blocks.insert_or_assign(block_name, std::move(sequence));
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
            os << proxy;
            os << "--------------------------------------------------\n";
        }

        return os;
    }
}