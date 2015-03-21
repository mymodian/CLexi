#ifndef CHROMIUM_TASK_H
#define CHROMIUM_TASK_H

//#include <boost/config.hpp>
//#include <iostream>
//#include <vector>
//#include <string>
//#include <boost\pool\pool.hpp>
//#include <time.h>
//#include <string>
//#include <boost\smart_ptr\scoped_ptr.hpp>
//#include <boost\lexical_cast.hpp>
//#include <boost\regex.hpp>
//#include <boost\smart_ptr\shared_ptr.hpp>
//#include <boost\smart_ptr\weak_ptr.hpp>
//#include <boost\function.hpp>
//#include <boost\bind.hpp>

template<typename T>
struct TupleTraits
{
	typedef T ParamType;
};

struct Tuple0 {};

template<class A>
struct Tuple1
{
	Tuple1() {}
	explicit Tuple1(typename TupleTraits<A>::ParamType a) : a(a) {}
	A a;
};

template<class A,class B>
struct Tuple2
{
	Tuple2() {}
	explicit Tuple2(typename TupleTraits<A>::ParamType a,typename TupleTraits<B>::ParamType b) : a(a),b(b) {}
	A a;
	B b;
};

template<class A, class B, class C>
struct Tuple3
{
	Tuple3() {}
	explicit Tuple3(typename TupleTraits<A>::ParamType a, typename TupleTraits<B>::ParamType b, 
		typename TupleTraits<C>::ParamType c) : a(a), b(b), c(c) {}
	A a;
	B b;
	C c;
};

template<class A, class B, class C, class D>
struct Tuple4
{
	Tuple4() {}
	explicit Tuple4(typename TupleTraits<A>::ParamType a, typename TupleTraits<B>::ParamType b,
		typename TupleTraits<C>::ParamType c, typename TupleTraits<D>::ParamType d) : a(a), b(b), c(c), d(d) {}
	A a;
	B b;
	C c;
	D d;
};

template<class A, class B, class C, class D, class E>
struct Tuple5
{
	Tuple5() {}
	explicit Tuple5(typename TupleTraits<A>::ParamType a, typename TupleTraits<B>::ParamType b,
		typename TupleTraits<C>::ParamType c, typename TupleTraits<D>::ParamType d, 
		typename TupleTraits<E>::ParamType e) : a(a), b(b), c(c), d(d), e(e) {}
	A a;
	B b;
	C c;
	D d;
	E e;
};

Tuple0 MakeTuple();

template<typename A>
Tuple1<A> MakeTuple(const A& a)
{
	return Tuple1<A>(a);
}

template<typename A,typename B>
Tuple2<A,B> MakeTuple(const A& a,const B& b)
{
	return Tuple2<A,B>(a,b);
}

template<typename A, typename B, typename C>
Tuple3<A, B, C> MakeTuple(const A& a, const B& b, const C& c)
{
	return Tuple3<A, B, C>(a, b, c);
}

template<typename A, typename B, typename C, typename D>
Tuple4<A, B, C, D> MakeTuple(const A& a, const B& b, const C& c, const D& d)
{
	return Tuple4<A, B, C, D>(a, b, c, d);
}

template<typename A, typename B, typename C, typename D, typename E>
Tuple5<A, B, C, D, E> MakeTuple(const A& a, const B& b, const C& c, const D& d, const E& e)
{
	return Tuple5<A, B, C, D, E>(a, b, c, d, e);
}

template<class CommonPara>
class Task
{
public:
	virtual ~Task() {}
	virtual void run(CommonPara* common_para) = 0;
	virtual void cancel() =0;
};

template<class T, class CommonPara, class Method, class Params>
class RunnableMethod : public Task<CommonPara>
{
public:
	RunnableMethod(T* ptr,Method method,Params params) : obj_(ptr),
		meth_(method),
		param_(params)
	{
		/*std::cout <<"task_create\n";*/
	}
	virtual ~RunnableMethod() { /*std::cout <<"task_destroy\n";*/ }
	virtual void run(CommonPara* common_para) override
	{
		run_with_param(obj_, common_para, param_);
		/*if( pObj ) (pObj.get()->*meth_)(param_);
		else std::cout <<"has dead\n";*/
	}
	virtual void cancel() override
	{
		/*obj_.reset();*/
	}
protected:
	void run_with_param(T* pObj, CommonPara* common_para, Tuple0)
	{
		if (pObj) (pObj->*meth_)(common_para);
		else std::cout <<"has dead\n";
	}
	template<typename A>
	void run_with_param(T* pObj, CommonPara* common_para, Tuple1<A> param1)
	{
		if (pObj) (pObj->*meth_)(common_para, param1.a);
		else std::cout <<"has dead\n";
	}
	template<typename A,typename B>
	void run_with_param(T* pObj, CommonPara* common_para, Tuple2<A, B> param2)
	{
		if (pObj) (pObj->*meth_)(common_para, param2.a, param2.b);
		else std::cout <<"has dead\n";
	}
	template<typename A, typename B, typename C>
	void run_with_param(T* pObj, CommonPara* common_para, Tuple3<A, B, C> param3)
	{
		if (pObj) (pObj->*meth_)(common_para, param3.a, param3.b, param3.c);
		else std::cout << "has dead\n";
	}
	template<typename A, typename B, typename C, typename D>
	void run_with_param(T* pObj, CommonPara* common_para, Tuple4<A, B, C, D> param4)
	{
		if (pObj) (pObj->*meth_)(common_para, param4.a, param4.b, param4.c, param4.d);
		else std::cout << "has dead\n";
	}
	template<typename A, typename B, typename C, typename D, typename E>
	void run_with_param(T* pObj, CommonPara* common_para, Tuple5<A, B, C, D, E> param5)
	{
		if (pObj) (pObj->*meth_)(common_para, param5.a, param5.b, param5.c, param5.d, param5.e);
		else std::cout << "has dead\n";
	}
protected:
	RunnableMethod();
	RunnableMethod(const RunnableMethod&);
	RunnableMethod& operator=(const RunnableMethod&);
private:
	T* obj_;
	Method meth_;
	Params param_;
};

template<class T,class CommonPara, class Method>
Task<CommonPara>* NewRunnableMethod(T* ptr, CommonPara*, Method method)
{
	return new RunnableMethod<T, CommonPara, Method, Tuple0>(ptr, method, MakeTuple());
}

template<class T, class CommonPara, class Method, class Params1>
Task<CommonPara>* NewRunnableMethod(T* ptr, CommonPara*, Method method, Params1 params)
{
	return new RunnableMethod<T, CommonPara, Method, Tuple1<Params1>>(ptr, method, MakeTuple(params));
}

template<class T, class CommonPara, class Method, class Params1, class Params2>
Task<CommonPara>* NewRunnableMethod(T* ptr, CommonPara*, Method method, Params1 params1, Params2 params2)
{
	return new RunnableMethod<T, CommonPara, Method, Tuple2<Params1, Params2>>(ptr, method, MakeTuple(params1, params2));
}

template<class T, class CommonPara, class Method, class Params1, class Params2, class Params3>
Task<CommonPara>* NewRunnableMethod(T* ptr, CommonPara*, Method method, Params1 params1, Params2 params2, Params3 params3)
{
	return new RunnableMethod<T, CommonPara, Method, Tuple3<Params1, Params2, Params3>>(ptr, method, MakeTuple(params1, params2, params3));
}

template<class T, class CommonPara, class Method, class Params1, class Params2, class Params3, class Params4>
Task<CommonPara>* NewRunnableMethod(T* ptr, CommonPara*, Method method, Params1 params1, Params2 params2, Params3 params3, Params4 params4)
{
	return new RunnableMethod<T, CommonPara, Method, Tuple4<Params1, Params2, Params3, Params4>>(ptr, method, MakeTuple(params1, params2, params3, params4));
}

template<class T, class CommonPara, class Method, class Params1, class Params2, class Params3, class Params4, class Params5>
Task<CommonPara>* NewRunnableMethod(T* ptr, CommonPara*, Method method, Params1 params1, Params2 params2, Params3 params3, Params4 params4, Params5 params5)
{
	return new RunnableMethod<T, CommonPara, Method, Tuple5<Params1, Params2, Params3, Params4, Params5>>(ptr, method, MakeTuple(params1, params2, params3, params4, params5));
}

#endif