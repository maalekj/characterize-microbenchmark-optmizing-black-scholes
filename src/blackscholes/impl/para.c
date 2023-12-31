/* para.c
 *
 * Author:
 * Date  :
 *
 *  Description
 */

/* Standard C includes */
#include <stdlib.h>

/* Include common headers */
#include "common/macros.h"
#include "common/types.h"

/* Include application-specific headers */
#include "include/types.h"
#include "scalar.h"


typedef struct singleThread {
    int id;
    int data_size;
    args_t *data;
} singleThread;

void *blackScholesCaller(void *args) {
    singleThread *thread_args = (singleThread *) args;
    int id = thread_args->id;
    int data_size = thread_args->data_size;
    args_t *parsed_args = thread_args->data;

    int startingIndex = id * data_size;
    int endIndex = startingIndex + data_size;

    for (int i = startingIndex; i < endIndex; i++) {
        parsed_args->output[i] = blackScholes(parsed_args->sptPrice[i], parsed_args->strike[i], parsed_args->rate[i],
                                              parsed_args->volatility[i],
                                              parsed_args->otime[i], (parsed_args->otype[i] > 67 ? 1 : 0), 0);
    }
}

/* Alternative Implementation */
void *impl_parallel(void *args) {
    /* Get the argument struct */
    args_t *parsed_args = (args_t *) args;
    int num_threads = parsed_args->nthreads;
    size_t dataset_size = parsed_args->num_stocks;
    pthread_t thread[num_threads];
    singleThread thread_args[num_threads];

    for (int i = 0; i < num_threads; i++) {
        // Initialize the argument struture
        thread_args[i].id = i;
        thread_args[i].data_size = dataset_size / num_threads;
        thread_args[i].data = parsed_args;
        pthread_create(&thread[i], NULL, blackScholesCaller, (void *) &thread_args[i]);
    }

    //calculate the rest of the data
    for (int i = 0; i < dataset_size % num_threads; i++) {
        int originIndex = dataset_size - dataset_size % num_threads;
        parsed_args->output[originIndex + i] = blackScholes(parsed_args->sptPrice[originIndex + i],
                                                            parsed_args->strike[originIndex + i],
                                                            parsed_args->rate[originIndex + i],
                                                            parsed_args->volatility[originIndex + i],
                                                            parsed_args->otime[originIndex + i],
                                                            (parsed_args->otype[originIndex + i] > 67 ? 1 : 0), 0);
    }

    //wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(thread[i], NULL);
    }

}
