/**
 * @file expose_metrics.h
 * @brief Programa para leer el uso de CPU y memoria, estadisticas de disco y red, y mostrarlas en un servidor HTTP.
 */

#include "metrics.h"
#include <errno.h>
#include <prom.h>
#include <promhttp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para sleep
/**
 * @brief Tamaño del buffer
 * Tamaño del buffer para leer los archivos
 */
#define BUFFER_SIZE 256

/**
 * @brief Actualiza la métrica de uso de CPU.
 *
 * Esta función lee el uso actual de CPU y actualiza la métrica correspondiente en
 * el servidor HTTP. Puede ser llamada periódicamente para mantener las estadísticas
 * actualizadas.
 *
 * @return void
 */
void update_cpu_gauge();

/**
 * @brief Actualiza la métrica de uso de memoria.
 *
 * Esta función lee el uso actual de memoria y actualiza la métrica correspondiente
 * en el servidor HTTP.
 *
 * @return void
 */
void update_memory_gauge();

/**
 * @brief Actualiza la métrica de uso de disco.
 *
 * Esta función lee las estadísticas de uso del disco y actualiza la métrica
 * correspondiente en el servidor HTTP.
 *
 * @return void
 */
void update_diskstats_gauge();

/**
 * @brief Actualiza la métrica de uso de red.
 *
 * Esta función lee las estadísticas de tráfico de red y actualiza la métrica
 * correspondiente en el servidor HTTP.
 *
 * @return void
 */
void update_network_gauge();

/**
 * @brief Actualiza la métrica de conteo de procesos y cambios de contexto.
 *
 * Esta función lee el número de procesos en ejecución y el conteo de cambios de
 * contexto, y actualiza las métricas correspondientes en el servidor HTTP.
 *
 * @return void
 */
void update_running_processes_add_context_gauge();

/**
 * @brief Función del hilo para exponer las métricas vía HTTP en el puerto 8000.
 *
 * Esta función se ejecuta en un hilo separado y se encarga de iniciar el servidor
 * HTTP que expone las métricas recopiladas.
 *
 * @param arg Argumento no utilizado.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Inicializa mutex y métricas.
 *
 * Esta función inicializa los mutex utilizados para la protección de acceso concurrente
 * a las métricas.
 *
 * @return void
 */
void init_metrics();

/**
 * @brief Registra todas las métricas.
 *
 * Esta función registra todas las métricas en el sistema para que puedan ser expuestas
 * a través del servidor HTTP.
 *
 * @return void
 */
void register_metrics();

/**
 * @brief Destructor de mutex.
 *
 * Esta función libera los recursos asociados con los mutex utilizados para la protección
 * de acceso concurrente a las métricas.
 *
 * @return void
 */
void destroy_mutex();
