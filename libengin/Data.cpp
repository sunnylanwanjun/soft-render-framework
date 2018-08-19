#include "Data.h"
#include <memory.h>
NS_ENGIN_BEGIN
Data::Data():_bytes(nullptr),_size(0){

}

Data::Data(Data& data){
	if (data._size > 0){
		_bytes = new BYTE[data._size];
		memcpy(_bytes, data._bytes, data._size);
		_size = data._size;
	}
}

Data::~Data(){
	clear();
}

void Data::fastSet(unsigned char* bytes, const size_t size){
	clear();
	_bytes = bytes;
	_size = size;
}

bool Data::isNull(){
	return _size == 0;
}

void Data::clear(){
	if (_bytes){
		delete[] _bytes;
		_bytes = nullptr;
	}
}

NS_ENGIN_END