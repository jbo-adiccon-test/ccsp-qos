#ifndef __QOS_QUEUE_HAL_H__
#define __QOS_QUEUE_HAL_H__

#include <net/if.h>

#define QUEUE_MAX_TC 16

enum queue_alg {
    QUEUE_ALG_SP, QUEUE_ALG_WRR
};

struct qos_queue {
    // Interface name
    char device_name[IFNAMSIZ];

    unsigned bandwidth;
    char alias[256];
};

// Adds queue & shaping
int qos_addQueue(int index, struct qos_queue *queue);

// Removes ALL queues & shaping
int qos_removeQueue(struct qos_queue *queue);

#endif
