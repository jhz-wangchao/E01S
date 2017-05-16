#include "log.h"
#include <iostream>
using namespace std;

void Log::Error(const char * Message)
{
	cout<<"Error\t\t"<<Message<<"\r"<<endl;
}
void Log::Waring(const char * Message)
{
	cout<<"Waring\t\t"<<Message<<"\r"<<endl;
}
void Log::Infomation(const char * Message)
{
	cout<<"Information\t"<<Message<<"\r"<<endl;
}
