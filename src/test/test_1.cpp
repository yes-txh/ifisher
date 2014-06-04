#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)  
  #include<iostream>
  #include<map>
#endif

//////////////////#define G 2
using std::map;

int main(){
   map<int, int> test_map;
   test_map[1] = 2;
   test_map[2] = 3;
   if(test_map.size() ==2 )
       std::cout<< __GNUC__ <<std::endl;
   else
       std::cout<< "no" <<std::endl;
   return 0;
}
