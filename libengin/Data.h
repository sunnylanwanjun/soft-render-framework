#ifndef __DATA_H
#define __DATA_H
#include "macros.h"
#include "Ref.h"
NS_ENGIN_BEGIN
class ENGIN_DLL Data:public Ref{
public:
	Data();
	Data(Data& data);
	virtual ~Data();
	void fastSet(unsigned char* bytes, const size_t size);
	bool isNull();
	void clear();
	BYTE* getBytes(){ return _bytes; }
	size_t getSize(){ return _size; }
private:
	BYTE* _bytes;
	size_t _size;
};
NS_ENGIN_END
#endif

