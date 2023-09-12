
set(LibUv_Found 1)
set(LibUv_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
IF (CMAKE_BUILD_TYPE STREQUAL Debug)       
    set(LibUv_LIBRARIES  ${CMAKE_CURRENT_LIST_DIR}/lib/Debug/uv.lib  )
ELSE()
    set(LibUv_LIBRARIES  ${CMAKE_CURRENT_LIST_DIR}/lib/Release/uv.lib  )                         
ENDIF()

