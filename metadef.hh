
#ifndef METAMACROS_DEFINED
#define METAMACROS_DEFINED

#define TPARAM(...) class __VA_ARGS__
#define IPARAM(...) unsigned long long __VA_ARGS__
#define CHPARAM(...) char __VA_ARGS__
#define VARTPARAM(...) class... __VA_ARGS__
#define VARIPARAM(...) unsigned long long... __VA_ARGS__

#define OF(...) template<__VA_ARGS__>
#define DEF(...) struct __VA_ARGS__
#define CASE(...) <__VA_ARGS__>

#define GET(WHAT, ...) typename __VA_ARGS__::WHAT
#define IGET(WHAT, ...) __VA_ARGS__::WHAT

#define EVAL(...) GET(type, __VA_ARGS__)
#define IEVAL(...) IGET(value, __VA_ARGS__)

#define TAPP(WHAT, ...) EVAL(WHAT CASE(__VA_ARGS__))
#define IAPP(WHAT, ...) IEVAL(WHAT CASE(__VA_ARGS__))

#define TLET(NAME, ...) typedef __VA_ARGS__ NAME;
#define ILET(NAME, ...) enum{ NAME = __VA_ARGS__ };

#define TVAL(...) TLET(type, __VA_ARGS__)
#define IVAL(...) ILET(value, __VA_ARGS__)

#define TRETURN(...) TLET(type, __VA_ARGS__)
#define IRETURN(...) ILET(value, __VA_ARGS__)

#define COND(...) EVAL(std::conditional<__VA_ARGS__>)

#define TSAMEAS(...) TVAL(EVAL(__VA_ARGS__))
#define ISAMEAS(...) IVAL(IEVAL(__VA_ARGS__))


// lambdas. used as:
// TLAMBDA0(is_even, T,
#define TLAMBDA0(NAME, PNAME, ...) \
	OF(TPARAM(PNAME)) \
	DEF(NAME) { \
		TSAMEAS(__VA_ARGS__) \
	};

#define ILAMBDA0(NAME, PNAME, ...) \
	OF(TPARAM(PNAME)) \
	DEF(NAME) { \
		ISAMEAS(__VA_ARGS__) \
	};

#endif //METAMACROS_DEFINED
