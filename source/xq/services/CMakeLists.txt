add_subdirectory( quantum )
add_subdirectory( sub )
add_subdirectory( val )

target_sources(xqc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/crypto.c
	${CMAKE_CURRENT_LIST_DIR}/decrypt.c
	${CMAKE_CURRENT_LIST_DIR}/encrypt.c
)