###################################################
# Thanks to Alexandru Naiman for optimization ideas
###################################################

cmake_minimum_required (VERSION 3.0)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE, DEBUG)
endif (NOT CMAKE_BUILD_TYPE)

set (CMAKE_CXX_STANDARD 17)

if(NOT MSVC)
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g2 -O0 -Wall -Werror -rdynamic")
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0 -Wall -Werror -march=native -mtune=native -funroll-loops -Ofast -fno-math-errno -fomit-frame-pointer -foptimize-strlen -ftree-loop-distribution -ftree-loop-distribute-patterns -ffast-math -flto")
else(NOT MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /nologo /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /D_DEBUG /GS /Zi /debug")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /nologo /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /GS /GL /Gm /O2 /Od /Ox")
	add_compile_options(/MP)
endif(NOT MSVC)

file (GLOB_RECURSE LiteEditorSourceFiles *)

add_library (LiteEditor SHARED ${LiteEditorSourceFiles})
set_property (TARGET LiteEditor PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories (LiteEditor PUBLIC ${PROJECT_SOURCE_DIR}/Editor)
target_include_directories (LiteEditor PRIVATE ${PROJECT_SOURCE_DIR}/Engine)
target_include_directories (LiteEditor PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/include/glm)

if(NOT MSVC)
	find_package (Bullet REQUIRED)
	
	include_directories (${BULLET_INCLUDE_DIRS})
else(NOT MSVC)
	include_directories (${PROJECT_SOURCE_DIR}/3rdparty/include/bullet)
endif(NOT MSVC)

if(MSVC)
	include_directories (${PROJECT_SOURCE_DIR}/3rdparty/include)
endif(MSVC)

if(MSVC)
	MACRO(GROUP_SRC SRCS)
	  foreach(FILE ${LiteEditorSourceFiles}) 
		#convert source file to absolute
		get_filename_component(ABSOLUTE_PATH "${FILE}" ABSOLUTE)
		# Get the directory of the absolute source file
		get_filename_component(PARENT_DIR "${ABSOLUTE_PATH}" DIRECTORY)
		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")
		# Group into "Source Files" and "Header Files"
		if ("${FILE}" MATCHES ".*\\.cpp")
	      set(GROUP "Source Files${GROUP}")
	    elseif("${FILE}" MATCHES ".*\\.h")
	      set(GROUP "Header Files${GROUP}")
	    endif()
	    source_group("${GROUP}" FILES "${FILE}")
	  endforeach()
	ENDMACRO(GROUP_SRC)

	group_src (${LiteEditorSourceFiles})
endif(MSVC)

if(NOT MSVC)
	target_link_libraries (LiteEditor stdc++fs)

	target_compile_definitions (LiteEditor PRIVATE "ENGINE_API=")
else(NOT MSVC)
	target_link_libraries (LiteEditor LiteEngine)

	target_compile_definitions (LiteEditor PRIVATE "ENGINE_API=__declspec(dllimport)")
endif(NOT MSVC)

target_compile_definitions (LiteEditor PRIVATE GLM_ENABLE_EXPERIMENTAL)
