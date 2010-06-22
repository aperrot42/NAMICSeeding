PROJECT(NAMICSEED)

# CMake >= 2.6 has to be installed (fighting for progress)
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MESSAGE( STATUS "NAMIC SEED" )

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings. Copied from ITK CMakeLists.
IF( WIN32 )
  IF( NOT BORLAND )
    IF( NOT CYGWIN )
      IF( NOT MINGW )
          ADD_DEFINITIONS(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
      ENDIF( NOT MINGW )
    ENDIF( NOT CYGWIN )
  ENDIF( NOT BORLAND )
ENDIF( WIN32 )


#--------------------------------------------------------------------------
# Define CMAKE_whatever_OUTPUT_DIRECTORY
#--------------------------------------------------------------------------
# all target executables will be
# in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${NAMICSEED_BINARY_DIR}/bin
)

# all target static libraries will be in ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${NAMICSEED_BINARY_DIR}/lib
)

# all shared libraries will be in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
# Note that modules (plugins) are considered as shared libraries
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
   ${NAMICSEED_BINARY_DIR}/lib
)


LINK_DIRECTORIES(
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  )


#--------------------------------------------------------------------------
# Define TESTING_DATA_PATH
#--------------------------------------------------------------------------
#SET( TESTING_DATA_PATH
#  ${NAMICSEED_SOURCE_DIR}/Testing/Data
#  CACHE PATH "Directory which contains data for testing."
#)



#--------------------------------------------------------------------------
# ITK
#--------------------------------------------------------------------------
FIND_PACKAGE( ITK )
IF( ITK_FOUND )
  INCLUDE( ${ITK_USE_FILE} )
ENDIF( ITK_FOUND )


#-------------------------------------------------------------------------
# PROJECT STRUCTURE
#-------------------------------------------------------------------------
INCLUDE_DIRECTORIES( BEFORE
  ${ITK_INCLUDES}
  ${NAMICSEED_SOURCE_DIR}/Code/
  ${NAMICSEED_BINARY_DIRECTORY}/Code/
  ${NAMICSEED_SOURCE_DIR}/Code/Filters/
  ${NAMICSEED_BINARY_DIRECTORY}/Code/Filters/
  )

ADD_SUBDIRECTORY( Code )



#-------------------------------------------------------------------------
# Examples and tests
#-------------------------------------------------------------------------
OPTION( BUILD_EXAMPLES "Build Examples" ON )

IF( BUILD_EXAMPLES )
  ADD_SUBDIRECTORY( Examples )
ENDIF( BUILD_EXAMPLES )


