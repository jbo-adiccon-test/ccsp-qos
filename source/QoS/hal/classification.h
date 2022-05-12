#ifndef __QOS_CLASSIFICATION_HAL_H__
#define __QOS_CLASSIFICATION_HAL_H__

#include <net/if.h>
#include <arpa/inet.h>

#define CLASS_MAC_SIZE 18
#define CLASS_CHAIN_NAME_SIZE 16

struct qos_class {
    // Unique ID
    unsigned id;
    // Traffic class of the rule
    int traffic_class;
    // Chain: PREROUTING, INPUT, FORWARD, OUTPUT, POSTROUTING
    char chain_name[CLASS_CHAIN_NAME_SIZE];
    // DSCP mark value
    int dscp_mark;
    // Input interface
    char iface_in[IFNAMSIZ];
    // Output interface
    char iface_out[IFNAMSIZ];
    // Source mac
    char mac_src_addr[CLASS_MAC_SIZE];
    // --tcp-flags SYN,ACK,FIN,RST ACK except PSH

    char duration[64];

    //Add Alias out of TR-181 for classification
    char alias[255];
};

int qos_addClass(const struct qos_class *param);

int qos_removeAllClasses();

int qos_removeOneClass();

#endif
