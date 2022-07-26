cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)

project(PodcastDownloader VERSION 0.0.1 LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(pods main.cpp XmlNode.cpp XmlDoc.cpp MyUtils.cpp HttpClient.cpp Iterator.cpp)

# target_compile_options(pods PUBLIC "-fdiagnostics-color")
# target_compile_options(pods PUBLIC )
add_compile_options("/WX /Wall")

target_compile_features(pods PUBLIC cxx_std_17)
target_link_libraries(pods PUBLIC ${CONAN_LIBS})


add_custom_command (
    TARGET pods
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../cacert.pem" "${CMAKE_BINARY_DIR}"
)

if ( NOT ( "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" STREQUAL "${CMAKE_BINARY_DIR}" ) )
    message("copying to runtime dir")
    add_custom_command(
        TARGET pods
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../cacert.pem" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
endif ()
