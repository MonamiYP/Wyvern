# ────────────────────────────────────────────────
# Wyvern is a game engine
# ────────────────────────────────────────────────

project(wyvern)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC lib/glfw/include
    PUBLIC lib
)

add_subdirectory(lib/glfw)

target_link_libraries(${PROJECT_NAME} 
    glfw
    Vulkan::Vulkan
)

# For debug builds only
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:_DEBUG>
)