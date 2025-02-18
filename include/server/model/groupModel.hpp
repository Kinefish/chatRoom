#ifndef __GROUPMODEL_H__
#define __GROUPMODEL_H__
#include "group.hpp"
#include <string>
class GroupModel
{
public:
    bool createGroup(Group& group);
    bool addGroup(int userId, int groupId, std::string role);
    std::vector<Group> queryGroups(int userId);
    std::vector<int> queryGroupUsers(int userId, int groupId);
};

#endif