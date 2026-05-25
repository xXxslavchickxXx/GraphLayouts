#pragma once

#include <graph_layouts/uniform_blocks_layout/iterators/view.h>

#include <uniformBlockReflector/uniformBlockData.h>

namespace ag::iterators {
    class uniform_block_data :
    public umtus_template<uniform_block_data, uniform_block_view, uniform_block_data_info>
    {
        using Base = umtus_template<uniform_block_data, uniform_block_view, uniform_block_data_info>;
    public:
        using Base::Base;

        template<typename T>
        void operator=(const T& value) {
            Base::operator=(value);
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

            buffer->upload_part(value, composit.offset);
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

        friend class uniform_block_sequence;
    };
}