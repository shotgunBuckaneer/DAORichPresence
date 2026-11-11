cmake_minimum_required(VERSION 3.21)

project(DAORichPresence VERSION 0.1.0 LANGUAGES CXX)

# access DAOLib but don't include it
add_subdirectory(extern/DAOLib EXCLUDE_FROM_ALL)

# Prebuilt Discord RPC, 32-bit dynamic library
set(DISCORD_RPC_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/discord-rpc/win32-dynamic"
)

add_library(discord_rpc SHARED IMPORTED GLOBAL)

set_target_properties(discord_rpc PROPERTIES
    IMPORTED_LOCATION
        "${DISCORD_RPC_ROOT}/bin/discord-rpc.dll"

    IMPORTED_IMPLIB
        "${DISCORD_RPC_ROOT}/lib/discord-rpc.lib"

    INTERFACE_INCLUDE_DIRECTORIES
        "${DISCORD_RPC_ROOT}/include"
)

# The actual plugin to be built
add_daolib_plugin(richpresence SOURCES src/richpresence.cpp)

#Link 'em
target_link_libraries(richpresence PRIVATE discord_rpc)

# Place Discord RPC DLL besides DAOrigins.exe in the build output.
add_custom_target(discord_rpc_runtime ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${DISCORD_RPC_ROOT}/bin/discord-rpc.dll"
        "${CMAKE_BINARY_DIR}/discord-rpc.dll"
    COMMENT "Copying Discord RPC runtime DLL"
    VERBATIM
)

add_dependencies(richpresence discord_rpc_runtime)