# CMakeList.txt: проект CMake для testManager; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("testManager")

add_subdirectory(${CMAKE_SOURCE_DIR}/googletest googletest-build)


add_executable (testManager "testManager.cpp" "testManager.h" "../messageManager/messageManager.cpp" )


target_include_directories(testManager PRIVATE
    ../messageManager
    ../receiver
    ../googletest 
)

target_link_libraries(testManager PRIVATE gtest_main)

enable_testing()
include(GoogleTest)	
gtest_discover_tests(testManager)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET testManager PROPERTY CXX_STANDARD 20)
endif()
