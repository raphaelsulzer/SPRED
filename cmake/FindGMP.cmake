set(GMP_PREFIX "" CACHE PATH "path ")


find_path(GMP_INCLUDE_DIR gmp.h gmpxx.h
    PATHS ${GMP_PREFIX}/include /usr/include /usr/local/include )

find_library(GMP_LIB NAMES gmp libgmp
    PATHS ${GMP_PREFIX}/lib /usr/lib /usr/local/lib)

find_library(GMPXX_LIB NAMES gmpxx libgmpxx
    PATHS ${GMP_PREFIX}/lib /usr/lib /usr/local/lib)



if(GMP_INCLUDE_DIR AND GMP_LIB AND GMPXX_LIB)
    #get_filename_component(GMP_LIBRARY_DIR ${GMP_LIBRARY} PATH)
    set(GMP_FOUND TRUE)
    set(GMP_LIBRARY ${GMP_LIB} ${GMPXX_LIB})
endif()

if(GMP_FOUND)
   if(NOT GMP_FIND_QUIETLY)
      MESSAGE(STATUS "Found GMP: ${GMP_LIBRARY}")
   endif()
elseif(GMP_FOUND)
   if(GMP_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GMP")
   endif()
endif()

