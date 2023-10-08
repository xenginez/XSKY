cmake_minimum_required (VERSION 3.8)

project ("route")

set(SOURCES route.c ../common/hash_map.c)
add_compile_options(-Wall -Wextra)

#add_module(route ${SOURCES})


set(KERNEL_DIR /usr/src/linux-headers-5.15.0-84-generic)

function(compile_module obj srcs)
	set(TARGET_NAME ${obj})
	add_custom_target(${TARGET_NAME} ALL cp -f ${srcs}
		COMMAND echo "compiling module ${obj}.ko..."
	)
	list(LENGTH ARGN argn_len)
	set(i 0)
	set(depend_objlist "")
	while( i LESS ${argn_len})
		list(GET ARGN ${i} argn_value)
		set(depend_objlist "${depend_objlist} ${argn_value}.o")
		math(EXPR i "${i} + 1")
	endwhile()

	add_custom_command(TARGET ${TARGET_NAME}
	POST_BUILD
	COMMAND echo "obj-m := ${obj}.o" > ${CMAKE_CURRENT_BINARY_DIR}/Makefile
	COMMAND echo "${obj}-objs:=${depend_objlist}" >>${CMAKE_CURRENT_BINARY_DIR}/Makefile
	COMMAND make -C ${KERNEL_DIR} M=${CMAKE_CURRENT_BINARY_DIR} modules
	)
endfunction()

compile_module(route SOURCES)