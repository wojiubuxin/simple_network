#可以建一个文件收纳一些编译后的一堆杂物
#mkdir build_command
#cmake ./
#make

set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")

set(LIB_LIST 
log.cpp 
cb_time.cpp 
threadpool.cpp 
socket_er.cpp 
epoll_er.cpp 
)
set(SRC_LIST
main.cpp 
)
set(SRC_LIST_CLIENT
clientmain.cpp 
)

#加进这个库里
add_library(simple ${LIB_LIST})

#执行文件
add_executable(fucktest_server ${SRC_LIST})

#再将它们链接起来
target_link_libraries(fucktest_server simple)

#这块是客户端的
add_executable(fucktest_client ${SRC_LIST_CLIENT})
target_link_libraries(fucktest_client simple)

#不用cmake的话，可以手动敲这些
#g++ -g -Wall -std=c++11 -pthread  log.cpp cb_time.cpp threadpool.cpp socket_er.cpp epoll_er.cpp  main.cpp -o fucktest_server
#g++ -g -Wall -std=c++11 -pthread  log.cpp cb_time.cpp threadpool.cpp socket_er.cpp epoll_er.cpp  clientmain.cpp -o fucktest_client
