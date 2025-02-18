#当前目录下所有源文件代码参加编译
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)

add_executable(chatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})
find_library(MYSQL_CLIENT_LIB mysqlclient PATHS /usr/lib64/mysql)

target_link_libraries(chatServer muduo_net muduo_base ${MYSQL_CLIENT_LIB} pthread)