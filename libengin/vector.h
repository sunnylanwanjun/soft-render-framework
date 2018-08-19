#ifndef __VECTOR_H
#define __VECTOR_H
#include "macros.h"
#include <vector>
NS_ENGIN_BEGIN
template<class T>
class vector{
private:
	std::vector<T> _data;
};
NS_ENGIN_END
#endif
