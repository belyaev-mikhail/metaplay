/*
 * type_list.hpp
 *
 *  Created on: Dec 17, 2011
 *      Author: kopcap
 */

#ifndef TYPE_LIST_HPP_
#define TYPE_LIST_HPP_

#include "metadef.hh"

OF(TPARAM(...Elems)) DEF(type_list);
OF(TPARAM(Head), TPARAM(...Tail)) DEF(type_list) <Head, Tail...> {
    TLET(head, Head)
    TLET(tail, type_list<Tail...>)
};
OF() DEF(type_list) <> {};

TLET(nil, type_list<>)

#define LIST(...) type_list<__VA_ARGS__>
#define HEAD(...) GET(head, __VA_ARGS__)
#define TAIL(...) GET(tail, __VA_ARGS__)

OF(TPARAM(T)) DEF(meta_iid) { ISAMEAS(T) TVAL(T) };

// cons operation
OF(TPARAM(H), TPARAM(T)) DEF(tl_cons);
// cons H [Rest...] = [H, Rest...]
OF(TPARAM(H), TPARAM(...Rest)) DEF(tl_cons) <H, LIST(Rest...)> {
	TRETURN(LIST(H, Rest...))
};

OF(TPARAM(TList)) DEF(tl_empty);
OF(TPARAM(...Elements)) DEF(tl_empty) <LIST(Elements...)> { IRETURN(false) };
OF() DEF(tl_empty) <LIST()> { IRETURN(true) };

// list length
OF(TPARAM(TList))    DEF(tl_length);
// length [Elems...] = sizeof...(Elems)
OF(TPARAM(...Elems)) DEF(tl_length) <LIST(Elems...)> { IVAL(sizeof...(Elems)) };

#define LENGTH(...) IAPP(tl_length, __VA_ARGS__)

// mapping lists
OF(TPARAM(TList), OF(TPARAM())TPARAM(Func)) DEF(tl_map);
// map [Elems...] f = [f(Elems)...]
OF(OF(TPARAM())TPARAM(Func), TPARAM(...Elems)) DEF(tl_map) <LIST(Elems...), Func> {
    TRETURN(LIST(EVAL(Func<Elems>)...))
};
// get element of a list
OF(TPARAM(TList), IPARAM(index)) DEF(tl_get);
// get [Head, _...] 0 = Head
OF(TPARAM(Head), TPARAM(...Elems)) DEF(tl_get) <LIST(Head, Elems...), 0U> { TRETURN(Head) };
// get lst Index =
//                 let rest = tail lst in
//                 get rest (Index - 1)
OF(IPARAM(Index), TPARAM(...Elems)) DEF(tl_get) <LIST(Elems...), Index> {
    static_assert(LENGTH(LIST(Elems...)) > Index ,"Indexing type list overflow");

    TLET(rest, TAIL(LIST(Elems...)))
    TSAMEAS(tl_get<rest, Index-1>)
};
// filter list by predicate
OF(TPARAM(TList), OF(TPARAM())TPARAM(Pred) = meta_iid) DEF(tl_filter);
// filter [] _ = []
OF(OF(TPARAM())TPARAM(Pred)) DEF(tl_filter) <LIST(), Pred> { TRETURN(LIST()) };
// filter [Head,Elems...] Pred =
//                            let rest = filter [Elems...} Pred
//                            in if (Pred Head) then Head:rest
//                                              else
OF(
	OF(TPARAM())TPARAM(Pred),
	TPARAM(Head),
	TPARAM(...Elems)
) DEF(tl_filter) <LIST(Head, Elems...), Pred> {
    TLET(rest, TAPP(tl_filter, LIST(Elems...), Pred))
    TRETURN(COND(IEVAL(Pred<Head>), EVAL(tl_cons<Head,rest>), rest))
};

// all func
template<class TList, template<class>class Pred = meta_iid>
struct tl_all;

template<template<class>class Pred>
struct tl_all<LIST(), Pred> { IVAL(true) };

template<template<class>class Pred, class Head, class ...TElems>
struct tl_all<LIST(Head,TElems...), Pred>{
	IVAL( IEVAL(Pred<Head>) && IEVAL(tl_all<LIST(TElems...), Pred>))
};

// any func
template<class TList, template<class>class Pred = meta_iid>
struct tl_any;

template<template<class>class Pred>
struct tl_any<LIST(), Pred> { IVAL(true) };

template<template<class>class Pred, class Head, class ...TElems>
struct tl_any<LIST(Head,TElems...), Pred>{
	IVAL( IEVAL(Pred<Head>) || IEVAL(tl_any<LIST(TElems...), Pred>))
};


// foldl functions
template<class TList, template<class,class>class Bifun, class Elem>
struct tl_fold;
// fold [] _ Elem = Elem
template<template<class,class>class Bifun, class Elem>
struct tl_fold<LIST(), Bifun, Elem> {
    TVAL(Elem)
};
// fold [Elems...] Bifun Elem =
//							let lst = [Elems...]
//								newElem = Bifun Elem (head lst)
//							in fold (tail lst) Bifun newElem
template<template<class,class>class Bifun, class Elem, class ...Elems>
struct tl_fold<LIST(Elems...), Bifun, Elem> {

    TLET(lst, LIST(Elems...))
    TLET(newElem, EVAL(Bifun<Elem,HEAD(lst)>))

    TSAMEAS(tl_fold<TAIL(lst), Bifun, newElem>)
};
// reduce func
template<class TList, template<class,class>class Bifun>
struct tl_reduce;
// reduce [Head,TElems...] Bifun = fold [TElems...] Bifun Head
template<template<class,class>class Bifun, class Head, class ...TElems>
struct tl_reduce<LIST(Head,TElems...), Bifun>{
    TSAMEAS(tl_fold<LIST(TElems...), Bifun, Head>)
};
// contains func
template<class TList, class Elem>
struct tl_contains;
// contains [] _ = false
template<class Elem>
struct tl_contains<LIST(), Elem> { IVAL(false) };
// contains [Head,_...] Head = true
template<class Head, class ...TElems>
struct tl_contains<LIST(Head, TElems...), Head>{ IVAL(true) };
// contains [_, TElems...] Elem = contains [TElems...] Elem
template<class Elem, class Head, class ...TElems>
struct tl_contains<LIST(Head, TElems...), Elem>{
    ISAMEAS(tl_contains<LIST(TElems...), Elem>)
};

template<class NoTypeList>
struct is_type_list {
	IVAL(false)
};
template<class ...Elems>
struct is_type_list<LIST(Elems...)>{
	IVAL(true)
};

// append
template<class ...TLists>
struct tl_append;
// append <> = []
template<>
struct tl_append<> {
	TVAL(LIST())
};
// append lst = lst
template<class ...Elems>
struct tl_append<LIST(Elems...)> {
	TVAL(LIST(Elems...))
};
// append [] lst = lst
template<class ...Elems>
struct tl_append<LIST(),LIST(Elems...)> {
	TVAL(LIST(Elems...))
};
// append [] lst = lst
template<class ...Elems>
struct tl_append<LIST(Elems...),LIST()> {
	TVAL(LIST(Elems...))
};
// append lst0 lst1 = (head lst0) : append (tail lst0) lst1
template<class TList0, class TList1>
struct tl_append<TList0, TList1> {
	static_assert(
			IEVAL(is_type_list<TList0>) && IEVAL(is_type_list<TList1>),
			"One of arguments to tl_append is not a type list"
	);

	TSAMEAS(tl_cons<HEAD(TList0),EVAL(tl_append<TAIL(TList0),TList1>)>)
};
// append lst0 lst1 lsts... = let pair = append lst0 lst1 in
//							  append pair lsts...
template<class L1, class L2, class ...TLists>
struct tl_append<L1, L2, TLists...> {
	static_assert(
			IEVAL(is_type_list<L1>) && IEVAL(is_type_list<L2>),
			"One of arguments to tl_append is not a type list"
	);

	TLET(pair,EVAL(tl_append<L1,L2>))

	TSAMEAS(tl_append<pair,TLists...>)
};

// flatten
// [[a,b,c],[d,e]] -> [a,b,c,d,e]
template<class TListList>
struct tl_flatten;

template<class ...TLists>
struct tl_flatten<LIST(TLists...)> {
	TSAMEAS(tl_append<TLists...>)
};


// sort
template<class TList, template<class,class>class Compare>
struct tl_sort;
// sort [] _ = []
template<template<class,class>class Compare>
struct tl_sort<LIST(), Compare>{
	TVAL(LIST())
};
// sort [Head, Rest...] Compare = let small = filter (< Head) [Rest...]
//								      mid = [Head]
//                                    large = filter (>= Head) [Rest...]
//                                in append (sort small) mid (sort large)
template<template<class,class>class Compare, class Head, class ...TElems>
struct tl_sort<LIST(Head,TElems...), Compare>{
	template<class T>
	struct less { ISAMEAS(Compare<T, Head>)	};
	template<class T>
	struct more { IVAL(!IEVAL(less<T>))	};

	TLET(lst, LIST(TElems...))
	TLET(mid, LIST(Head))
	TLET(small, EVAL(tl_filter<lst, less>))
	TLET(large, EVAL(tl_filter<lst, more>))

	TSAMEAS(tl_append<EVAL(tl_sort<small, Compare>), mid, EVAL(tl_sort<large, Compare>)>)
};

// a lousy version of find
// find lst pred = head $ filter lst pred
template<class TList, template<class>class Pred>
struct tl_find {
	static_assert(IEVAL(is_type_list<TList>),"tl_find used with a no-list type");

	TLET(flt, EVAL(tl_filter<TList, Pred>))// HEAD(EVAL(...)) causes double typename issue

	TVAL(HEAD(flt))
};
// a lousy version of remove_all
// remove_all lst elem = filter lst (!= elem)
template<class TList, class Elem>
struct tl_remove_all {
	static_assert(IEVAL(is_type_list<TList>),"tl_remove_all used with a no-list type");

	template<class T>
	struct notElem { IVAL(! IEVAL(std::is_same<T,Elem>)) };

	TSAMEAS(tl_filter<TList,notElem>)
};

// remove_dups h:t = h : remove_dups (remove_all t h)
template<class TList>
struct tl_remove_dups;

template<>
struct tl_remove_dups<LIST()>{
	TVAL(LIST())
};

template<class Head, class ...TElems>
struct tl_remove_dups<LIST(Head,TElems...)>{
	TLET(tail_no_head, EVAL(tl_remove_all<LIST(TElems...), Head>))
	TSAMEAS(tl_cons<Head, EVAL(tl_remove_dups<tail_no_head>)>)
};


template<class TList, class Elem>
struct tl_index_of {
	static_assert(IEVAL(is_type_list<TList>), "tl_index_of used with no list");
	static_assert(IEVAL(tl_contains<TList, Elem>), "tl_index_of used with an element not in the list");

	IVAL(IEVAL(tl_index_of<TAIL(TList), Elem>) + 1)
};

template<class Head, class ...Rest>
struct tl_index_of<LIST(Head,Rest...), Head> { IVAL(0U) };

#include "metaundef.hh"

#undef LIST
#undef HEAD
#undef TAIL


#endif /* TYPE_LIST_HPP_ */
