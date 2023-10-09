cmake_minimum_required (VERSION 3.8)

project(route)

set(KERNEL_DIR /usr/src/linux-headers-5.15.0-84-generic)
set(KERNEL_INCLUDE_DIR /usr/include)

if(LINUX_KNL)

add_executable (route "main.c")
target_include_directories(route PRIVATE ${CMAKE_SOURCE_DIR} ${LINUX_HEADER}${KERNEL_DIR} ${LINUX_HEADER}${KERNEL_INCLUDE_DIR})
target_compile_definitions(route PRIVATE -DMODULE -D__KERNEL__ -D__LITTLE_ENDIAN_BITFIELD -D__UAPI_DEF_IN6_ADDR -D__UAPI_DEF_IN6_ADDR_ALT)

else()

add_custom_target(route ALL cp -r -f ${CMAKE_CURRENT_SOURCE_DIR}/../common ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_BINARY_DIR}/ COMMAND echo "compiling module route.ko...")
list(LENGTH ARGN argn_len)
set(i 0)
set(depend_objlist "")
while(i LESS ${argn_len})  
    list(GET ARGN ${i} argn_value)  
    set(depend_objlist "${depend_objlist} ${argn_value}.o") 
    math(EXPR i "${i} + 1")  
endwhile() 

add_custom_command(TARGET route
    POST_BUILD
    COMMAND cp -f Makefile Makefile.bak
    COMMAND echo "obj-m += main.o" > ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND echo "ccflags-y += -O2 -DMODULE -D__KERNEL__" >> ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND echo "route-objs:=${depend_objlist}" >> ${CMAKE_CURRENT_BINARY_DIR}/Makefile
    COMMAND make -C ${KERNEL_DIR} M=${CMAKE_CURRENT_BINARY_DIR} modules
    COMMAND cp -f Makefile Makefile.op
    COMMAND cp -f Makefile.bak Makefile
)

endif()
