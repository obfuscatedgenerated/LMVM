# check if installers are enabled
if (INSTALLER STREQUAL OFF)
    MESSAGE(STATUS "Installer disabled, skipping installer generation")
    return()
endif()

include(scripts/cmake_modules/DetectArch.cmake)
target_architecture(ARCH)

# use nsis by default if windows, otherwise use zip. always use tar.gz on unix
# user can override the choice by passing the -G flag to cpack, e.g. -G ZIP to force zip
IF(WIN32)
    include(scripts/cmake_modules/FindNSIS.cmake)
    if (NSIS_FOUND)
        MESSAGE(STATUS "NSIS found")
        set(CPACK_GENERATOR NSIS)
    else()
        MESSAGE(STATUS "NSIS not found, using ZIP")
        set(CPACK_GENERATOR ZIP)
    endif()
ELSE()
    MESSAGE(STATUS "Not windows, using tar.gz")
    set(CPACK_GENERATOR TGZ)
ENDIF()

# use pandoc to convert readme
find_program(PANDOC pandoc)

if (PANDOC)
    MESSAGE(STATUS "Pandoc found")

    # make html on readme changes
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/README.html
            COMMAND ${PANDOC} -s -o ${CMAKE_CURRENT_BINARY_DIR}/README.html ${CMAKE_CURRENT_SOURCE_DIR}/README.md
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    )
    # additionally, run it now
    execute_process(COMMAND ${PANDOC} -s -o ${CMAKE_CURRENT_BINARY_DIR}/README.html ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
else()
    MESSAGE(FATAL_ERROR "Pandoc not found, refusing to continue. Install pandoc or disable installers by passing -DINSTALLER=OFF to cmake.")
endif()

# add install target
install(TARGETS LMVM LMASM DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/README.html DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION . RENAME LICENSE.txt)

# use custom nsis template
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/scripts/nsis_template")

# set package details
set(CPACK_PACKAGE_NAME "Little Man Virtual Machine")
set(CPACK_PACKAGE_VENDOR "obfuscatedgenerated")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://ollieg.codes/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_VERSION "v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images\\\\package_icon.bmp")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_EXECUTABLES "LMVM;LMVM" "LMASM;LMASM")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LMVM")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_MENU_LINKS
        "https://ollieg.codes/;Ollie's Website"
        "/LICENSE.txt;Review License"
        "/README.md;README (markdown)"
        "/README.html;README (html)"
        "Uninstall.exe;Uninstall LMVM"
        )
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_URL_INFO_ABOUT "https://ollieg.codes/")

set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/installer_icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/images/uninstaller_icon.ico")

set(CPACK_NSIS_MUI_HEADERIMAGE_BITMAP "${CMAKE_CURRENT_SOURCE_DIR}/images\\\\installer_header.bmp")
set(CPACK_NSIS_MUI_HEADERIMAGE_UNBITMAP "${CMAKE_CURRENT_SOURCE_DIR}/images\\\\uninstaller_header.bmp")

set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\LMVM.exe")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/installers")
string(REGEX REPLACE " " "_" FIXED_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_FILE_NAME ${FIXED_NAME}-${CMAKE_BUILD_TYPE}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCH})
set(CPACK_NSIS_BRANDING_TEXT "LMVM - Little Man Virtual Machine")
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_CURRENT_SOURCE_DIR}/images\\\\installer_welcome.bmp")
set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${CMAKE_CURRENT_SOURCE_DIR}/images\\\\uninstaller_welcome.bmp")
set(CPACK_NSIS_WELCOME_TITLE "Welcome to LMVM's installer...")
set(CPACK_NSIS_FINISH_TITLE "LMVM is ready to go!")

# get current source dir with backslashes
string(REPLACE "/" "\\\\" CMAKE_SOURCE_WINDIR ${CMAKE_SOURCE_DIR})

# associate file extensions
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "!include ${CMAKE_SOURCE_WINDIR}\\scripts\\nsis_modules\\FileAssociation.nsh")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "!include ${CMAKE_SOURCE_WINDIR}\\scripts\\reg_associations.nsh")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "!include ${CMAKE_SOURCE_WINDIR}\\scripts\\unreg_associations.nsh")

include(CPack)

