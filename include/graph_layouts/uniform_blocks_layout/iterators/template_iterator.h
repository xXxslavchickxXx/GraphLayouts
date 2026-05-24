#pragma once

#include <uniform/uniform_buffer.hpp>
#include <AGreflection.h>

#include <string>
#include <unordered_map>

namespace ag::iterators {
	template<typename Derived, typename Composition>
	class template_uniform_setter {
	protected:
		Composition composit;
		std::weak_ptr<ag::uniform_buffer> buffer_ref;
	
	public:
		template_uniform_setter() = default;
		template_uniform_setter(std::shared_ptr<ag::uniform_buffer> buffer_ptr, const Composition& composit)
		: buffer_ref(buffer_ptr), composit(composit) {}



		template_uniform_setter(template_uniform_setter&&) = default;
		template_uniform_setter& operator=(template_uniform_setter&&) = default;
		template_uniform_setter(const template_uniform_setter&) = default;
		template_uniform_setter& operator=(const template_uniform_setter&) = default;

		template<typename T>
		void operator=(const T& value) {
			static_cast<Derived*>(this)->set_impl(value);
		}

		template<typename T>
		operator T() const {
			return static_cast<Derived*>(this)->get<T>();
		}

		Composition& get_raw() { return composit; }

		friend std::ostream& operator<<(std::ostream& os, const template_uniform_setter<Derived, Composition>& composit) {
			os << composit.composit;
			return os;
		}
	};
}