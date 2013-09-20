set(ALL_IDL_FILENAMES
	Ami.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

add_library(ami STATIC
  ${TAO_IDL_GENERATED}
  AmiImpl.h
  AmiImpl.cpp
  AmiCallbackImpl.h
  AmiCallbackImpl.cpp
)

link_libraries(
  ami
  miro
)

set( TARGETS
  ami_server_example
  ami_client_example
  ami_colocated_example
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp)
	# Add target to DartTestfile.txt - [db] 6/4/08
	# (Test hangs when you run ami_server_example)
	#add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS} )

install_targets(${EXAMPLES_BIN_DIR}
  ${TARGETS}
)
