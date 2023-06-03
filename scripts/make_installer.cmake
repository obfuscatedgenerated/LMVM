include(scripts/cmake_modules/DetectArch.cmake)
target_architecture(ARCH)

# use findnsis
include(scripts/cmake_modules/FindNSIS.cmake)
if (NSIS_FOUND)
    MESSAGE(STATUS "NSIS found")
    set(CPACK_GENERATOR NSIS)
else()
    MESSAGE(STATUS "NSIS not found, using ZIP")
    set(CPACK_GENERATOR ZIP)
endif()

# add install target
install(TARGETS LMVM LMASM DESTINATION bin)

# TODO: pandoc conversion
# actually, cmake has some built in conversion, but its not very stylish

# set package details
set(CPACK_PACKAGE_NAME "Little Man Virtual Machine")
set(CPACK_PACKAGE_VENDOR "Ollie (obfuscatedgenerated)")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://ollieg.codes/")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LMVM - Little Man Virtual Machine")
# TODO: use git
#set(CPACK_PACKAGE_VERSION ${GIT_TAG})
#set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
#set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
#set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
#set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.bmp")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT__SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_EXECUTABLES "LMVM;LMVM" "LMASM;LMASM")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LMVM")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_MENU_LINKS
        "https://ollieg.codes/;Ollie's Website"
        "/LICENSE.txt;Review License"
        "/README.md;README (markdown)"
        "Uninstall.exe;Uninstall LMVM"
        )
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_URL_INFO_ABOUT "https://ollieg.codes/")
#set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/main.ico")
#set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/main.ico")
#set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\projectify.exe")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/installers")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CMAKE_BUILD_TYPE}-${CPACK_PACKAGE_VERSION}-${ARCH})
set(CPACK_NSIS_BRANDING_TEXT "LMVM - Little Man Virtual Machine")
#set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_CURRENT_SOURCE_DIR}/assets\\icon.bmp")
#set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${CMAKE_CURRENT_SOURCE_DIR}/assets\\icon.bmp")
set(CPACK_NSIS_WELCOME_TITLE "Welcome to LMVM's installer...")
set(CPACK_NSIS_FINISH_TITLE "LMVM is ready to go!")

# get current source dir with backslashes
string(REPLACE "/" "\\\\" CMAKE_SOURCE_WINDIR ${CMAKE_SOURCE_DIR})

# associate file extensions
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS
   "!include ${CMAKE_SOURCE_WINDIR}\\scripts\\nsis_modules\\FileAssociation.nsh")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
        "!include ${CMAKE_SOURCE_WINDIR}\\scripts\\reg_associations.nsh")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
        "!include ${CMAKE_SOURCE_WINDIR}\\scripts\\unreg_associations.nsh")

include(CPack)

