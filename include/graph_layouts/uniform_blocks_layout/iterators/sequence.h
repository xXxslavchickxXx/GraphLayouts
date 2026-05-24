#pragma once

#include <graph_layouts/uniform_blocks_layout/iterators/data.h>
#include <uniformBlockReflector/uniformBlockSequence.h>

#include <memory>

namespace ag::iterators {
	class uniform_block_sequence :
		private opengl_template_iterator<uniform_block_sequence, uniform_block_data>,
		private template_uniform_setter<uniform_block_sequence, shader::uniform_block_sequence_info>
	{
		using Setter = template_uniform_setter<uniform_block_sequence, shader::uniform_block_sequence_info>;
		using Iterator = opengl_template_iterator<uniform_block_sequence, uniform_block_data>;

		std::shared_ptr<ag::uniform_buffer> buffer_owner;
	public:
		uniform_block_sequence(std::shared_ptr<ag::uniform_buffer> buffer, const shader::uniform_block_sequence_info& composit,
		GLint program, const std::string& name)
			: buffer_owner(buffer), Setter(buffer, composit), Iterator(name, program) {}

		using Setter::Setter;
		using Setter::operator=;
		using Iterator::operator[];
		using Iterator::operator->;
		using Iterator::print;
		using Iterator::add_entry;
		using Iterator::size;
		using Iterator::begin;
		using Iterator::end;

		template<ag::concepts::Container T>
		void set(const T& value) {
			set_impl(value);
		}

		// Вариативный сеттер (через запятую: sequence.set(cam1, cam2, cam3))
		/*template<typename... T>
			requires (sizeof...(T) > 0)
		void set(const T&... args) {
			using FirstType = std::tuple_element_t<0, std::tuple<T...>>;
			std::vector<FirstType> vec{ args... };

			set_impl(vec);
		}*/

		template<typename... Args>
		void set(Args&&... args) {
			size_t idx = 0;

			// Вместо operator[](idx++) мы берем entries СТРОГО по порядку вычисления пакета
			// Но так как базовый operator[] может косячить, давай завяжемся на явный индекс.
			int dummy[] = { 0, (this->entries[idx++].set_impl(std::forward<Args>(args)), 0)... };
			(void)dummy;
		}

		template<typename T>
		void set_impl(std::initializer_list<T> list) {
			auto buffer = buffer_ref.lock();
			if (!buffer) {
				throw std::runtime_error("UBO is dead");
			}

			if (list.size() > this->entries.size()) {
				throw std::runtime_error("Passed initializer list size exceeds uniform block array size on GPU");
			}

			size_t idx = 0;
			for (const auto& value : list) {
				size_t target_offset = idx * sizeof(T);
				bool block_found = false;

				// Тупо бежим по вектору через обычный индекс
				for (size_t i = 0; i < this->entries.size(); ++i) {
					// Берем напрямую элемент вектора. Никаких разыменований итераторов.
					if (static_cast<size_t>(this->entries[i].composit.offset) == target_offset) {
						this->entries[i].set_impl(value);
						block_found = true;
						break;
					}
				}

				if (!block_found) {
					throw std::runtime_error("Could not find uniform block with correct offset for index " + std::to_string(idx));
				}

				idx++;
			}
		}

		// Системный сеттер, который вызывается в том числе через operator=
		template<ag::concepts::Container T>
		void set_impl(const T& value) {
			auto buffer = buffer_ref.lock();
			if (!buffer) {
				throw std::runtime_error("UBO is dead");
			}

			// Проверяем, что нам передали не больше элементов, чем размер массива в шейдере
			if (value.size() > this->entries.size()) {
				throw std::runtime_error("Passed container size exceeds uniform block array size on GPU");
			}

			// Просто пробегаемся по переданным структурам и скармливаем их
			// соответствующим инстансам блоков (uniform_block_data) из entries!
			size_t index = 0;
			for (const auto& element : value) {
				this->entries[index].set_impl(element);
				index++;
			}
		}

		// Геттер: выкачать весь массив блоков в C++ контейнер структур
		template<ag::concepts::Container T>
		T get() const {
			auto buffer = buffer_ref.lock();
			if (!buffer) {
				throw std::runtime_error("UBO is dead");
			}

			T container;
			// Ресайзим контейнер под размер массива из рефлектора
			container.resize(this->entries.size());

			// Вытаскиваем тип структуры (например, CameraData)
			using ElementType = typename T::value_type;

			size_t index = 0;
			for (auto& element : container) {
				// Вызываем геттер конкретного блока и кладем в контейнер
				element = this->entries[index].template get<ElementType>();
				index++;
			}

			return container;
		}

		friend std::ostream& operator<<(std::ostream& os, const uniform_block_sequence& composit) {
			os << composit.composit;
			return os;
		}
	};
}