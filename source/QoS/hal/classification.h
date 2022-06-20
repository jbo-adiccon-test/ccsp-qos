#ifndef __QOS_CLASSIFICATION_HAL_H__
#define __QOS_CLASSIFICATION_HAL_H__

#include <net/if.h>
#include <arpa/inet.h>

#define CLASS_MAC_SIZE 18
#define CLASS_CHAIN_NAME_SIZE 16

struct qos_class {
    // Unique ID
    unsigned id;
    // DSCP mark value
    int dscp_mark;
    // Source mac
    char mac_src_addr[CLASS_MAC_SIZE];
    // --tcp-flags SYN,ACK,FIN,RST ACK except PSH

    char duration[64];

    //Add Alias out of TR-181 for classification
    char alias[255];
};

/**
 * A Type to alloc the qos class in an type
 */
typedef struct {
    const struct qos_class *data;
    size_t size;
    char *str;
} qos_struct;

int qos_addClass(const struct qos_class *param);

int qos_removeAllClasses();

int qos_removeOneClass(char *com, char *file);

int qos_persistClass(const qos_struct *obj);

#endif
