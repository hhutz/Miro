include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory( libA )
add_subdirectory( libB )
add_subdirectory( libC )

link_libraries(
	libA
	libB
	libC
  ${ACE_LIBRARIES}
  miroCore
  miro
)

set( TARGETS
  test_singleton
  test_miroSingleton
  test_repositorySingleton
  test_namingRepositorySingleton
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp)
	# add target to DartTestfile.txt - [db] 6/4/08
	add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS} )

install_targets(${TESTS_BIN_DIR}  ${TARGETS})
