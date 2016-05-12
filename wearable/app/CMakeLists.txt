CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(nfc-setting-app C)

SET(SRCS
src/nsa-main.c
src/nsa-popup.c
src/nsa-ui-widget.c
src/nsa-view-main.c
src/nsa-view-tap-n-pay.c
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

# include library +
SET(dependents "glib-2.0 elementary efl-extension pkgmgr-info dlog vconf capi-appfw-application capi-network-nfc")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_app REQUIRED
${dependents}
)
# include library -

FOREACH(flag ${pkgs_app_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -Wl,-rpath,${LIBDIR}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")
ADD_DEFINITIONS("-fpie")
MESSAGE("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_app_LDFLAGS} "-pie")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)

# install image
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/b_setting_payment_help_image.png DESTINATION ${IMGDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/tw_ic_popup_btn_check.png DESTINATION ${IMGDIR})

ADD_SUBDIRECTORY(data)
