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
    int sleep_time = 1;
    bool cpu_enabled = true, memory_enabled = true, diskstats_enabled = true, network_enabled = true;
    //procesamos los argumentos
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--interval")==0 && i+1 < argc){
            sleep_time = atoi(argv[++i]);
        }
        else{
            if(strcmp(argv[i],"--metrics")==0 && i+1 < argc){
                cpu_enabled=memory_enabled=diskstats_enabled=network_enabled=false;

                char* metrics=argv[++i];
                if(strstr(metrics,"cpu")) cpu_enabled=true;
                if(strstr(metrics,"memory")) memory_enabled=true;
                if(strstr(metrics,"diskstats")) diskstats_enabled=true;
                if(strstr(metrics,"network")) network_enabled=true;
            }
            else{
                perror("Error al procesar los argumentos.");
                return EXIT_FAILURE;
            }
        }
    }
    printf("Intervalo de muestreo: %d segundos\n", sleep_time);
    printf("Métricas habilitadas:\n");
    if (cpu_enabled) printf("  - CPU\n");
    if (memory_enabled) printf("  - Memoria\n");
    if (diskstats_enabled) printf("  - Disco\n");
    if (network_enabled) printf("  - Red\n");
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
        if(cpu_enabled){
            update_cpu_gauge();
        }
        if(memory_enabled){
            update_memory_gauge();
        }
        if(diskstats_enabled){
            update_diskstats_gauge();
        }
        if(network_enabled){
            update_network_gauge();
        }
        update_running_processes_add_context_gauge();

        sleep(sleep_time);
    }

    return EXIT_SUCCESS;
}
 