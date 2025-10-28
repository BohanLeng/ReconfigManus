cmake_minimum_required(VERSION 3.20)

project(MESServer)

add_executable(${PROJECT_NAME} main.cpp
        MESServer.cpp
        MESServer.h
)

configure_file(mes_server_cfg.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mes_server_cfg.json COPYONLY)

include_directories(${ROOT_DIR}/TCPConn)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)


if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    set(Boost_NO_WARN_NEW_VERSIONS 1)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREAD ON)
endif()

if (TARGET TCPConn)
    target_link_libraries(${PROJECT_NAME} PRIVATE TCPConn)
else()
    find_library(TCPConn_LIB
            NAMES TCPConn libTCPConn TCPConn.lib libTCPConn.dll.a
            HINTS
                ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    if (NOT TCPConn_LIB)
        message(FATAL_ERROR "Could not find TCPConn library, please confirm it has been built.
        Looked in: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}; ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}; ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE ${TCPConn_LIB})
endif()