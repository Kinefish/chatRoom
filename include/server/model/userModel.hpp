#ifndef __USERMODEL_H__
#define __USERMODEL_H__

#include <string>
#include "user.hpp"
class UserModel {
public:
    bool insert(User&);
    User query(int);
    bool updateState(User&);
};

#endif