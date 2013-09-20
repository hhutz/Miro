set(ALL_IDL_FILENAMES
	Event.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

add_library(event STATIC
  ${TAO_IDL_GENERATED}
)

link_libraries(
	miro
  event
)

set( TARGETS
  admin_qos
  offer_change
  offer_list
  offer_size
  type_code_size
  offer_subscribe
  subscription_list
  test_consumer
  test_supplier
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp)
	# Add target to DartTestfile.txt - [db] 6/4/08
	add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS} )

install_targets(${TESTS_BIN_DIR}
  ${TARGETS}
)
