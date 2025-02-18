#include <json.hpp>
#include <iostream>
#include <string>
#include <vector>
using json = nlohmann::json;

//json 序列化
static std::string func1() {
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are u doing?";

    std::cout << js << std::endl;

    std::string str = js.dump();
    return str;
}

//json 反序列化
static void func2() {
    std::string recvBuf = func1();
    json jsbuf = json::parse(recvBuf);
    std::cout << jsbuf["msg_type"] << std::endl;
}

int main() {
    func2();
    return 0;
}
