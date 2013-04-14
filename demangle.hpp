/*
 * demangle.hpp
 *
 *  Created on: Oct 13, 2012
 *      Author: belyaev
 */

#ifndef DEMANGLE_HPP_
#define DEMANGLE_HPP_

#include <cxxabi.h>
#include <string>
#include <memory>

template<typename _Tp>
struct free_based_delete
{
	constexpr free_based_delete() noexcept = default;

	template<typename _Up, typename = typename
			std::enable_if<std::is_convertible<_Up*, _Tp*>::value>::type>
	free_based_delete(const free_based_delete<_Up>&) noexcept { }

	void
	operator()(_Tp* __ptr) const
	{
		static_assert(sizeof(_Tp)>0,
				"can't delete pointer to incomplete type");
		free((void*)__ptr);
	}
};



inline std::string demangle_impl( const char * const mangled )
{
	if ( !mangled )return "(null)";
	else if ( !mangled[ 0 ] ) return "(empty)";

	int status = -1;
	const std::unique_ptr< const char, free_based_delete< const char > > demangled( abi::__cxa_demangle( mangled, 0, 0, & status ) );


	if ( ! demangled.get() ) {
		return mangled;
	}
	else if ( 0 == status ) {
		return std::string( demangled.get() );
	}
	return mangled;
}

template<class T>
std::string name() {
	return demangle_impl(typeid(T).name());
}

template<class T>
std::string name(const T& v) {
	return demangle_impl(typeid(v).name());
}

template<class T>
std::string static_name(const T&) {
	return name<T>();
}

#endif /* DEMANGLE_HPP_ */
