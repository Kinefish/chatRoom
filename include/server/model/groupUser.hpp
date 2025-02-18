#ifndef __GROUPUSER_H__
#define __GROUPUSER_H__
#include "user.hpp"
class GroupUser : public User
{
public:
    void setRole(std::string role) { role_ = role; }
    const std::string getRole() { return role_; }
private:
    std::string role_;
};



#endif