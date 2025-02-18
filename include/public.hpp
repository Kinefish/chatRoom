#ifndef __PUBLIC_H__
#define __PUBLIC_H__

enum MSG_TYPE {
    TYPE_LOGIN = 1, //登录
    TYPE_LOGIN_ACK,
    TYPE_REG,   //注册
    TYPE_REG_ACK,
    TYPE_MSG_ONE_2_ONE, //1-1聊天
    TYPE_ADD_FRIEND, //好友添加
};

#endif