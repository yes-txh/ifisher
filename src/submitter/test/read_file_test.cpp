#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <stdlib.h>
using namespace std;


//从文件读入到string里
string readFileIntoString(char * filename)
{
ifstream ifile(filename);
//将文件读入到ostringstream对象buf中
ostringstream buf;
char ch;
while(buf&&ifile.get(ch))
buf.put(ch);
//返回与流对象buf关联的字符串
return buf.str();
}


int main()
{
//文件名
char * fn="/home/yesw/a.txt";
string str;
str=readFileIntoString(fn);
cout<<str<<endl;
system("pause");

}
