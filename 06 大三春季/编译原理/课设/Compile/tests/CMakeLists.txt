add_library(pl0_test_support STATIC unit/TestSupport.cpp)
target_link_libraries(pl0_test_support PRIVATE pl0::pl0)
target_include_directories(pl0_test_support PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/unit)

add_executable(pl0_tests
  unit/TestMain.cpp
  unit/LexerTests.cpp
  unit/ParserTests.cpp
  unit/CodegenTests.cpp
  unit/VmTests.cpp
)

target_link_libraries(pl0_tests PRIVATE pl0::pl0 pl0_test_support)

add_test(NAME pl0 COMMAND pl0_tests)
