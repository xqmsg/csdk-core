add_subdirectory( algorithms )
add_subdirectory( services )

target_sources(xqc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/config.c
	${CMAKE_CURRENT_LIST_DIR}/connect.c
)