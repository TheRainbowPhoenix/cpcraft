cmake_minimum_required(VERSION 3.15)

project(cpcraft_port_simulator LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Paths ---
set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(SRC_DIR "${ROOT_DIR}/src")
set(ENGINE_DIR "${SRC_DIR}/engine")
set(DEMO_DIR "${SRC_DIR}/demo")
set(SIM_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(SIM_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include")
set(WEB_DIR "${CMAKE_CURRENT_LIST_DIR}/web")

# --- Sources ---
file(GLOB ENGINE_SRCS CONFIGURE_DEPENDS "${ENGINE_DIR}/*.c")
file(GLOB DEMO_SRCS CONFIGURE_DEPENDS "${DEMO_DIR}/*.c")

set(APP_MAIN "${SRC_DIR}/main.cpp")
set(SIM_SRCS
    "${SIM_SRC_DIR}/simulator.cpp"
    "${SIM_SRC_DIR}/main_simulator.cpp"
)

set(ALL_SIM_SRCS
    ${ENGINE_SRCS}
    ${DEMO_SRCS}
    "${APP_MAIN}"
    ${SIM_SRCS}
)

# main.cpp belongs to the ClassPad app. Rename its main() so
# main_simulator.cpp can provide the actual native/web entry point.
set_source_files_properties("${APP_MAIN}" PROPERTIES
    COMPILE_DEFINITIONS main=app_main
)

function(configure_cpcraft_sim_target target_name)
    target_include_directories(${target_name} PRIVATE
        "${SIM_INCLUDE}"
        "${SRC_DIR}"
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wno-unused-parameter
        )
    endif()

    if(NOT WIN32 AND NOT EMSCRIPTEN)
        target_link_libraries(${target_name} PRIVATE m)
    endif()
endfunction()

if(EMSCRIPTEN)
    set(WEB_OUT_NAME "cpcraft-port")

    add_executable(cpcraft-port-web ${ALL_SIM_SRCS})
    configure_cpcraft_sim_target(cpcraft-port-web)

    set_target_properties(cpcraft-port-web PROPERTIES
        OUTPUT_NAME "${WEB_OUT_NAME}"
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY "${WEB_DIR}"
    )

    target_compile_definitions(cpcraft-port-web PRIVATE
        __EMSCRIPTEN__
        SIMULATOR_WEB
    )

    target_compile_options(cpcraft-port-web PRIVATE
        -sUSE_SDL=2
    )

    target_link_options(cpcraft-port-web PRIVATE
        -sUSE_SDL=2
        -sALLOW_MEMORY_GROWTH=1
        -sASYNCIFY=1
        --shell-file "${WEB_DIR}/shell.html"
    )
else()
    find_package(SDL2 CONFIG REQUIRED)

    add_executable(cpcraft-port-sim ${ALL_SIM_SRCS})
    configure_cpcraft_sim_target(cpcraft-port-sim)

    target_compile_definitions(cpcraft-port-sim PRIVATE
        SIMULATOR_NATIVE
    )

    target_link_libraries(cpcraft-port-sim PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )

    set_target_properties(cpcraft-port-sim PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/out"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/out"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/out"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_BINARY_DIR}/out"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_BINARY_DIR}/out"
    )

    # vcpkg usually deploys runtime DLLs automatically. This explicit copy keeps
    # local non-vcpkg SDL2 installs convenient too when SDL2::SDL2 is a DLL target.
    if(WIN32 AND TARGET SDL2::SDL2)
        add_custom_command(TARGET cpcraft-port-sim POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL2::SDL2>"
                "$<TARGET_FILE_DIR:cpcraft-port-sim>"
            COMMAND_EXPAND_LISTS
        )
    endif()
endif()
