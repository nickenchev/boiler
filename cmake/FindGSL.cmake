# Locate GSL library

FIND_PATH(GSL_INCLUDE_DIR gsl.h
	HINTS
    ${GSL}
    $ENV{GSL}
    PATH_SUFFIXES include/GSL include GSL 
    i686-w64-mingw32/include/GSL
    x86_64-w64-mingw32/include/GSL
	PATHS
    /usr/local/include/GSL
    /usr/include/GSL
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL REQUIRED_VARS GSL_INCLUDE_DIR)


