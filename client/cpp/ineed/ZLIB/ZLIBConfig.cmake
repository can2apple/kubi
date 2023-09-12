
set(ZLIB_Found 1)
set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
IF (CMAKE_BUILD_TYPE STREQUAL Debug)       
    set(zlib_LIBRARIES  ${CMAKE_CURRENT_LIST_DIR}/lib/zlibd.lib  )
    set(ZLIB_LIBRARY  ${CMAKE_CURRENT_LIST_DIR}/lib/zlibd.lib  )
ELSE()
    set(zlib_LIBRARIES  ${CMAKE_CURRENT_LIST_DIR}/lib/zlib.lib  )    
    set(ZLIB_LIBRARY  ${CMAKE_CURRENT_LIST_DIR}/lib/zlib.lib  )                               
ENDIF()

