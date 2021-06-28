# BLEEPemul is an user interface for the blockchain emulation
# Currently, emulation interface is developed for bitcoin emulation
if (ALL_OPT STREQUAL ON)
    add_subdirectory(bitcoin)
    add_subdirectory(BLEEPlib)

elseif (GIT_OPT STREQUAL ON)
    add_subdirectory(bitcoin)
    add_subdirectory(BLEEPlib)

elseif (BITCOIN_OPT STREQUAL ON)
    add_subdirectory(bitcoin)
    add_subdirectory(BLEEPlib)

elseif (MONERO_OPT STREQUAL ON)

else (ALL_OPT STREQUAL ON)
    add_subdirectory(bitcoin)
    add_subdirectory(BLEEPlib)

endif (ALL_OPT STREQUAL ON)
