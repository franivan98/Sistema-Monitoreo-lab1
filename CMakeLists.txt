# Configuración del subproyecto de monitoreo
project(Monitor VERSION 1.0.0 LANGUAGES C)

# Incluir directorios
include_directories(include)

# Dependencias externas
find_library(PROM_LIB prom REQUIRED)
find_library(PROMHTTP_LIB promhttp REQUIRED)

# Archivos fuente
set(MONITOR_SOURCES
    src/expose_metrics.c
    src/metrics.c
    src/main.c
)

# Crear el ejecutable
add_executable(metrics ${MONITOR_SOURCES})

# Enlazar las librerías necesarias
target_link_libraries(metrics PRIVATE pthread ${PROM_LIB} ${PROMHTTP_LIB})
