/*
 * SylixOS(TM)  LW : long wing
 * Copyright All Rights Reserved
 *
 * MODULE PRIVATE DYNAMIC MEMORY IN VPROCESS PATCH
 * this file is support current module C++ global object construction safety in a vprocess.
 *
 * Author: Han.hui <sylixos@gmail.com>
 */

#include <SylixOS.h>
#include <pthread.h>

/*
 *  cxa atexit node
 */
typedef struct cxa_exit_node {
    struct cxa_exit_node  *prev;    /* prev cxa atexit node */
    struct cxa_exit_node  *next;    /* next cxa atexit node */
    void  (*func)(void *arg);       /* cxa atexit function */
    void   *arg;                    /* cxa atexit function argument */
    void   *handle;                 /* cxa atexit function handle */
} cxa_exit_node;

/*
 *  cpp runtime lock
 */
static pthread_mutex_t  cpp_rt_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 *  cpp runtime cond
 */
static pthread_cond_t   cpp_rt_cond = PTHREAD_COND_INITIALIZER;

/*
 *  cxa atexit node list
 */
static cxa_exit_node   *cpp_atexit_header = LW_NULL;

/*
 *  cpp runtime lock and unlock
 */
#define __LW_CPP_RT_LOCK()              pthread_mutex_lock(&cpp_rt_lock)
#define __LW_CPP_RT_UNLOCK()            pthread_mutex_unlock(&cpp_rt_lock)

/*
 *  cpp global object construction guard
 */
#define __LW_CPP_GUARD_GET(gv)          (*((char *)(gv)))
#define __LW_CPP_GUARD_SET(gv, x)       (*((char *)(gv))  = x)

#define __LW_CPP_GUARD_MAKE_RELE(gv)    (*((char *)(gv)) |= 0x40)
#define __LW_CPP_GUARD_IS_RELE(gv)      (*((char *)(gv)) &  0x40)

/*
 *  cxa atexit node list add / delete / foreach
 */
#define __LW_CPP_INSERT_TO_HEADER(node, header) \
        do { \
            (node)->next = (header); \
            (node)->prev = LW_NULL; \
            if (header) { \
                (header)->prev = (node); \
            } \
            (header) = (node); \
        } while (0)

#define __LW_CPP_DELETE_FROM_LIST(node, header) \
        do { \
            if (!(node)->prev) { \
                (header) = (node)->next; \
            } else { \
                (node)->prev->next = (node)->next; \
            } \
            if ((node)->next) { \
                (node)->next->prev = (node)->prev; \
            } \
        } while (0)

#define __LW_CPP_FOREACH_FROM_LIST_SAFE(node, n, header) \
        for ((node) = (header), (n) = (node) ? (node)->next : LW_NULL; \
             (node) != LW_NULL; (node) = (n), (n) = (n) ? (n)->next : LW_NULL)

/*
 *  __cxa_atexit
 */
int  __cxa_atexit (void  (*f)(void *), void  *p, void  *d)
{
    cxa_exit_node  *node;

    node = (cxa_exit_node *)lib_malloc(sizeof(cxa_exit_node));
    if (!node) {
        fprintf(stderr, "C++ run time error - __cxa_atexit system low memory\n");
        return  (PX_ERROR);
    }

    node->func   = f;
    node->arg    = p;
    node->handle = d;

    __LW_CPP_RT_LOCK();
    __LW_CPP_INSERT_TO_HEADER(node, cpp_atexit_header);
    __LW_CPP_RT_UNLOCK();

    return  (ERROR_NONE);
}

/*
 *  __cxa_finalize
 */
void __cxa_finalize (void  *d)
{
    cxa_exit_node  *node;
    cxa_exit_node  *n;

    if (d) {
        __LW_CPP_RT_LOCK();
        __LW_CPP_FOREACH_FROM_LIST_SAFE(node, n, cpp_atexit_header) {
            if (node->handle == d) {
                __LW_CPP_DELETE_FROM_LIST(node, cpp_atexit_header);
                __LW_CPP_RT_UNLOCK();
                if (node->func) {
                    node->func(node->arg);
                }
                lib_free(node);
                __LW_CPP_RT_LOCK();
            }
        }
        __LW_CPP_RT_UNLOCK();
    } else {
        __LW_CPP_RT_LOCK();
        __LW_CPP_FOREACH_FROM_LIST_SAFE(node, n, cpp_atexit_header) {
            __LW_CPP_DELETE_FROM_LIST(node, cpp_atexit_header);
            __LW_CPP_RT_UNLOCK();
            if (node->func) {
                node->func(node->arg);
            }
            lib_free(node);
            __LW_CPP_RT_LOCK();
        }
        __LW_CPP_RT_UNLOCK();
    }
}

/*
 *  __cxa_module_finalize
 */
void __cxa_module_finalize (void  *base, size_t  len, BOOL  need_call)
{
    cxa_exit_node  *node;
    cxa_exit_node  *n;

    __LW_CPP_RT_LOCK();
    __LW_CPP_FOREACH_FROM_LIST_SAFE(node, n, cpp_atexit_header) {
        if (((char *)node->handle >= (char *)base) &&
            ((char *)node->handle <  (char *)base + len)) {
            __LW_CPP_DELETE_FROM_LIST(node, cpp_atexit_header);
            __LW_CPP_RT_UNLOCK();
            if (node->func && need_call) {
                node->func(node->arg);
            }
            lib_free(node);
            __LW_CPP_RT_LOCK();
        }
    }
    __LW_CPP_RT_UNLOCK();
}

/*
 *  __cxa_guard_acquire
 */
int  __cxa_guard_acquire (int volatile  *gv)
{
    int  can_con;

    __LW_CPP_RT_LOCK();

__retry:
    if (__LW_CPP_GUARD_IS_RELE(gv)) {
        can_con = 0;

    } else if (__LW_CPP_GUARD_GET(gv)) {
        pthread_cond_wait(&cpp_rt_cond, &cpp_rt_lock);
        goto    __retry;

    } else {
        __LW_CPP_GUARD_SET(gv, 1);
        can_con = 1;
    }

    __LW_CPP_RT_UNLOCK();

    return  (can_con);
}

/*
 *  __cxa_guard_release
 */
void  __cxa_guard_release (int volatile  *gv)
{
    __LW_CPP_RT_LOCK();

    __LW_CPP_GUARD_SET(gv, 1);
    __LW_CPP_GUARD_MAKE_RELE(gv);

    pthread_cond_broadcast(&cpp_rt_cond);

    __LW_CPP_RT_UNLOCK();
}

/*
 *  __cxa_guard_abort
 */
void  __cxa_guard_abort (int volatile  *gv)
{
    __LW_CPP_RT_LOCK();

    __LW_CPP_GUARD_SET(gv, 0);

    pthread_cond_broadcast(&cpp_rt_cond);

    __LW_CPP_RT_UNLOCK();
}

/*
 *  __cxa_pure_virtual
 */
void __cxa_pure_virtual ()
{
    fprintf(stderr, "C++ run time error - __cxa_pure_virtual should never be called\n");
    lib_abort();
}

/*
 * end
 */
