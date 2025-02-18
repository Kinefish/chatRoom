#ifndef __USER_H__
#define __USER_H__
#include <string>
class User {
public:
    User(int id = -1, std::string name = "", std::string password = "", std::string state = "offline")
        :id_(id), name_(name), password_(password), state_(state) {}
    ~User() = default;

    int getId() const { return id_; }
    std::string getName() const { return name_; }
    std::string getPassword() const { return password_; }
    std::string getState() const { return state_; }

    void setId(const int id) { this->id_ = id; } 
    void setName(const std::string name) { this->name_ = name; } 
    void setPassword(const std::string password) { this->password_ = password; } 
    void setState(const std::string state) { this->state_ = state; }
private:
    int id_;
    std::string name_;
    std::string password_;
    std::string state_;
};

#endif