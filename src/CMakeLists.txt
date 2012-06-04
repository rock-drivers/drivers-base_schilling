rock_library(base_schilling
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(base_schilling_bin Main.cpp
    DEPS base_schilling)

