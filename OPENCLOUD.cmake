set( APPLICATION_NAME       "copyparty Desktop")
set( APPLICATION_SHORTNAME  "copyparty" )
set( APPLICATION_EXECUTABLE "copyparty-desktop" )
set( APPLICATION_VENDOR     "copyparty" )
set( APPLICATION_ICON_NAME  "copyparty" )
set( APPLICATION_REV_DOMAIN "com.copyparty.desktop" )

if(BETA_CHANNEL_BUILD)
    set( APPLICATION_NAME       "${APPLICATION_NAME} Beta")
    set( APPLICATION_SHORTNAME  "${APPLICATION_SHORTNAME} Beta" )
    set( APPLICATION_EXECUTABLE "${APPLICATION_EXECUTABLE}_beta" )
    set( APPLICATION_VENDOR     "${APPLICATION_VENDOR} Beta" )
    set( APPLICATION_REV_DOMAIN "${APPLICATION_REV_DOMAIN}.beta" )
endif()
# TODO: re enable once we got icons
#set( MAC_INSTALLER_BACKGROUND_FILE "${CMAKE_SOURCE_DIR}/admin/osx/installer-background.png")

set( THEME_CLASS            "CopypartyTheme" )

set( THEME_INCLUDE          "copypartytheme.h" )

option( WITH_CRASHREPORTER "Build crashreporter" OFF )
