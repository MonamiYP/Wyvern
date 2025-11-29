file(GLOB_RECURSE TESTAPP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(testapp ${TESTAPP_SOURCES})

target_include_directories(testapp
    PRIVATE ${CMAKE_SOURCE_DIR}/wyvern/include
)

# Link testbed to engine dylib
target_link_libraries(testapp
    PRIVATE wyvern
)