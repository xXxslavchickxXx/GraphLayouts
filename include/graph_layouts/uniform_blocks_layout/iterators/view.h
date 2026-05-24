#pragma once

#include <graph_layouts/uniform_blocks_layout/iterators/template_iterator.h>
#include <templates/opengl_iterator.h>
#include <graph_layouts/concepts/concepts.h>

#include <graph_layouts/uniform_blocks_layout/iterators/field.h>

#include <tuple>

namespace ag::iterators {
	class uniform_block_view :
	public opengl_template_iterator<uniform_block_view, uniform_block_field>,
	public template_uniform_setter<uniform_block_view, uniform_block_member>
	{
	using Setter = template_uniform_setter<uniform_block_view, uniform_block_member>;
	using Iterator = opengl_template_iterator<uniform_block_view, uniform_block_field>;
	public:
		using Setter::Setter;
		using Setter::operator=;
		using Iterator::operator[];
		using Iterator::operator->;
		using Iterator::add_entry;
		using Iterator::size;
		using Iterator::begin;
		using Iterator::end;

		template<ag::concepts::Container T>
		void set(const T& value) {
			set_impl(value);
		}

		template<ag::concepts::Scalar... T>
		void set(const T&... args) {
			using FirstType = std::tuple_element_t<0, std::tuple<T...>>;
			std::vector<FirstType> vec{ args... };

			set_impl(vec);
		}

		template<ag::concepts::Container T>
		void set_impl(const T& value) {
			auto buffer = buffer_ref.lock();
			if (!buffer) {
				throw std::runtime_error("UBO is dead");
			}

			size_t input_size_bytes = value.size() * sizeof(typename T::value_type);

			if (input_size_bytes > static_cast<size_t>(composit.byte_size())) {
				throw std::runtime_error("Passed data size exceeds uniform block member size on GPU");
			}

			buffer->upload_part(value.data(), input_size_bytes, composit.offset);
		}

		template<ag::concepts::Container T>
		T get() const {
			auto buffer = buffer_ref.lock();
			if (!buffer) throw std::runtime_error("UBO is dead");

			T container;
			container.resize(composit.size);

			size_t size_bytes = container.size() * sizeof(typename T::value_type);
			buffer->download_part(size_bytes, container.data(), composit.offset);

			return container;
		}

		friend std::ostream& operator<<(std::ostream& os, const uniform_block_view& composit) {
			os << composit.composit;
			return os;
		}
	};
}