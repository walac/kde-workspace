#######################################
# Effect

# Source files
set( kwin4_effect_builtins_sources ${kwin4_effect_builtins_sources}
    lookingglass/lookingglass.cpp
    )

kde4_add_kcfg_files(kwin4_effect_builtins_sources lookingglass/lookingglassconfig.kcfgc)

# .desktop files
install( FILES
    lookingglass/lookingglass.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}/kwin )

# Data files
install( FILES
    lookingglass/data/1.10/lookingglass.frag
    DESTINATION ${DATA_INSTALL_DIR}/kwin/shaders/1.10 )
install( FILES
    lookingglass/data/1.40/lookingglass.frag
    DESTINATION ${DATA_INSTALL_DIR}/kwin/shaders/1.40 )

#######################################
# Config

# Source files
set( kwin4_effect_builtins_config_sources ${kwin4_effect_builtins_config_sources}
    lookingglass/lookingglass_config.cpp
    lookingglass/lookingglass_config.ui
    )

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources lookingglass/lookingglassconfig.kcfgc)

# .desktop files
install( FILES
    lookingglass/lookingglass_config.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}/kwin )
