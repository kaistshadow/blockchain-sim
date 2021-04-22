# BLEEPemul is an user interface for the blockchain emulation
# Currently, emulation interface is developed for bitcoin emulation
if (ALL_OPT STREQUAL ON)
    add_subdirectory(emulation)

elseif (GIT_OPT STREQUAL ON)
    add_subdirectory(emulation)

elseif (BITCOIN_OPT STREQUAL ON)
    add_subdirectory(emulation)

elseif (MONERO_OPT STREQUAL ON)

else (ALL_OPT STREQUAL ON)
    add_subdirectory(emulation)

endif (ALL_OPT STREQUAL ON)
