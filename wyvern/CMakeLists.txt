# ────────────────────────────────────────────────
# Wyvern is a game engine
# ────────────────────────────────────────────────

project(wyvern)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${ENGINE_SOURCES})

# ────────────────────────────────────────────────
# Shader compilation
# ────────────────────────────────────────────────
set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/shaders")
file(GLOB GLSL_SHADERS "${SHADER_SRC_DIR}/*.vert" "${SHADER_SRC_DIR}/*.frag")
set(SPIRV_OUTPUT_DIR "${CMAKE_BINARY_DIR}/res/shaders")
file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

# Find glslc from Vulkan SDK
find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/bin")
if(NOT GLSLC)
    message(FATAL_ERROR "glslc not found! Make sure Vulkan SDK is installed and VULKAN_SDK is set.")
endif()

# # Add custom command for each shader
set(SPIRV_OUTPUTS "") # Collect compiled shader outputs
foreach(GLSL_SHADER ${GLSL_SHADERS})
    get_filename_component(FILE_NAME ${GLSL_SHADER} NAME)
    set(SPIRV_FILE "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSLC} ${GLSL_SHADER} -o ${SPIRV_FILE}
        DEPENDS ${GLSL_SHADER}
        COMMENT "Compiling shader: ${FILE_NAME}"
        VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV_FILE})
endforeach()

# Custom target to build shaders
add_custom_target(Shaders ALL DEPENDS ${SPIRV_OUTPUTS})
add_dependencies(${PROJECT_NAME} Shaders)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC lib/glfw/include
    PUBLIC res
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

# ───────────────
# Copy `res/` next to binary
# ───────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SPIRV_OUTPUT_DIR}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders
)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    SHADER_DIR="${CMAKE_BINARY_DIR}/res/shaders/"
)
