/*
 * cpstrings.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: belyaev
 */

#include <type_traits>
#include <iostream>

#include "demangle.hpp"

#include "type_list.hpp"

#include "metadef.hh"

template<char ...Args> struct static_string {
	enum{ length = sizeof...(Args) };

	friend std::ostream& operator<<(std::ostream& ost, static_string<Args...> str) {
		const char output[]{ Args..., '\0' };
		return ost << output;
	}
};

template<class SS, char c> struct ss_contains;
template<char c, char ...t> struct ss_contains<static_string<c, t...>, c> {
	enum{ value = true };	
};
template<char c, char h, char ...t> struct ss_contains<static_string<h, t...>, c> {
	enum{ value = ss_contains<static_string<t...>, c>::value };	
};

template<size_t Sz>
constexpr char at(char const(&s)[Sz], size_t i) {
	return i >= Sz ? '\0' : s[i];
}

template<class SS, char wha>
struct append_char;

template<char... Str>
struct append_char<static_string<Str...>, '\0'> {
	typedef static_string<Str...> type;
};

template<char wha, char... Str>
struct append_char<static_string<Str...>, wha> {
	typedef static_string<Str..., wha> type;
};

template<class SS, char wha>
using append_char_q = typename append_char<SS, wha>::type;

template<class SS, char... wha>
struct append_chars;

template<class SS>
struct append_chars<SS> {
	typedef SS type;
};

template<class SS, char Head, char ...Tail>
struct append_chars<SS, Head, Tail...> {
	typedef typename append_char<SS, Head>::type headed;
	typedef typename append_chars<headed, Tail...>::type type;
};

template<class SS, char... wha>
using append_chars_q = typename append_chars<SS, wha...>::type;

template<char... wha>
using make_ss = typename append_chars<static_string<>, wha...>::type;

#define STATIC_STRING(S) \
	make_ss< \
	at(S, 0), at(S, 1), at(S, 2), at(S, 3), at(S, 4), at(S, 5), at(S, 6), at(S, 7), at(S, 8), at(S, 9), \
	at(S,10), at(S,11), at(S,12), at(S,13), at(S,14), at(S,15), at(S,16), at(S,17), at(S,18), at(S,19), \
	at(S,20), at(S,21), at(S,22), at(S,23), at(S,24), at(S,25), at(S,26), at(S,27), at(S,28), at(S,29), \
	at(S,30), at(S,31), at(S,32), at(S,33), at(S,34), at(S,35), at(S,36), at(S,37), at(S,38), at(S,39), \
	at(S,40), at(S,41), at(S,42), at(S,43), at(S,44), at(S,45), at(S,46), at(S,47), at(S,48), at(S,49), \
	at(S,50), at(S,51), at(S,52), at(S,53), at(S,54), at(S,55), at(S,56), at(S,57), at(S,58), at(S,59), \
	at(S,60), at(S,61), at(S,62), at(S,63), at(S,64), at(S,65), at(S,66), at(S,67), at(S,68), at(S,69), \
	at(S,70), at(S,71), at(S,72), at(S,73), at(S,74), at(S,75), at(S,76), at(S,77), at(S,78), at(S,79) \
	>

#define KEYWORD(...) STATIC_STRING(#__VA_ARGS__)
#define TSTRING(...) static_string<__VA_ARGS__>

OF(TPARAM(RestSS), IPARAM(Col = 0), IPARAM(LINE = 0)) DEF(parser_state) {};

OF(TPARAM(PS)) DEF(read_char);
OF(IPARAM(Col), IPARAM(Line), CHPARAM(Head), CHPARAM(...Seq)) DEF(read_char)
		<parser_state<TSTRING(Head, Seq...), Col, Line>> {
	IRETURN(Head); TRETURN(parser_state<TSTRING(Seq...), Head == '\n'? 0:Col+1, Head == '\n'? Line+1:Line >)
};
OF(IPARAM(Col), IPARAM(Line)) DEF(read_char)
		<parser_state<TSTRING(), Col, Line>> {
	IRETURN(-1); TRETURN(parser_state<TSTRING(), Col, Line>)
};
OF(TPARAM(PS)) DEF(peek_char);
OF(IPARAM(Col), IPARAM(Line), CHPARAM(Head), CHPARAM(...Seq)) DEF(peek_char)
		<parser_state<TSTRING(Head, Seq...), Col, Line>> {
	IRETURN(Head); TRETURN(parser_state<TSTRING(Head, Seq...), Col, Line >)
};
OF(IPARAM(Col), IPARAM(Line)) DEF(peek_char)
		<parser_state<TSTRING(), Col, Line>> {
	IRETURN(-1); TRETURN(parser_state<TSTRING(), Col, Line>)
};


OF(TPARAM(Message), TPARAM(PState)) DEF(error) {};
OF(TPARAM(Res)) DEF(is_error) { IRETURN(false) };
OF(TPARAM(Message), TPARAM(PState)) DEF(is_error) <error<Message, PState>> { IRETURN(true) };
OF(TPARAM(PState)) DEF(anychar) {
	TLET(reader, read_char<PState>)
	ILET(acquired, IEVAL(reader))
	TLET(state, EVAL(reader))
	TLET(result, COND(acquired != -1, TSTRING(IEVAL(reader)), error<STATIC_STRING("parse failed"), PState>))
};
OF(CHPARAM(theChar)) DEF(const_char) {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(reader, read_char<PState>)
		ILET(acquired, IEVAL(reader))
		TLET(state, COND(acquired == theChar, EVAL(reader), PState))
		TLET(result, COND(acquired == theChar, TSTRING(acquired), error<STATIC_STRING("parse failed"), PState>))
	};
};

OF(CHPARAM(from), CHPARAM(to)) DEF(char_range) {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(reader, read_char<PState>)
		ILET(acquired, IEVAL(reader))
		TLET(state, COND(acquired <= from && acquired >= to, EVAL(reader), PState))
		TLET(result, COND(acquired <= from && acquired >= to, TSTRING(acquired), error<STATIC_STRING("parse failed"), PState>))
	};
};
OF(TPARAM(SS)) DEF(one_of) {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(reader, read_char<PState>)
		ILET(acquired, IEVAL(reader))
		TLET(state, COND(IEVAL(ss_contains<SS, acquired>), EVAL(reader), PState))
		TLET(result, COND(IEVAL(ss_contains<SS, acquired>), TSTRING(acquired), error<STATIC_STRING("parse failed"), PState>))
	};
};

////////////////////////////////////////////////seq

OF(OF(TPARAM())TPARAM(...Args)) DEF(seq);
OF() DEF(seq) <> {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(state, PState)
		TLET(result, nil)
	};
};

OF(IPARAM(worked), TPARAM(Elem), TPARAM(PState), OF(TPARAM())TPARAM(...T)) DEF(seq_step2);
OF(TPARAM(PState), TPARAM(Elem), OF(TPARAM())TPARAM(...T)) DEF(seq_step2) <false, Elem, PState, T...> {
	TLET(evo, typename seq<T...>::template inner<typename Elem::state>)
	TLET(state, typename evo::state)
	TLET(result, TAPP(tl_cons, typename Elem::result, typename evo::result))
};
OF(TPARAM(PState), TPARAM(Elem), OF(TPARAM())TPARAM(...T)) DEF(seq_step2) <true, Elem, PState, T...> {
	TLET(state, PState)
	TLET(result, typename Elem::result)
};

OF(OF(TPARAM())TPARAM(H), OF(TPARAM())TPARAM(...T)) DEF(seq) <H, T...> {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(head, H<PState>)
		TLET(step2, seq_step2<IEVAL(is_error<typename head::result>), head, PState, T...>)
		TLET(state, typename step2::state)
		TLET(result, typename step2::result)
	};
};

////////////////////////////////////////////////splus

OF(OF(TPARAM())TPARAM(Arg)) DEF(splus);
OF(IPARAM(worked), TPARAM(Elem), TPARAM(PState), OF(TPARAM())TPARAM(Arg)) DEF(splus_step2);
OF(TPARAM(PState), TPARAM(Elem), OF(TPARAM())TPARAM(Arg)) DEF(splus_step2) <false, Elem, PState, Arg> {
	TLET(evo, typename splus<Arg>::template inner<typename Elem::state>)
	TLET(state, typename evo::state)
	TLET(result, TAPP(tl_cons, typename Elem::result, typename evo::result))
};
OF(TPARAM(PState), TPARAM(Elem), OF(TPARAM())TPARAM(Arg)) DEF(splus_step2) <true, Elem, PState, Arg> {
	TLET(state, PState)
	TLET(result, nil)
};

OF(OF(TPARAM())TPARAM(Arg)) DEF(splus) {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(head, Arg<PState>)
		TLET(step2, splus_step2<IEVAL(is_error<typename head::result>), head, PState, Arg>)
		TLET(state, typename step2::state)
		TLET(result, typename step2::result)
	};
};


////////////////////////////////////////////////sor

OF(OF(TPARAM())TPARAM(...Args)) DEF(sor);
OF() DEF(sor) <> {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(state, PState)
		TLET(result, nil)
	};
};

OF(IPARAM(worked), TPARAM(Elem), TPARAM(PState), OF(TPARAM())TPARAM(...T)) DEF(sor_step2);
OF(TPARAM(PState), TPARAM(Elem), OF(TPARAM())TPARAM(...T)) DEF(sor_step2) <true, Elem, PState, T...> {
	TLET(evo, typename sor<T...>::template inner<PState>)
	TLET(state, typename evo::state)
	TLET(result, typename evo::result)
};
OF(TPARAM(PState), TPARAM(Elem), OF(TPARAM())TPARAM(...T)) DEF(sor_step2) <false, Elem, PState, T...> {
	TLET(state, typename Elem::state)
	TLET(result, typename Elem::result)
};

OF(OF(TPARAM())TPARAM(H), OF(TPARAM())TPARAM(...T)) DEF(sor) <H, T...> {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(head, H<PState>)
		TLET(step2, sor_step2<IEVAL(is_error<typename head::result>), head, PState, T...>)
		TLET(state, typename step2::state)
		TLET(result, typename step2::result)
	};
};

////////////////////////////////////////////////optional
struct none {};

OF(IPARAM(togo), TPARAM(Elem), TPARAM(PState)) DEF(optional_step2);

OF(TPARAM(PState), TPARAM(Elem)) DEF(optional_step2) <true, Elem, PState> {
	TLET(state, PState)
	TLET(result, none)
};
OF(TPARAM(PState), TPARAM(Elem)) DEF(optional_step2) <false, Elem, PState> {
	TLET(state, typename Elem::state)
	TLET(result, typename Elem::result)
};
OF(OF(TPARAM())TPARAM(Delegate)) DEF(optional) {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(head, Delegate<PState>)
		TLET(step2, optional_step2<IEVAL(is_error<typename head::result>), head, PState>)
		TLET(state, typename step2::state)
		TLET(result, typename step2::result)
	};
};

////////////////////////////////////////////////snot
OF(IPARAM(togo), TPARAM(Elem), TPARAM(PState)) DEF(snot_step2);

OF(TPARAM(PState), TPARAM(Elem)) DEF(snot_step2) <true, Elem, PState> {
	TLET(state, PState)
	TLET(result, none)
};
OF(TPARAM(PState), TPARAM(Elem)) DEF(snot_step2) <false, Elem, PState> {
	TLET(state, PState)
	TLET(result, error<STATIC_STRING("not failed!"), PState>)
};
OF(OF(TPARAM())TPARAM(Delegate)) DEF(snot) {
	OF(TPARAM(PState)) DEF(inner) {
		TLET(head, Delegate<PState>)
		TLET(step2, snot_step2<IEVAL(is_error<typename head::result>), head, PState>)
		TLET(state, typename step2::state)
		TLET(result, typename step2::result)
	};
};

int main() {
	using namespace std;
	cerr << static_string<'H', 'e', 'l', 'l', 'o'>() << endl;
	append_char_q<static_string<'F', 'o'>, 'o'> foo;
	append_chars_q<static_string<'F', 'o'>, '\0','o'> foo2;
	cerr << foo2 << endl;

	STATIC_STRING("generated") generated;
	cerr << name(generated) << endl;
	cerr << generated << endl;
	cerr << generated.length << endl;

	cerr << KEYWORD(int main(int argc, char* argv[]))() << endl;

	typedef parser_state<STATIC_STRING("AA*")> initial_parser_state;
	typedef typename splus<const_char<'A'>::inner>::inner<initial_parser_state>::result seqres;

	cerr << name<seqres>() << endl;

}
