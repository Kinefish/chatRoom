#ifndef __FRINEDMODEL_H__
#define __FRINEDMODEL_H__
#include <string>
#include <vector>
#include "user.hpp"
class FriendModel
{
public:
    bool insert(int, int);
    std::vector<User> query(int);
};

#endif