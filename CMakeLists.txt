set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")

set(LIB_LIST 
log.cpp 
cb_time.cpp 
threadpool.cpp 
)
set(SRC_LIST
main.cpp 
)
add_library(simple ${LIB_LIST})
add_executable(fucktest ${SRC_LIST})
target_link_libraries(fucktest simple)

