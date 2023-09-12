cmake_minimum_required (VERSION 3.7)
project(kubi_server)
option(IF_DEBUG         "Set to ON to build use debug"  OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -std=c++latest -O2 -stdlib=libc++ -isysroot  ${CMAKE_OSX_SYSROOT}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAG}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


set(INEED_LIB_DIR  "./ineed")
if(WIN32)
    set (KITS_PAL x64)
    set(IF_USR_SOURCE_GROUP TRUE)
    set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAG}")
    ADD_DEFINITIONS(-DHAVE_CONFIG_H -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN )
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
        endif()
    set(_GOBAL_LIB  WS2_32.lib imm32.lib  Secur32.lib Shlwapi.lib bcrypt.lib vfw32.lib comctl32.lib OpenGL32.Lib
	      Winmm.lib Version.lib Strmiids.lib Iphlpapi.lib Psapi.lib userenv.lib netapi32.lib Crypt32.lib )
    #source:
    function(fun_file_filter _argv_)
        list(LENGTH ARGV  len)
        set(ind 0)
        while(ind LESS ${len})
            list(GET ARGV ${ind} node)  
            get_filename_component(PARENT_DIR "${node}" DIRECTORY)
            string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
            source_group("${GROUP}" FILES "${node}")
            math(EXPR ind "${ind}+1")
        endwhile()
    endfunction()
endif()

set(CMAKE_TYPE 1)
if (IF_DEBUG)
set(CMAKE_TYPE 0)
endif()
list(GET CMAKE_CONFIGURATION_TYPES ${CMAKE_TYPE} CMAKE_BUILD_TYPE)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
set(DEBUG_OR_RELEASE ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

set(gflags_DIR ${INEED_LIB_DIR}/gflags/lib/cmake/gflags)
find_package(gflags REQUIRED CONFIG NO_DEFAULT_PATH) 
set(glog_DIR ${INEED_LIB_DIR}/glog/lib/cmake/glog)
find_package(glog REQUIRED CONFIG NO_DEFAULT_PATH)
set(jsoncpp_DIR ${INEED_LIB_DIR}/jsoncpp)
find_package(jsoncpp REQUIRED CONFIG NO_DEFAULT_PATH)
set(Libuv_DIR ${INEED_LIB_DIR}/Libuv)
find_package(Libuv REQUIRED CONFIG NO_DEFAULT_PATH)
set(protobuf_DIR ${INEED_LIB_DIR}/protobuf/cmake)
find_package(protobuf REQUIRED CONFIG NO_DEFAULT_PATH)

#…Ë÷√Pythonƒø±Í£∫
set(PYTHON_EXECUTABLE "C:/Python38/python.exe")
set(pybind11_DIR ${INEED_LIB_DIR}/pybind11/share/cmake/pybind11)
find_package(pybind11 REQUIRED CONFIG NO_DEFAULT_PATH)


set(DLL_FILES)
if(WIN32)
 
    set(Libuv_DLL_FILES ${Libuv_DIR}/lib/Release/uv.dll )
    set(protobuf_DLL_FILES ${protobuf_DIR}/../bin/libprotobuf.dll
                        ${protobuf_DIR}/../bin/libprotobuf-lite.dll
                        ${protobuf_DIR}/../bin/libprotoc.dll
    )
    if (CMAKE_BUILD_TYPE STREQUAL Debug) 
        set(Libuv_DLL_FILES ${Libuv_DIR}/lib/Debug/uv.dll )
        set(protobuf_DLL_FILES ${protobuf_DIR}/../bin/libprotobufd.dll
                        ${protobuf_DIR}/../bin/libprotobuf-lited.dll
                        ${protobuf_DIR}/../bin/libprotocd.dll
        )
     
    endif()
endif()
file(MAKE_DIRECTORY ${DEBUG_OR_RELEASE}) 

list(APPEND DLL_FILES ${Libuv_DLL_FILES} )
list(APPEND DLL_FILES ${protobuf_DLL_FILES} )
file(COPY ${DLL_FILES} DESTINATION ${DEBUG_OR_RELEASE})  



set(_SRC2 
crypto/sha1.h
crypto/sha1.c
crypto/md5.h
crypto/md5.c
crypto/base64_encoder.h
crypto/base64_encoder.c
crypto/base64_decoder.h
crypto/base64_decoder.c

http_parser.h
http_parser.c
websocket_parser.h
websocket_parser.c
Kubi.pb.h
Kubi.pb.cc
kubi_utils.h
kubi_utils.cpp
kubi_ws.h
kubi_ws.cpp
kubi_session.h
kubi_session.cpp
kubi_ser.h
kubi_ser.cpp 
)
 include_directories( ${Libuv_INCLUDE_DIR}  ${jsoncpp_INCLUDE_DIRS})

 set(SRC_IM
    kubi_ser_cpp11_imp.h
    kubi_ser_cpp11_imp.cpp
    demo.cpp
)
 

 add_executable( kubi_ser
    ${_SRC2} ${SRC_IM}
    )
target_link_libraries(kubi_ser PRIVATE ${_GOBAL_LIB} gflags glog::glog ${jsoncpp_LIBRARIES}  ${Libuv_LIBRARY}  protobuf::libprotobuf protobuf::libprotoc )
target_include_directories(kubi_ser PRIVATE ${Libuv_INCLUDE_DIR}  ${jsoncpp_INCLUDE_DIRS}  )

#set(Python_root "C:/Python38")
#set(Python_numpy_root ${Python_root}/Lib/site-packages/numpy/core )
#set(PYTHON_INCLUDE_DIRS "${Python_root}/include" ${Python_numpy_root}/include)
#set(PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIRS})
#set(PYTHON_LIBRARIES  ${Python_root}/libs/python3.lib ${Python_root}/libs/python38.lib  ${Python_numpy_root}/lib/npymath.lib)
#set(PYTHON_LIBRARIE ${Python_LIBRARIES})


 set(SRC_IM_PY
    kubi_ser_py_imp.h
    kubi_ser_py_imp.cpp
)
pybind11_add_module(kubi_ser_py
    ${_SRC2}
    ${SRC_IM_PY}
 
)
target_include_directories(kubi_ser_py PRIVATE ${Libuv_INCLUDE_DIR}  ${jsoncpp_INCLUDE_DIRS})
target_link_libraries(kubi_ser_py PRIVATE ${_GOBAL_LIB} gflags glog::glog ${jsoncpp_LIBRARIES}  ${Libuv_LIBRARY}  protobuf::libprotobuf protobuf::libprotoc)



SET(PY_FILES  kubi_ser.py  kubi_ser_py.py  kubi_ser_demo.py)
list(APPEND RESULUT_FILES ${PY_FILES} )
file(COPY ${RESULUT_FILES} DESTINATION ${DEBUG_OR_RELEASE})  

if(IF_USR_SOURCE_GROUP)
    set(all_files  ${_SRC2} ${SRC_IM} ${SRC_IM_PY}
    )

fun_file_filter(${all_files})
endif(IF_USR_SOURCE_GROUP)
