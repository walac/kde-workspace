########### next target ###############

set(kwin_killer_helper_SRCS killer.cpp )


kde4_add_executable(kwin_killer_helper ${kwin_killer_helper_SRCS})

target_link_libraries(kwin_killer_helper ${KDE4_KDEUI_LIBS} ${X11_X11_LIB})

install(TARGETS kwin_killer_helper DESTINATION ${LIBEXEC_INSTALL_DIR} )
