target_sources(xqc PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/authorize_alias.c
	${CMAKE_CURRENT_LIST_DIR}/authorize.c
	${CMAKE_CURRENT_LIST_DIR}/code_validation.c
	${CMAKE_CURRENT_LIST_DIR}/combined.c
	${CMAKE_CURRENT_LIST_DIR}/delegate.c
	${CMAKE_CURRENT_LIST_DIR}/exchange.c
	${CMAKE_CURRENT_LIST_DIR}/packet.c
	${CMAKE_CURRENT_LIST_DIR}/settings.c
	${CMAKE_CURRENT_LIST_DIR}/subscriber.c
)