#include "expose_metrics.h"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria */
static prom_gauge_t* memory_usage_metric;

/** Metrica de Prometheus para la memoria total */
static prom_gauge_t* total_memory_metric;

/** Metrica de Prometheus para la memoria disponible */
static prom_gauge_t* free_memory_metric;

/** Metrica de Prometheus para la memoria usada */
static prom_gauge_t* used_memory_metric;

/** Metrica de Prometheus para las lecturas de disco */
static prom_gauge_t* reads_gauge;

/** Metrica de Prometheus para las escrituras de disco*/
static prom_gauge_t* writes_gauge;

/** Metrica de Prometheus para el tiempo total de disco activo */
static prom_gauge_t* total_time_gauge;

/** Metrica de Prometheus transmision de red */
static prom_gauge_t* network_tx_metric;

/** Metrica de Prometheus recepcion de red */
static prom_gauge_t* network_rx_metric;

/** Metrica de Prometheus procesos en ejecución */
static prom_gauge_t* running_processes_metric;

/** Metrica de Prometheus cambios de contexto */
static prom_gauge_t* context_switches_metric;

void update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
}

void update_memory_gauge()
{
    unsigned long long total_mem = 0, free_mem = 0, used_mem = 0;
    double usage_mem = get_memory_usage(&total_mem, &free_mem, &used_mem);
    if (usage_mem >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage_mem, NULL);
        prom_gauge_set(total_memory_metric, total_mem, NULL);
        prom_gauge_set(free_memory_metric, free_mem, NULL);
        prom_gauge_set(used_memory_metric, used_mem, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

void update_diskstats_gauge()
{
    Diskstats diskstats;
    int control_disk = collect_diskstats(&diskstats);
    if (control_disk == 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(reads_gauge, diskstats.reads, NULL);
        prom_gauge_set(writes_gauge, diskstats.writes, NULL);
        prom_gauge_set(total_time_gauge, diskstats.total_time, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de disco\n");
    }
}

void update_network_gauge()
{
    network_stats_t network_stats;
    int control_net = get_network_traffic(&network_stats);
    if (control_net == 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(network_tx_metric, network_stats.tx_bytes, NULL);
        prom_gauge_set(network_rx_metric, network_stats.rx_bytes, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de red\n");
    }
}
void update_running_processes_add_context_gauge()
{
    int running_processes = 0, context_switches = 0;
    int control = get_running_processes_and_context_switches(&running_processes, &context_switches);
    if (control == 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, context_switches, NULL);
        prom_gauge_set(running_processes_metric, running_processes, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el cambio de contextos\n");
    }
}
void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

void init_metrics()
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return;
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
        return;
    }

    // Creamos la métrica para el uso de CPU
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
        return;
    }

    // Creamos la métrica para el uso de memoria
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
        return;
    }

    // creamos la metrica para la memoria total
    total_memory_metric = prom_gauge_new("total_memory", "Memoria total", 0, NULL);
    if (total_memory_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de la memoria total\n");
        return;
    }

    // creamos la metrica para la memoria disponible
    free_memory_metric = prom_gauge_new("free_memory", "Memoria disponible", 0, NULL);
    if (free_memory_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de la memoria disponible\n");
        return;
    }

    // creamos la metrica para la memoria usada
    used_memory_metric = prom_gauge_new("used_memory", "Memoria usada", 0, NULL);
    if (used_memory_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de la memoria usada\n");
        return;
    }

    // creamos la metrica para las lecturas
    reads_gauge = prom_gauge_new("reads_sda", "Lecturas", 0, NULL);
    if (reads_gauge == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de las lecturas\n");
        return;
    }

    // creamos la metrica para las escrituras
    writes_gauge = prom_gauge_new("writes_sda", "Escrituras", 0, NULL);
    if (writes_gauge == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de las escrituras\n");
        return;
    }

    // creamos la metrica para el tiempo total
    total_time_gauge = prom_gauge_new("total_time_sda", "Tiempo total", 0, NULL);
    if (total_time_gauge == NULL)
    {
        fprintf(stderr, "Error al crear la metrica del tiempo total\n");
        return;
    }

    // creamos la metrica para recepcion de datos
    network_rx_metric = prom_gauge_new("network_rx_bytes", "Bytes recibidos", 0, NULL);
    if (network_rx_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de recepción de datos\n");
        return;
    }

    // creamos la metrica para envío de datos
    network_tx_metric = prom_gauge_new("network_tx_bytes", "Bytes enviados", 0, NULL);
    if (network_tx_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de envío de datos\n");
        return;
    }

    // creamos la metrica para procesos corriendo
    running_processes_metric = prom_gauge_new("running_processes", "Procesos corriendo", 0, NULL);
    if (running_processes_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de procesos corriendo\n");
        return;
    }
    // creamos la metrica para contextos de switches
    context_switches_metric = prom_gauge_new("context_switches", "Contextos de switches", 0, NULL);
    if (context_switches_metric == NULL)
    {
        fprintf(stderr, "Error al crear la metrica de contextos de switches\n");
        return;
    }

    register_metrics();
}
void register_metrics()
{
    if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la métrica de uso de CPU\n");
        return;
    }

    if (prom_collector_registry_must_register_metric(memory_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la métrica de uso de memoria\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(total_memory_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de la memoria total\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(free_memory_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de la memoria disponible\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(used_memory_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de la memoria usada\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(reads_gauge) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de las lecturas\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(writes_gauge) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de las escrituras\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(total_time_gauge) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica del tiempo total\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(network_rx_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de recepción de datos\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(network_tx_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de envío de datos\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(running_processes_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de procesos corriendo\n");
        return;
    }
    if (prom_collector_registry_must_register_metric(context_switches_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar la metrica de contextos de switches\n");
        return;
    }
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
