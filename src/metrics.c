#include "metrics.h"

double get_memory_usage(unsigned long long* total_mem, unsigned long long* free_mem, unsigned long long* used_mem)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", free_mem) == 1)
        {
            break; // MemAvailable encontrado
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (*total_mem == 0 || *free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de memoria
    *used_mem = *total_mem - *free_mem;
    double mem_usage_percent = 0.0;
    mem_usage_percent = ((double)(*used_mem) / (double)(*total_mem)) * 100.0;
    return mem_usage_percent;
}

double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

int collect_diskstats(Diskstats* diskstats)
{
    // Abrir el archivo /proc/diskstats
    FILE* file = fopen("/proc/diskstats", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return 1;
    }
    // Leer el archivo linea por linea
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file))
    {
        long long reads, writes, total_time;
        char device[STRING_LENGHT];
        // Filtrar solo dispositivos como sda, sda1, etc.
        if (sscanf(buffer, "%*d %*d %s %lld %*d %lld %*d %*d %*d %*d %lld", device, &reads, &writes, &total_time) == 4)
        {
            if (strncmp(device, "sda", 3) == 0)
            {
                diskstats->reads = reads;
                diskstats->writes = writes;
                diskstats->total_time = total_time;
                return 0;
            }
        }
    }
    fclose(file);
    return 0;
}

int get_network_traffic(network_stats_t* network_stats)
{
    // Abrir el archivo /proc/net/dev
    FILE* file = fopen("/proc/net/dev", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return 1;
    }
    // Leer el archivo linea por linea
    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file))
    {
        // Comprobar si es un dispositivo de red
        if (strncmp(line, "wlp", 3) == 0)
        {
            sscanf(line, "%*s %lu %*d %*d %*d %*d %*d %*d %*d %lu", &network_stats->rx_bytes, &network_stats->tx_bytes);
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return -1;
}

int get_running_processes_and_context_switches(int* running_processes, int* context_switches)
{
    char buffer[BUFFER_SIZE];
    FILE* file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1;
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (sscanf(buffer, "ctxt %d", context_switches) == 1)
        {
            continue;
        }
        if (sscanf(buffer, "procs_running %d", running_processes) == 1)
        {
            break;
        }
    }
    fclose(file);
    return 0;
}
