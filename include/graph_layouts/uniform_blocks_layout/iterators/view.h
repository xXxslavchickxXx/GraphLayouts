#pragma once

#include <uniformBlockReflector/uniformBlockView.h>
#include <tuple>

#include <graph_layouts/uniform_blocks_layout/iterators/field.h>


namespace ag::iterators {
	class uniform_block_view :
	public oitus_template<uniform_block_view, uniform_block_field, uniform_block_view_info>
	{
		using Base = oitus_template<uniform_block_view, uniform_block_field, uniform_block_view_info>;
		using Base::entries;

	public:
		using Base::Base;
		using Base::operator[];

		template<typename T>
		void operator=(const T& value) {
			Base::operator=(value);
		}

		template<ag::concepts::TriviallyCopyable T>
		void set_impl(const T& value) {
			auto buffer = buffer_ref.lock();
			if (!buffer) {
				throw std::runtime_error("UBO is dead");
			}

			size_t input_size_bytes = sizeof(T);

			if (input_size_bytes > static_cast<size_t>(composit.byte_size())) {
				throw std::runtime_error("Passed data size exceeds uniform block member size on GPU");
			}

			buffer->upload_part(value, composit.offset);
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
	};
}