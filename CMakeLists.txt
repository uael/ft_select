cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
cmake_policy(SET CMP0054 OLD)
cmake_policy(SET CMP0045 OLD)

project(ft_select C)
add_subdirectory(libft)

set(BUILD_SHARED_LIBS OFF)

set(ft_select_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(ft_select_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(ft_select_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)

set(CMAKE_C_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")

set(ft_select_HDR ${ft_select_INC_DIR}/ft_select.h)
file(GLOB_RECURSE ft_select_SRCS ${ft_select_SRCS} ${ft_select_SRC_DIR}/*.c)

add_executable(ft_select ${ft_select_HDR} ${ft_select_HDRS} ${ft_select_SRCS})
set_property(TARGET ft_select PROPERTY C_STANDARD 90)
target_include_directories(ft_select PUBLIC ${ft_select_INC_DIR})
target_link_libraries(ft_select PUBLIC ft)

target_compile_definitions(ft_select
    PRIVATE $<$<CONFIG:Debug>:DEBUG>$<$<CONFIG:Release>:NDEBUG>)
if (MSVC)
    target_compile_options(ft_select
        PRIVATE /MT$<$<CONFIG:Debug>:d> /Oy /W3
        PRIVATE /D_CRT_SECURE_NO_WARNINGS /Dinline=__inline
        PRIVATE /O$<$<CONFIG:Debug>:d>$<$<CONFIG:Release>:x>)
else ()
    target_compile_options(ft_select
        PRIVATE -Wall -Werror -Wextra -fomit-frame-pointer -ansi
        PRIVATE -Wno-missing-field-initializers -Wno-missing-braces
        PRIVATE -O$<$<CONFIG:Debug>:0 -g3>$<$<CONFIG:Release>:3>)
endif ()
