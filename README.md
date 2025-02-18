# chatRoom
 基于muduo网络库的chatRoom聊天室

## 网络模块代码

- 使用`muduo`网络库进行封装`I/O`和读写

## 业务模块代码

- 业务类只需要一个实例即可，使用单例模式
- 业务中看到的都应该是对象

- 根据不同的`msgtype`区分不同的业务类型，使用`std::bind`绑定相应的处理逻辑，可以实现业务和网络的解耦
- 避免使用`throw`导致进程在业务模块`core dump`

### 登录业务

`{"typeId":1,"id":15,"password":"666666"}`

- 通过`userConnMap_`记录在线的用户，用于服务器推送消息
- 需要**线程安全**，因为不同用户的上线，下线会对这个`Map`进行读写操作，相当于多线程环境
- 异常退出时，需要移除在线`Map`的信息，使用`c20`的`erase_if`替代了`for(const auto& it)`查找当前`con`，并且不能使用`find_if`进行`erase`，会造成迭代器失效

### 聊天业务【TODO】

`{"typeId":5,"from":13,"to":15,"message":"hello?"}`

`{"typeId":5,"from":21,"to":13,"message":"what the hell?"}`

- 通过`id`，服务器直接转发`json`
- **【TODO】**在线用户需要加锁访问`Map`，如果同时多个在线用户发一对一消息，是否会慢 ?

## 数据模块代码

- 实现对数据库的连接，提供增删改查接口

## 映射模块代码

- 建立数据库表的`ORM`类、`table_model`类，其中`ORM`类只提供字段的`getter/setter`方法，在`table_model`类中实现这些数据的操作方法，调用增删改查接口
- 数据操作的操作方法，如`user`类，有`name`字段，那么操作数据的方法就是修改`name`，新增`user`等

