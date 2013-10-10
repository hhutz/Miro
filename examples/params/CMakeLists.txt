set( MIRO_PARAMS_ARGS -j )

miro_makeparams( SimpleSurveyParameters.xml )

include_directories(
  ${JSONCPP_INCLUDE_DIR}
)

add_library( miroParamsExample SHARED
  SimpleSurveyParameters.cpp
)

target_link_libraries( miroParamsExample
  miroJson
  miroXml
)
if ( MIRO_BUILD_WITH_JSON )
  target_link_libraries( 
    ${JSONCPP_LIBRARIES}
  )
endif ( MIRO_BUILD_WITH_JSON )

link_libraries(
  miroParamsExample
)

set( LOCAL_TARGETS
  convertParams
)

foreach( LOCAL_TARGET ${LOCAL_TARGETS} )
  add_executable( ${LOCAL_TARGET} 
    ${LOCAL_TARGET}.cpp
  )
endforeach( LOCAL_TARGET ${LOCAL_TARGETS} )

install_targets(${EXAMPLES_BIN_DIR}
  ${LOCAL_TARGETS}
)
