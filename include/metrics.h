/**
 * @file metrics.h
 * @brief Funciones para obtener las metricas de uso de CPU y memoria, estadisticas de disco y red a traves de los
 * archivos /proc/meminfo, /proc/stat y /proc/diskstats.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del buffer.
 *
 * Tamaño del buffer para leer los archivos.
 */
#define BUFFER_SIZE 256

/**
 * @brief Tamaño de un string.
 *
 * Tamaño de un string para leer los archivos.
 */
#define STRING_LENGHT 20

/**
 * @brief Estructura para almacenar las estadísticas de disco.
 */
typedef struct
{
    char device[20];      /**< Nombre del dispositivo */
    long long reads;      /**< Lecturas completadas */
    long long writes;     /**< Escrituras completadas */
    long long total_time; /**< Tiempo total en milisegundos */
} Diskstats;

/**
 * @brief Estructura para almacenar las estadísticas de red.
 */
typedef struct
{
    unsigned long rx_bytes; /**< Bytes recibidos */
    unsigned long tx_bytes; /**< Bytes transmitidos */
} network_stats_t;

/**
 * @brief Obtiene el porcentaje de uso de memoria desde /proc/meminfo.
 *
 * Lee los valores de memoria total y disponible desde /proc/meminfo y calcula
 * el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_usage();

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage();

/**
 * @brief Obtiene las estadísticas de disco desde /proc/diskstats.
 *
 * Lee las estadísticas de disco desde /proc/diskstats y actualiza las estructuras
 * Diskstats. Luego, calcula las estadísticas de uso de disco en un intervalo de tiempo.
 *
 * @param diskstats Estructura para almacenar las estadísticas de disco.
 *
 * @return 0 en caso de éxito, -1 en caso de error.
 */
int collect_diskstats(Diskstats* diskstats);

/**
 * @brief Obtiene las estadísticas de red desde /proc/net/dev.
 *
 * Lee las estadísticas de red desde /proc/net/dev y actualiza las estructuras
 * network_stats_t.
 *
 * @param network_stats Estructura para almacenar las estadísticas de red.
 *
 * @return 0 en caso de éxito, -1 en caso de error.
 */
int get_network_traffic(network_stats_t* network_stats);

/**
 * @brief Obtiene el conteo de procesos y cambios de contexto desde /proc/stat.
 *
 * Lee el conteo de procesos y cambios de contexto desde /proc/stat y actualiza
 * las variables running_processes y context_switches.
 *
 * @param running_processes Puntero a la variable para almacenar el conteo de procesos.
 * @param context_switches Puntero a la variable para almacenar el conteo de cambios de contexto.
 *
 * @return 0 en caso de éxito, -1 en caso de error.
 */
int get_running_processes_and_context_switches(int* running_processes, int* context_switches);
