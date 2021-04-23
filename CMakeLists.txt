cmake_minimum_required(VERSION 3.10)


if (APPLE) 
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_OSX_ARCHITECTURES x86_64)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)
set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/Cellar/openssl@1.1/1.1.1k/)
set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
endif()

# Set the project name.
project( xqc )

# Set the output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_FILES_DIRECTORY ${CMAKE_BINARY_DIR}/build )

if( UNIX AND NOT APPLE )
set ( CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/ )
endif()


# Ensure that openSSL, boost and cpprestsdk exist.
find_package(OpenSSL 1.1 REQUIRED)
find_package(CURL REQUIRED)

# Set up the include directories.
include_directories(
"${PROJECT_SOURCE_DIR}/headers"
"${CMAKE_INSTALL_PREFIX}/include"
/usr/local/include
)

#add_executable(ibc ${PROJECT_SOURCE_DIR}/Source/main.cpp)

message(STATUS "Source Directory = ${PROJECT_SOURCE_DIR}")
message(STATUS "Current Source Directory = ${CMAKE_CURRENT_SOURCE_DIR}")


file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.* )

ADD_LIBRARY( xqc STATIC )


foreach(FILE ${SRCS})

    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.c$")
    
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/source" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        
       set(GROUP "Source Files${GROUP}")
       source_group("${GROUP}" FILES "${FILE}")
	   

    elseif("${FILE}" MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/headers/.*\\.h$")
    
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/headers" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        
        set(GROUP "Header Files${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
        target_sources(xqc PRIVATE ${FILE})
    endif()
    
endforeach()


add_subdirectory( ${PROJECT_SOURCE_DIR}/source/ext )
add_subdirectory( ${PROJECT_SOURCE_DIR}/source/xq )

add_executable( test )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if( UNIX AND NOT APPLE )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lcurl")
set(PLATFORM_SPECIFIC_LIBS "curl -lpthread")
else()
set(PLATFORM_SPECIFIC_LIBS "CURL")
endif()


set_target_properties(test PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_sources( test PRIVATE ${PROJECT_SOURCE_DIR}/test/test.c  ${PROJECT_SOURCE_DIR}/config/xq.ini   )
set(GROUP "Source Files")
source_group("Source Files" FILES  ${PROJECT_SOURCE_DIR}/test/test.c )
set(GROUP "Header Files")
set(GROUP "Config")
source_group("Config" FILES ${PROJECT_SOURCE_DIR}/config/xq.ini )
target_link_libraries( test xqc  OpenSSL::SSL ${PLATFORM_SPECIFIC_LIBS} )
file(COPY ${PROJECT_SOURCE_DIR}/config/xq.ini DESTINATION  ${CMAKE_BINARY_DIR}/bin )

