#include <iostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp> 

using namespace std;
 
int main(int argc, char** argv)
{
    string s = "sss/ddd,  ggg  ";
    vector<string> vStr;
    boost::split(vStr, s, boost::is_any_of(",/"), boost::token_compress_on);
    boost::trim(*(--vStr.end()));
    cout << (*(--vStr.end()));
    
    return 0;
}
