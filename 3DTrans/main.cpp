#define WIN32_LEAN_AND_MEAN 
#include "AppDelegate.h"
#include "dump.h"
#include <regex>
 
void testRegex();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	AllocConsole();
	FILE* fout=nullptr;
	FILE* fin=nullptr;
	freopen_s(&fout,"CONOUT$","w", stdout);
	freopen_s(&fin,"CONIN$", "r", stdin);

	InitMinDump();

	AppDelegate app;
	app.run();
	//testRegex();

	//FIXP16 f0=0x00000003;
	//FIXP16_Div(FIXP16_1,f0,f0);
	/*由于强转为整数后，小数部分没了，加0.5，四舍五入，使得结果更接近真实值
	int test=int(3.2456f*65536.0f);
	float test2=test/65536.0f;
	int test3=int(3.2456f*65536.0f+0.5);
	float test4=test3/65536.0f;

	FIXP16 t0=FLOAT_2_FIXP16(3.2456f);
	FIXP16 t1=FLOAT_2_FIXP16(-8.87825f);
	//float f0=FIXP16_2_FLOAT(t0);
	//float f1=FIXP16_2_FLOAT(t1);
	FIXP16 t2;//=(long long)t0*t1/65536;
	//int    t4=FIXP16_2_INT(t2);
	//float  t5=FIXP16_2_FLOAT(t2);
	FIXP16_Div(t0,t1,t2);
	int    t6=FIXP16_2_INT(t2);
	float  t7=FIXP16_2_FLOAT(t2);*/
	//system("pause");
	FreeConsole();
	return 0;
}

void testRegex(){
	/*
	regex_match要求完全匹配模式，而regex_search只要求目标串包含模式就行。
	此外regex_search中的std::smatch参数，在返回时，会在[0]在存放匹配过后的
	完整串，而在后续的容器按从前到后的顺序存放子表达式匹配的内容
	for (std::sregex_token_iterator i(weekend.begin(),weekend.end(), pattern); i != end ; ++i){
		std::cout << *i << std::endl;
	}

	.  ：任意单个字符
	[] ：字符集
	{} ：计数
	() ：子模式
	\  ：下一个字符具有特殊含义
	*  ：0个或多个
	+  ：一个或多个
	?  ：0个或一个
	|  ：或
	^  ：行的开始；否定
	$  ：行的结束

	\d ：一个十进制数字
	\l ：一个小写字母
	\s ：一个空白符（空格符，制表符等）
	\u ：一个大写字母
	\w ：一个字母（a～z或A～Z）或数字（0～9）或下划线（_）
	\D ：除了\d之外的字符
	\L ：除了\l之外的字符
	\S ：除了\s之外的字符
	\U ：除了\u之外的字符
	\W ：除了\w之外的字符
	这种方式，可以用一个模式匹配整个字串中所以符合的子串
	*/

	/*std::regex pat("Named object:\\s*\"(\\w+)\"");
	std::smatch matches;
	std::string buffer="Named object: \"Sphere\"";
	if(std::regex_search(buffer,matches,pat)){
		std::cout<<matches[0]<<" "<<matches[1]<<std::endl;
	}*/

	/*
	std::regex pat("(\\d+)\\D*(\\d+)");
	std::string testStr="Tri-mesh, Vertices: 482     Faces: 960";
	std::smatch matches;
	std::regex_search(testStr,matches,pat);
	std::cout<<matches[1]<<" "<<matches[2]<<std::endl;*/

	std::regex pat("([^\\\\/]+)$");
	std::string testStr="D:\\WINXPP Program Files\\ts5\\Textures\\metal04.bmp";
	std::smatch matches;
	std::regex_search(testStr,matches,pat);
	std::cout<<matches[1]<<" "<<matches[2]<<" "<<matches[3]<<" "<<matches[4]<<std::endl;
}