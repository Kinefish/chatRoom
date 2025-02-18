#头文件搜索路径
#include_directories()

#库文件搜索路径
#link_directories()

set(SRC_LIST test_muduo.cpp)

#将当前目录下源文件一起放入SRC_LIST中编译
#aux_source_directory(. ${SRC_LIST})

add_executable(test_muduo ${SRC_LIST})

#动态库连接
target_link_libraries(test_muduo muduo_net muduo_base pthread)
