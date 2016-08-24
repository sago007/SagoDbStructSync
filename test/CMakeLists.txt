find_package(Boost COMPONENTS program_options system filesystem unit_test_framework REQUIRED)

file(GLOB TEST_SOURCES "*.cpp")

foreach(loop_var "${TEST_SOURCES}")
  get_filename_component(RESULT_NAME ${loop_var} NAME_WE)
  add_executable("${RESULT_NAME}.result" "${loop_var}")
  TARGET_LINK_LIBRARIES( ${RESULT_NAME}.result SagoDbStructSync ${Boost_LIBRARIES} cppdb )
  add_test(${RESULT_NAME} ${RESULT_NAME}.result)
endforeach(loop_var)

#add_executable(dbsync-test sampletest.cpp)
#TARGET_LINK_LIBRARIES( dbsync-test SagoDbStructSync ${Boost_LIBRARIES} cppdb )
#add_test(dbsync-test dbsync-test)