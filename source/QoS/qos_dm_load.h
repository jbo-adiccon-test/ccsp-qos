/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2021 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef _QOS_DM_JSON_CFG_INIT_H
#define _QOS_DM_JSON_CFG_INIT_H

#include <stdbool.h>
#include "qos_dm_api.h"

#define QOS_CONFIG_DEFAULT_PATH "/usr/ccsp/qos/"
#define QOS_CONFIG_CURRENT_PATH "/usr/ccsp/qos/"
#define QOS_CONFIG_CURRENT_NAME "QoS_config_current.json"
#define QOS_CONFIG_DEFAULT_NAME "QoS_defaults.json"
#define MAX_REGION_LENGTH   16
#define MAX_FILENAME_LENGTH 128
#define MAX_DIRNAME_LENGTH 255

typedef union {
    const char *pValString;
    uint32_t unsignedInt;
    bool boolean;
    int integer;
} Value_t;

#define DM_QOS                      "QoS"

#define DM_QUEUE                    "Queue"
#define DM_QUEUE_Enable             "Enable"
#define DM_QUEUE_Interface          "Interface"
#define DM_QUEUE_X_DT_Bandwidth     "X_DT_Bandwidth"
#define DM_QUEUE_Alias              "Alias"

#define DM_CLF                      "Classification"
#define DM_CLF_Enable               "Enable"
#define DM_CLF_SourceMACAddress     "SourceMACAddress"
#define DM_CLF_DSCPMark             "DSCPMark"
#define DM_CLF_X_DT_Expiration      "X_DT_Expiration"
#define DM_CLF_Id                   "Id"

//Add own Alias out of TR-181
#define DM_CLF_Alias                "Alias"

int32_t qos_DmLoadDefaults(void);

bool qos_DmSave(const char *pTableName, uint32_t entryIdx, const char *pValueName, Value_t *pParam);

bool qos_DmSaveQoS(QoS_t *qos);

bool qos_DmSetString(const char *pTableName, uint32_t entryIdx, const char *valueName, const char *value);

bool qos_DmSetInt(const char *pTableName, uint32_t entryIdx, const char *valueName, int32_t value);

bool qos_DmSetBool(const char *pTableName, uint32_t entryIdx, const char *valueName, bool value);

#endif
