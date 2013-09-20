set(ALL_IDL_FILENAMES
	Basic.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

add_library(basic STATIC
  ${TAO_IDL_GENERATED}
  BasicImpl.h
  BasicImpl.cpp
)

link_libraries(
	miro
  basic
)

set( TARGETS
  basic_client_example
  basic_server_example
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp)
	# Add target to DartTestfile.txt - [db] 6/4/08
	add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS} )

install_targets(${EXAMPLES_BIN_DIR}
  ${TARGETS}
)
