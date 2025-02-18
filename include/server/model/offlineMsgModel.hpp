#ifndef __OFFLINEMSGMODEL_H__
#define __OFFLINEMSGMODEL_H__

#include <vector>
#include <string>
/*
没有提供OffLineMsg类，因为只有两个idx
*/
class OffLineMsgModel
{
public:
    bool insert(int, std::string);
    std::vector<std::string> query(int);
    bool remove(int);
};

#endif