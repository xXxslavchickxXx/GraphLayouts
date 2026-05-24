#pragma once

#include <graph_layouts/uniform_blocks_layout/iterators/view.h>

namespace ag::iterators {
    class uniform_block_data :
    public template_uniform_setter<uniform_block_data, uniform_block_info>,
    public template_container_iterator<uniform_block_data,
        std::unordered_map<std::string, uniform_block_view>,
        const std::string&, uniform_block_view>
    {
    using Setter = template_uniform_setter<uniform_block_data, uniform_block_info>;
    using Iterator = template_container_iterator<uniform_block_data,
        std::unordered_map<std::string, uniform_block_view>,
        const std::string&, uniform_block_view>;
    public:
        uniform_block_data();
        uniform_block_data(std::shared_ptr<ag::uniform_buffer> buffer_ref, uniform_block_info& info)
            : Setter(buffer_ref, info) {}

        using Iterator::begin;
        using Iterator::end;
        using Iterator::size;
        using Iterator::operator[];
        using Setter::operator=;

        void add_member(uniform_block_view&& member) {
            this->entries[member.name] = std::move(member);
        }

        template<typename T>
        void set_impl(const T& value) {
            auto buffer = buffer_ref.lock();
            if (!buffer) {
                throw std::runtime_error("UBO is dead");
            }

            size_t input_size_bytes = sizeof(T);

            if (input_size_bytes > static_cast<size_t>(composit.byte_size)) {
                throw std::runtime_error("Passed structure size exceeds uniform block size on GPU");
            }

            buffer->upload_part(value);
        }

        template<typename T>
        T get() const {
            auto buffer = buffer_ref.lock();
            if (!buffer) {
                throw std::runtime_error("UBO is dead");
            }

            T value;
            size_t size_bytes = sizeof(T);

            if (size_bytes > static_cast<size_t>(composit.byte_size)) {
                throw std::runtime_error("Requested structure size exceeds uniform block size");
            }

            buffer->download_part(size_bytes, &value);
            return value;
        }

        void print(std::ostream& os, int indent = 0) const override {
            composit.print(os, indent);
        }

        friend std::ostream& operator<<(std::ostream& os, const uniform_block_data& composit) {
            os << composit.composit;
            return os;
        }
    };
}