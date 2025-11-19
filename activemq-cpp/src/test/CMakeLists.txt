# Build tests under src/test

if(NOT TARGET CppUnit)
  find_package(CppUnit QUIET)
endif()

file(GLOB_RECURSE TEST_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

set(TEST_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
if(EXISTS ${TEST_MAIN})
  # Build a single test harness driven by the top-level main.cpp. Remove
  # main.cpp from the sources list, then create a single `test` executable
  # that contains all tests and the runner.
  list(REMOVE_ITEM TEST_SRCS ${TEST_MAIN})
  add_executable(test ${TEST_MAIN} ${TEST_SRCS})
  target_include_directories(test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/main
    ${CMAKE_SOURCE_DIR}/src/test
  )
  target_link_libraries(test PRIVATE activemq_cms activemq_core decaf)
  if(CppUnit_FOUND)
    if(TARGET CppUnit::CppUnit)
      target_link_libraries(test PRIVATE CppUnit::CppUnit)
    elseif(TARGET CppUnit)
      target_link_libraries(test PRIVATE CppUnit)
    else()
      target_link_libraries(test PRIVATE CppUnit)
    endif()
  endif()
else()
  # Fallback: build individual test binaries when there is no main.cpp
  foreach(_src IN LISTS TEST_SRCS)
    get_filename_component(_name ${_src} NAME)
    get_filename_component(_base ${_src} NAME_WE)
    add_executable(test_${_base} ${_src})
    target_include_directories(test_${_base} PRIVATE
      ${CMAKE_SOURCE_DIR}/src/main
      ${CMAKE_SOURCE_DIR}/src/test
    )
    target_link_libraries(test_${_base} PRIVATE activemq_cms activemq_core decaf)
    if(CppUnit_FOUND)
      if(TARGET CppUnit::CppUnit)
        target_link_libraries(test_${_base} PRIVATE CppUnit::CppUnit)
      elseif(TARGET CppUnit)
        target_link_libraries(test_${_base} PRIVATE CppUnit)
      else()
        target_link_libraries(test_${_base} PRIVATE CppUnit)
      endif()
    endif()
  endforeach()
endif()
