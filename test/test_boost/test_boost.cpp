#include <iostream>
#include <string>


int main() {
    std::string str = "hello world";
    boost::to_upper(str);
    std::cout << str << std::endl;
    return 0;
}