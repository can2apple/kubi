
set(Libuv_Found 1)
set(Libuv_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
set(Libuv_INCLUDE_DIR ${Libuv_INCLUDE_DIRS})
IF (CMAKE_BUILD_TYPE STREQUAL Debug)       
    set(Libuv_LIBRARIES  ${CMAKE_CURRENT_LIST_DIR}/lib/Debug/uv.lib  )
ELSE()
    set(Libuv_LIBRARIES  ${CMAKE_CURRENT_LIST_DIR}/lib/Release/uv.lib  )                         
ENDIF()
set(Libuv_LIBRARY ${Libuv_LIBRARIES})
