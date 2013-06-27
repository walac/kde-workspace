set(time_engine_SRCS
    timeengine.cpp
    timesource.cpp
    solarsystem.cpp
)

#kde4_add_plugin(plasma_engine_time ${time_engine_SRCS})
add_library(plasma_engine_time SHARED ${time_engine_SRCS})
target_link_libraries(plasma_engine_time
    ${KDE4_KDECORE_LIBS}
    ${Plasma_LIBRARIES}
)

install(TARGETS plasma_engine_time DESTINATION ${PLUGIN_INSTALL_DIR}/kplugins)
install(FILES plasma-dataengine-time.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
