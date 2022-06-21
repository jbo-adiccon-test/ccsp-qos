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

#include "ansc_platform.h"
#include "ansc_time.h"
#include "cosa_apis_qosplugin.h"
#include "ccsp_trace.h"
#include "ccsp_syslog.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "qos_dm_api.h"
#include "qos_dm_load.h"

static int testval = 0;

/**********************************************************************

    caller:     owner of this object

    prototype:

                ULONG Classification_IsUpdated(ANSC_HANDLE hInsContext);

    description:

               This function is called to check if VoiceService updated .

    argument:   ANSC_HANDLE      hInsContext, 
               The instance handle;

    return:     The status of the operation.

**********************************************************************/


BOOL Classification_IsUpdated(ANSC_HANDLE hInsContext) {
    BOOL bIsUpdated = TRUE;
    return bIsUpdated;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

                ULONG Classification_Synchronize(ANSC_HANDLE hInsContext);

    description:

               This function is called to synchronize VoiceService data.

    argument:   ANSC_HANDLE      hInsContext, 
               The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG Classification_Synchronize(ANSC_HANDLE hInsContext) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Classification_GetEntryCount(ANSC_HANDLE hInsContext);

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG Classification_GetEntryCount(ANSC_HANDLE hInsContext) {
    ULONG count = 0;
    qos_ClassificationEntryCount(&count);
    return count;
}

/**********************************************************************
    caller:     owner of this object

    prototype:

        ANSC_HANDLE Classification_GetEntry(ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber);

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE Classification_GetEntry(ANSC_HANDLE hInsContext, ULONG nIndex, ULONG *pInsNumber) {
    ANSC_HANDLE pDmlEntry = NULL;

    Classification_t *pDmClsEntry = NULL;

    qos_ClassificationGetEntry(nIndex, &pDmClsEntry);

    if (pDmClsEntry != NULL) {
        *pInsNumber = pDmClsEntry->srv_instanceNumber;
        pDmlEntry = (ANSC_HANDLE) pDmClsEntry;
    }

    return pDmlEntry;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE Classification_AddEntry(ANSC_HANDLE hInsContext, ULONG* pInsNumber);

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
#define ADD_OBJ_MAGIC 10000

ANSC_HANDLE Classification_AddEntry(ANSC_HANDLE hInsContext, ULONG *pInsNumber) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    ULONG count = 0;
    Classification_t clf = {0}, *pNewClf = NULL;

    if (qos_ClassificationAddEntry(&clf) == ANSC_STATUS_SUCCESS) {
        qos_ClassificationGetEntryInstance(clf.srv_instanceNumber, &pNewClf);
        *pInsNumber = pNewClf->srv_instanceNumber;
    }

    return (ANSC_HANDLE) pNewClf;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Classification_DelEntry(ANSC_HANDLE hInsContext, ANSC_HANDLE hInstance);

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG Classification_DelEntry(ANSC_HANDLE hInsContext, ANSC_HANDLE hInstance) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    Classification_t *pClf = (Classification_t *) hInstance;

    if (pClf)
        returnStatus = qos_ClassificationDeleteEntryItem(pClf);

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Classification_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
#define GET_STR_PRM_VALIDATE_INPUT_BUFF(pUlSize, pStrBuf) if(_ansc_strlen(pStrBuf)>=*pUlSize){*pUlSize=_ansc_strlen(pStrBuf);return 1;}

ULONG
Classification_GetParamStringValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                char *pValue,
                ULONG *pUlSize
        ) {
    if (pParamName == NULL || pValue == NULL || pUlSize == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return -1;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return -1;
    }
// Add X_DT_Expiration Parameter
    else if (
            AnscEqualString(pParamName,
                            DM_CLF_X_DT_Expiration, TRUE)) {
        GET_STR_PRM_VALIDATE_INPUT_BUFF(pUlSize, pDmClsEntry->X_DT_Expiration);
        AnscCopyString(pValue, pDmClsEntry
                ->X_DT_Expiration);
    }
//Add Alias Param out of TR-181
    else if (
            AnscEqualString(pParamName,
                            DM_CLF_Alias, TRUE)) {
        GET_STR_PRM_VALIDATE_INPUT_BUFF(pUlSize, pDmClsEntry->Alias);
        AnscCopyString(pValue, pDmClsEntry
                ->Alias);
    } else if (
            AnscEqualString(pParamName,
                            DM_CLF_SourceMACAddress, TRUE)) {
        GET_STR_PRM_VALIDATE_INPUT_BUFF(pUlSize, pDmClsEntry->SourceMACAddress);
        AnscCopyString(pValue, pDmClsEntry
                ->SourceMACAddress);
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
        return -1;
    }

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Classification_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_GetParamBoolValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                BOOL *pBool
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || pBool == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_CLF_Enable, TRUE)) {
        *pBool = pDmClsEntry->Enable;
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Classification_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_GetParamUlongValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                ULONG *puLong
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || puLong == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;

}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Classification_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_GetParamIntValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                int *pInt
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || pInt == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return ret;
    } else if (
            AnscEqualString(pParamName,
                            DM_CLF_DSCPMark, TRUE)) {
        *
                pInt = pDmClsEntry->DSCPMark;
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

       BOOL
       Classification_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamArray,
                char*                       pString,
            );

    description:

        This function is called to set bulk parameter values; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name array;

                char*                       pString,
                The size of the array;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_SetParamStringValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                char *pString
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || pString == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return ret;
    } else if (
            AnscEqualString(pParamName,
                            DM_CLF_X_DT_Expiration, TRUE)) {
        AnscCopyString(pDmClsEntry
                               ->X_DT_Expiration, pString);
        ret = TRUE;
    } else if (
            AnscEqualString(pParamName,
                            DM_CLF_Alias, TRUE)) {
        AnscCopyString(pDmClsEntry
                               ->Alias, pString);
        ret = TRUE;
    } else if (
            AnscEqualString(pParamName,
                            DM_CLF_SourceMACAddress, TRUE)) {
        AnscCopyString(pDmClsEntry
                               ->SourceMACAddress, pString);
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return
            ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Classification_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_SetParamBoolValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                BOOL bValue
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_CLF_Enable, TRUE)) {
        pDmClsEntry->Enable = bValue;
        ret = TRUE;
        qos_CommitClassification(pDmClsEntry);
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Classification_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_SetParamUlongValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                ULONG uValue
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Classification_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Classification_SetParamIntValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                int iValue
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_CLF_DSCPMark, TRUE)) {
        pDmClsEntry->DSCPMark = iValue;
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL Classification_Validate(ANSC_HANDLE hInsContext);

    description:

        This function is called to validate the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     0 if there's no issue.

**********************************************************************/
BOOL
Classification_Validate
        (
                ANSC_HANDLE hInsContext
        ) {
    BOOL returnStatus = TRUE;

    printf("Classification_Validate\n");

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

       ULONG
       Classification_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Classification_Commit
        (
                ANSC_HANDLE hInsContext
        ) {
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;

    Classification_t *pDmClsEntry = (Classification_t *) hInsContext;

    if (pDmClsEntry == NULL) {
        printf("%s: (Classification_t*)hInsContext == NULL\n", __func__);
        return ANSC_STATUS_FAILURE;
    }

    returnStatus = ANSC_STATUS_SUCCESS;

    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Classification_Rollback(ANSC_HANDLE hInsContext);

    description:

        This function is called to validate the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     0 if there's no issue.

**********************************************************************/
ULONG Classification_Rollback
        (
                ANSC_HANDLE hInsContext
        ) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    printf("Classification_Rollback\n");

    return returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

                ULONG Queue_IsUpdated(ANSC_HANDLE hInsContext);

    description:

               This function is called to check if Queue updated .

    argument:   ANSC_HANDLE      hInsContext, 
               The instance handle;

    return:     The status of the operation.

**********************************************************************/


BOOL Queue_IsUpdated(ANSC_HANDLE hInsContext) {
    BOOL bIsUpdated = TRUE;
    return bIsUpdated;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

                ULONG Queue_Synchronize(ANSC_HANDLE hInsContext);

    description:

               This function is called to synchronize VoiceService data.

    argument:   ANSC_HANDLE      hInsContext, 
               The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG Queue_Synchronize(ANSC_HANDLE hInsContext) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Queue_GetEntryCount(ANSC_HANDLE hInsContext);

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG Queue_GetEntryCount(ANSC_HANDLE hInsContext) {
    ULONG count = 0;
    qos_QueueEntryCount(&count);
    return count;
}

/**********************************************************************
    caller:     owner of this object

    prototype:

        ANSC_HANDLE Queue_GetEntry(ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber);

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE Queue_GetEntry(ANSC_HANDLE hInsContext, ULONG nIndex, ULONG *pInsNumber) {
    ANSC_HANDLE pDmlEntry = NULL;

    Queue_t *pDmQueueEntry = NULL;

    qos_QueueGetEntry(nIndex, &pDmQueueEntry);

    if (pDmQueueEntry != NULL) {
        *pInsNumber = pDmQueueEntry->srv_instanceNumber;
        pDmlEntry = (ANSC_HANDLE) pDmQueueEntry;
    }

    return pDmlEntry;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE Queue_AddEntry(ANSC_HANDLE hInsContext, ULONG* pInsNumber);

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
ANSC_HANDLE Queue_AddEntry(ANSC_HANDLE hInsContext, ULONG *pInsNumber) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    ULONG count = 0;
    Queue_t queue = {0}, *pNewQueue = NULL;

    if (qos_QueueAddEntry(&queue) == ANSC_STATUS_SUCCESS) {
        qos_QueueGetEntryInstance(queue.srv_instanceNumber, &pNewQueue);
        *pInsNumber = pNewQueue->srv_instanceNumber;
    }

    return (ANSC_HANDLE) pNewQueue;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Queue_DelEntry(ANSC_HANDLE hInsContext, ANSC_HANDLE hInstance);

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG Queue_DelEntry(ANSC_HANDLE hInsContext, ANSC_HANDLE hInstance) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    Queue_t *pQueue = (Queue_t *) hInstance;

    if (pQueue)
        returnStatus = qos_QueueDeleteEntryItem(pQueue);

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Queue_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Queue_GetParamStringValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                char *pValue,
                ULONG *pUlSize
        ) {
    if (pParamName == NULL || pValue == NULL || pUlSize == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return -1;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return -1;
    }

    if (AnscEqualString(pParamName, DM_QUEUE_Interface, TRUE)) {
        GET_STR_PRM_VALIDATE_INPUT_BUFF(pUlSize, pDmQueueEntry->Interface);
        AnscCopyString(pValue, pDmQueueEntry->Interface);
    } else if (AnscEqualString(pParamName, DM_QUEUE_Alias, TRUE)) {
        GET_STR_PRM_VALIDATE_INPUT_BUFF(pUlSize, pDmQueueEntry->Alias);
        AnscCopyString(pValue, pDmQueueEntry->Alias);
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
        return -1;
    }

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Queue_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_GetParamBoolValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                BOOL *pBool
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || pBool == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_QUEUE_Enable, TRUE)) {
        *pBool = pDmQueueEntry->Enable;
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype:

        BOOL
        Queue_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_GetParamUlongValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                ULONG *puLong
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || puLong == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_QUEUE_Bandwidth, TRUE)) {
        *puLong = pDmQueueEntry->Bandwidth;
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Queue_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_GetParamIntValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                int *pInt
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || pInt == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

       BOOL
       Queue_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamArray,
                char*                       pString,
            );

    description:

        This function is called to set bulk parameter values; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name array;

                char*                       pString,
                The size of the array;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_SetParamStringValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                char *pString
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL || pString == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_QUEUE_Interface, TRUE)) {
        AnscCopyString(pDmQueueEntry->Interface, pString);
        ret = TRUE;
    } else if (AnscEqualString(pParamName, DM_QUEUE_Alias, TRUE)) {
        AnscCopyString(pDmQueueEntry->Alias, pString);
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Queue_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_SetParamBoolValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                BOOL bValue
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_QUEUE_Enable, TRUE)) {
        pDmQueueEntry->Enable = bValue;
        ret = TRUE;
        qos_CommitQueue(pDmQueueEntry);
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Queue_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_SetParamUlongValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                ULONG uValue
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    if (AnscEqualString(pParamName, DM_QUEUE_Bandwidth, TRUE)) {
        pDmQueueEntry->Bandwidth = uValue;
        ret = TRUE;
    } else {
        printf("%s: Unsupported parameter '%s'\n", __func__, pParamName);
    }

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Queue_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Queue_SetParamIntValue
        (
                ANSC_HANDLE hInsContext,
                char *pParamName,
                int iValue
        ) {
    BOOL ret = FALSE;

    if (pParamName == NULL) {
        printf("%s: Invalid Input Parameter [NULL]\n", __func__);
        return ret;
    }

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ret;
    }

    printf("%s: Unsupported parameter Huhaha Darkness comes '%s'\n", __func__, pParamName);

    return ret;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL Queue_Validate(ANSC_HANDLE hInsContext);

    description:

        This function is called to validate the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     0 if there's no issue.

**********************************************************************/
BOOL
Queue_Validate
        (
                ANSC_HANDLE hInsContext
        ) {
    BOOL returnStatus = TRUE;

    printf("Queue_Validate\n");

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

       ULONG
       Queue_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Queue_Commit
        (
                ANSC_HANDLE hInsContext
        ) {
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;

    Queue_t *pDmQueueEntry = (Queue_t *) hInsContext;

    if (pDmQueueEntry == NULL) {
        printf("%s: (Queue_t*)hInsContext == NULL\n", __func__);
        return ANSC_STATUS_FAILURE;
    }

    returnStatus = ANSC_STATUS_SUCCESS;

    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Queue_Rollback(ANSC_HANDLE hInsContext);

    description:

        This function is called to validate the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     0 if there's no issue.

**********************************************************************/
ULONG Queue_Rollback
        (
                ANSC_HANDLE hInsContext
        ) {
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    printf("Queue_Rollback\n");

    return returnStatus;
}
