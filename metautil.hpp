/*
 * metautil.hpp
 *
 *  Created on: Mar 23, 2012
 *      Author: belyaev
 */

#ifndef METAUTIL_HPP_
#define METAUTIL_HPP_

#include "metadef.hh"

template<class T>
struct type2type {
	TVAL(T)
};

template<typename V>
struct gen_value2type{
	template<V val>
	struct type{
		IVAL(val)
	};
};

template<int val>
struct int2type{
	ISAMEAS(gen_value2type<int>::type<val>)
};

template<char val>
struct char2type{
	ISAMEAS(gen_value2type<char>::type<val>)
};

template<long long val>
struct longlong2type{
	ISAMEAS(gen_value2type<long long>::type<val>)
};

template<size_t val>
struct size2type{
	ISAMEAS(gen_value2type<size_t>::type<val>)
};

template<class MetaVal>
struct m_not{
	IVAL(!IEVAL(MetaVal))
};

template<class ...MetaVals>
struct m_and{};

template<class Head, class ...Tail>
struct m_and<Head, Tail...>{
	IVAL(IEVAL(Head) && IEVAL(m_and<Tail...>))
};

template<>
struct m_and<>{
	IVAL(true)
};

template<class ...MetaVals>
struct m_or{};

template<class Head, class ...Tail>
struct m_or<Head, Tail...>{
	IVAL(IEVAL(Head) || IEVAL(m_or<Tail...>))
};

template<>
struct m_or<>{
	IVAL(false)
};



#include "metaundef.hh"

#endif /* METAUTIL_HPP_ */
