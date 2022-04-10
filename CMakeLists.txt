cmake_minimum_required(VERSION 3.12) # version can be different

project(bean_Tests) #name of your project

add_subdirectory(googletest/googletest) # add googletest subdirectory

include_directories(googletest/googletest/include googletest/googlemock/include) # this is so we can #include <gtest/gtest.h>

add_executable(bean.test recBean.test.cpp sendBean.test.cpp) # add this executable

target_link_libraries(bean.test PRIVATE gtest) # link google test to this executable

# line below is not working
# to make project buildable:
# 1. Open build/.sln in Visual Studio
# 2. Go to newmain.test -> Properties -> Configuration Properties -> C/C++ -> Code Generation -> Runtime Library
# 3. Set to /MTd option
# 4. Set newmain.test as startup project

# set_property(TARGET newmain.test PROPERTY
# MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
# lines below set proper runtime libraries
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")