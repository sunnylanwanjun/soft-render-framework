#include "InitEngin.h"
#include <math.h>
NS_ENGIN_BEGIN
int log2Table[LOG2MAX];
void initLog2Table(){
	float log2=log(2.0f);
	for(size_t i=0;i<LOG2MAX;i++){
		log2Table[i]=int(log((float)i)/log2);
	}
}
NS_ENGIN_END