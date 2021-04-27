include(GNUInstallDirs)

add_compile_options(
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++2a>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++17>)

add_subdirectory(shadow_interface)
add_subdirectory(shadow_bitcoin_interface)
add_subdirectory(shadow_memshare_interface)