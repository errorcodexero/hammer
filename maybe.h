#ifndef MAYBE_H
#define MAYBE_H

#include<cassert>

template<typename T>
class Maybe{
	T *t;

	public:
	Maybe():t(NULL){}
	explicit Maybe(T const&);
	Maybe& operator=(Maybe const&);

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
