/**
 * @file main.c
 * @brief Punto de entrada del sistema para exponer métricas del sistema.
 *
 * Este programa inicializa las métricas del sistema, crea un hilo para exponer
 * las métricas a través de un servidor HTTP, y actualiza las métricas cada segundo.
 */
#include "expose_metrics.h"
#include <stdbool.h>

/**
 * @brief Tiempo de sleep entre actualizaciones de las métricas.
 *
 * El tiempo de sleep entre actualizaciones de las métricas es de 1 segundo.
 */
#define SLEEP_TIME 1

/**
 * @brief Función principal del programa.
 *
 * Inicializa las métricas, crea un hilo para exponer métricas vía HTTP, y
 * entra en un bucle para actualizar las métricas periódicamente.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Arreglo de argumentos de línea de comandos.
 * @return EXIT_SUCCESS si el programa se ejecuta correctamente; EXIT_FAILURE en caso contrario.
 */
int main(int argc, char* argv[])
{
    // Inicializamos las métricas
    init_metrics();
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Bucle principal para actualizar las métricas cada segundo
    while (true)
    {
        update_cpu_gauge();
        update_memory_gauge();
        update_diskstats_gauge();
        update_network_gauge();
        update_running_processes_add_context_gauge();

        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}
