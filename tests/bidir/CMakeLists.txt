set(ALL_IDL_FILENAMES
	BiDir.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

add_library(bidir STATIC
  ${TAO_IDL_GENERATED}
  BiDirImpl.h
  BiDirImpl.cpp
)

link_libraries(
	miro
  bidir
)

set( TARGETS
  biDirClient
  biDirServer
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp	)
	# Add target to DartTestfile.txt - [db] 6/4/08
	add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS} )

install_targets(${TESTS_BIN_DIR}
  ${TARGETS}
)
