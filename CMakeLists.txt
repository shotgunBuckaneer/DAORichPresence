cmake_minimum_required(VERSION 3.21)

project(DAORichPresence VERSION 0.9.1 LANGUAGES CXX)

# access DAOLib but don't include it
add_subdirectory(extern/DAOLib EXCLUDE_FROM_ALL)

# Prebuilt Discord RPC, 32-bit dynamic library
set(DISCORD_RPC_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/discord-rpc/win32-dynamic"
)

# Discord's library
add_library(discord_rpc SHARED IMPORTED GLOBAL)

# Discord lib config
set_target_properties(discord_rpc PROPERTIES
    IMPORTED_LOCATION
        "${DISCORD_RPC_ROOT}/bin/discord-rpc.dll"

    IMPORTED_IMPLIB
        "${DISCORD_RPC_ROOT}/lib/discord-rpc.lib"

    INTERFACE_INCLUDE_DIRECTORIES
        "${DISCORD_RPC_ROOT}/include"
)

# Versioning configuration
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/version.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    @ONLY
)

# The actual plugin to be built
add_daolib_plugin(richpresence SOURCES 
	src/richpresence.cpp
	src/heroicStats.cpp
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	)

# Link 'em
target_link_libraries(richpresence PRIVATE discord_rpc)

# Ensure we can check for Discord RPC presence BEFORE trying to load it 
target_link_options(richpresence PRIVATE 
					"/DELAYLOAD:discord-rpc.dll"
)

# Link delay loader helper
target_link_libraries(richpresence PRIVATE delayimp)

# Place Discord RPC DLL besides DAOrigins.exe in the build output.
add_custom_target(discord_rpc_runtime ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${DISCORD_RPC_ROOT}/bin/discord-rpc.dll"
        "${CMAKE_BINARY_DIR}/discord-rpc.dll"
    COMMENT "Copying Discord RPC runtime DLL"
    VERBATIM
)

add_dependencies(richpresence discord_rpc_runtime)