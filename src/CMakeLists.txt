rock_library(base_schilling
    SOURCES Driver.cpp
    HEADERS Driver.hpp SchillingRaw.hpp
    DEPS_PKGCONFIG base-types iodrivers_base base-lib)
    
#add_definitions(-DBASE_LOG_DISABLE)



