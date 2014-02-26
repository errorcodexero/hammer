#ifndef MAYBE_H
#define MAYBE_H

#include<cassert>
#include<iostream>

template<typename T>
class Maybe{
	T *t;

	public:
	Maybe():t(NULL){}

	explicit Maybe(T const& t1):t(new T(t1)){}

	Maybe& operator=(Maybe const&);

	~Maybe(){
		delete t;
		t=NULL;
	}

	operator bool()const{ return !!t; }

	T& operator*(){
		assert(t);
		return *t;
	}

	T const& operator*()const{
		assert(t);
		return *t;
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& o,Maybe<T> const& m){
	if(m){
		return o<<*m;
	}else{
		return o<<"NULL";
	}
}

#endif
