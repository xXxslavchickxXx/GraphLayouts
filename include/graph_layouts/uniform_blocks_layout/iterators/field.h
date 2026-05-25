#pragma once

#include <graph_layouts/uniform_blocks_layout/iterators/template_iterator.h>
#include <uniformBlockReflector/uniformBlockField.h>
#include <graph_layouts/concepts/concepts.h>

namespace ag::iterators {
	class uniform_block_field :
	public template_uniform_setter<uniform_block_field, uniform_block_field_info>
	{
	using Setter = template_uniform_setter<uniform_block_field, uniform_block_field_info>;
	public:
		using Setter::Setter;
		template<typename T>
		void operator=(const T& value) {
			Setter::operator=(value);
		}

		template<ag::concepts::Scalar T>
		void set(const T& value) {
			set_impl(value);
		}

		template<ag::concepts::Scalar T>
		void set_impl(const T& value) {
			auto buffer = buffer_ref.lock();
			if (!buffer) {
				throw std::runtime_error("UBO is dead");
			}

			size_t input_size_bytes = sizeof(T);

			if (input_size_bytes > static_cast<size_t>(composit.byte_size)) {
				throw std::runtime_error("Passed data size exceeds uniform block member size on GPU");
			}

			buffer->upload_part(value, composit.offset);
		}

		template<typename T>
		T get() const {
			if (auto buffer = buffer_ref.lock()) {
				T value;
				buffer->download_part(sizeof(T), &value, composit.offset);
				return value;
			}
			throw std::runtime_error("UBO is dead");
		}
		friend std::ostream& operator<<(std::ostream& os, const uniform_block_field& composit) {
			os << composit.composit;
			return os;
		}
	};
}