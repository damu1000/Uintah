#ifndef custom_thread_h
#define custom_thread_h

//#define USE_FUNNELLED_COMM
//#define USE_MPI_EP

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */

#include<functional>
#include <omp.h>

void custom_partition_master(int num_partitions, int threads_per_partition, int *affinity, int g_nodal_rank, std::function<void(int)> f);

void custom_parallel_for(int b, int e, std::function<void(int)> f, int active_threads);

extern "C++" {
#endif

extern thread_local int cust_g_team_id, cust_g_thread_id;
extern int g_rank_temp;

int get_custom_team_id();
int get_custom_thread_id();
int get_team_size();

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
