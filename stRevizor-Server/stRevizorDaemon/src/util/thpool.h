/**********************************
 * @author      Johan Hanssen Seferidis
 * License:     MIT
 *
 **********************************/

#ifndef _THPOOL_
#define _THPOOL_

#ifdef __cplusplus
extern "C" {
#endif

/* =================================== API ======================================= */


typedef struct thpool_* threadpool_t;


/**
 * @brief  Initialize threadpool_t
 *
 * Initializes a threadpool_t. This function will not return untill all
 * threads have initialized successfully.
 *
 * @example
 *
 *    ..
 *    threadpool_t thpool;                     //First we declare a threadpool_t
 *    thpool = thpool_init(4, "My poll");               //then we initialize it to 4 threads
 *    ..
 *
 * @param  num_threads   number of threads to be created in the threadpool_t
 * @return threadpool_t    created threadpool_t on success,
 *                       NULL on error
 */
threadpool_t thpool_init(int, char*);


/**
 * @brief Add work to the job queue
 *
 * Takes an action and its argument and adds it to the threadpool_t's job queue.
 * If you want to add to work a function with more than one arguments then
 * a way to implement this is by passing a pointer to a structure.
 *
 * NOTICE: You have to cast both the function and argument to not get warnings.
 *
 * @example
 *
 *    void print_num(int num){
 *       printf("%d\n", num);
 *    }
 *
 *    int main() {
 *       ..
 *       int a = 10;
 *       thpool_add_work(thpool, (void*)print_num, (void*)a);
 *       ..
 *    }
 *
 * @param  threadpool_t    threadpool_t to which the work will be added
 * @param  function_p    pointer to function to add as work
 * @param  arg_p         pointer to an argument
 * @return 0 on successs, -1 otherwise.
 */
int thpool_add_work(threadpool_t, net_cb, void* arg_p);


/**
 * @brief Wait for all queued jobs to finish
 *
 * Will wait for all jobs - both queued and currently running to finish.
 * Once the queue is empty and all work has completed, the calling thread
 * (probably the main program) will continue.
 *
 * Smart polling is used in wait. The polling is initially 0 - meaning that
 * there is virtually no polling at all. If after 1 seconds the threads
 * haven't finished, the polling interval starts growing exponentially
 * untill it reaches max_secs seconds. Then it jumps down to a maximum polling
 * interval assuming that heavy processing is being used in the threadpool_t.
 *
 * @example
 *
 *    ..
 *    threadpool_t thpool = thpool_init(4);
 *    ..
 *    // Add a bunch of work
 *    ..
 *    thpool_wait(thpool);
 *    puts("All added work has finished");
 *    ..
 *
 * @param threadpool_t     the threadpool_t to wait for
 * @return nothing
 */
void thpool_wait(threadpool_t);


/**
 * @brief Pauses all threads immediately
 *
 * The threads will be paused no matter if they are idle or working.
 * The threads return to their previous states once thpool_resume
 * is called.
 *
 * While the thread is being paused, new work can be added.
 *
 * @example
 *
 *    threadpool_t thpool = thpool_init(4);
 *    thpool_pause(thpool);
 *    ..
 *    // Add a bunch of work
 *    ..
 *    thpool_resume(thpool); // Let the threads start their magic
 *
 * @param threadpool_t    the threadpool_t where the threads should be paused
 * @return nothing
 */
void thpool_pause(threadpool_t);


/**
 * @brief Unpauses all threads if they are paused
 *
 * @example
 *    ..
 *    thpool_pause(thpool);
 *    sleep(10);              // Delay execution 10 seconds
 *    thpool_resume(thpool);
 *    ..
 *
 * @param threadpool_t     the threadpool_t where the threads should be unpaused
 * @return nothing
 */
void thpool_resume(threadpool_t);


/**
 * @brief Destroy the threadpool_t
 *
 * This will wait for the currently active threads to finish and then 'kill'
 * the whole threadpool_t to free up memory.
 *
 * @example
 * int main() {
 *    threadpool_t thpool1 = thpool_init(2);
 *    threadpool_t thpool2 = thpool_init(2);
 *    ..
 *    thpool_destroy(thpool1);
 *    ..
 *    return 0;
 * }
 *
 * @param threadpool_t     the threadpool_t to destroy
 * @return nothing
 */
void thpool_destroy(threadpool_t);


/**
 * @brief Show currently working threads
 *
 * Working threads are the threads that are performing work (not idle).
 *
 * @example
 * int main() {
 *    threadpool_t thpool1 = thpool_init(2);
 *    threadpool_t thpool2 = thpool_init(2);
 *    ..
 *    printf("Working threads: %d\n", thpool_num_threads_working(thpool1));
 *    ..
 *    return 0;
 * }
 *
 * @param threadpool_t     the threadpool_t of interest
 * @return integer       number of threads working
 */
int thpool_num_threads_working(threadpool_t);


#ifdef __cplusplus
}
#endif

#endif
