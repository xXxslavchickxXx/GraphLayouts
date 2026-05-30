#pragma once

#include <uniform/uniform_buffer.hpp>
#include <templates/template_iterator.h>
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
			return static_cast<const Derived*>(this)->get<T>();
		}

		Composition& get_raw() { return composit; }

		friend std::ostream& operator<<(std::ostream& os, const template_uniform_setter<Derived, Composition>& composit) {
			os << composit.composit;
			return os;
		}
	};

	/// <summary>
	/// UMTUS - Unordered Map Template Uniform Setter
	/// </summary>
	/// <typeparam name="Derived"></typeparam>
	/// <typeparam name="Entry"></typeparam>
	/// <typeparam name="Composition"></typeparam>
	template<typename Derived, typename Entry, typename Composition>
	class umtus_template :
		protected template_container_iterator<Derived, std::unordered_map<std::string, Entry>, const std::string&, Entry>,
		public template_uniform_setter<Derived, Composition>
	{
		using Base = umtus_template<Derived, Entry, Composition>;
		using Setter = template_uniform_setter<Derived, Composition>;
		using UM = template_container_iterator<Derived, std::unordered_map<std::string, Entry>, const std::string&, Entry>;

	protected:
		using Setter::composit;
		using Setter::buffer_ref;
		
	public:
		using UM::operator[];
		using Setter::get_raw;

		umtus_template(
			std::shared_ptr<ag::uniform_buffer> buffer_ptr,
			const Composition& composit,
			const std::string& name) : Setter(buffer_ptr, composit), UM(name) {}

		template<typename T>
		void operator=(const T& value) {
			Setter::operator=(value);  // вызываем оператор базового класса
		}

		template<typename T>
		operator T() const {
			return Setter::operator T();
		}

		friend std::ostream& operator<<(std::ostream& os, const Base& composit) {
			os << composit.composit;
			return os;
		}
	};

	/// <summary>
	/// OITUS - OpenGL Iterator Template Uniform Setter
	/// </summary>
	/// <typeparam name="Derived"></typeparam>
	/// <typeparam name="Entry"></typeparam>
	/// <typeparam name="Composition"></typeparam>
	template<typename Derived, typename Entry, typename Composition>
	class oitus_template :
		private opengl_template_iterator<Derived, Entry>,
		public template_uniform_setter<Derived, Composition>
	{
		using Base = oitus_template<Derived, Entry, Composition>;
		using Setter = template_uniform_setter<Derived, Composition>;
		using OTI = opengl_template_iterator<Derived, Entry>;
		using OTI::operator[];

	protected:
		using OTI::entries;
		using Setter::composit;
		using Setter::buffer_ref;

	public:
		using OTI::add_entry;
		using OTI::operator->;
		using Setter::get_raw;

		template<typename T>
		void operator=(const T& value) {
			Setter::operator=(value);  // вызываем оператор базового класса
		}

		oitus_template() = default;
		oitus_template(
		std::shared_ptr<ag::uniform_buffer> buffer_ptr,
		const Composition& composit,
		const std::string& name) : Setter(buffer_ptr, composit), OTI(name) {}

		friend std::ostream& operator<<(std::ostream& os, const Base& composit) {
			os << composit.composit;
			return os;
		}
	};
}