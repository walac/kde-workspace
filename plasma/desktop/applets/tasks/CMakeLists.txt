project(tasks)

set(tasks_SRCS tasks.cpp)
message(FATAL_ERROR ${KDEBASE_WORKSPACE_SOURCEDIR})

kde4_add_plugin(plasma_applet_org_kde_tasks ${tasks_SRCS})

include_directories(${KDEBASE_WORKSPACE_SOURCEDIR}/libs ${KDEBASE_WORKSPACE_SOURCEDIR}/libs/taskmanager)
target_link_libraries(plasma_applet_org_kde_tasks
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} plasmagenericshell)

install(TARGETS plasma_applet_org_kde_tasks
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-tasks.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/packages/org.kde.tasks)
