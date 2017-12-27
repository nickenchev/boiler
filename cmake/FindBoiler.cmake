FIND_PATH(BOILER_INCLUDE_DIR boiler.h
    HINTS
    /Users/nenchev/Developer/projects/boiler/include
    /home/nenchev/Developer/projects/boiler/include)

FIND_LIBRARY(BOILER_LIBRARY libboiler.a
    HINTS
    /Users/nenchev/Developer/projects/boiler/build
    /home/nenchev/Developer/projects/boiler/build)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Boiler REQUIRED_VARS BOILER_LIBRARY BOILER_INCLUDE_DIR)
