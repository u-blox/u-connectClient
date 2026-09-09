/*
 * Copyright 2025 u-blox
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

/** @file
 * @brief Queue for incoming URCs
 */

#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"
#include "limits.h"
#include "string.h"

#include "u_cx_log.h"
#include "u_cx_at_urc_queue.h"

#if U_CX_USE_URC_QUEUE == 1

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

#define U_URC_ALIGN_SIZE(SIZE) \
    (((SIZE) + sizeof(uint16_t) - 1) & ~(sizeof(uint16_t) - 1))

/* ----------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * STATIC PROTOTYPES
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * STATIC VARIABLES
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * STATIC FUNCTIONS
 * -------------------------------------------------------------- */

static size_t getEnqueueOffset(uCxAtUrcQueue_t *pUrcQueue,
                               size_t requiredSize)
{
    if (requiredSize > pUrcQueue->bufferLen - pUrcQueue->usedBytes) {
        return SIZE_MAX;
    }

    if (pUrcQueue->usedBytes == 0) {
        pUrcQueue->readPos = pUrcQueue->startPos;
        pUrcQueue->writePos = pUrcQueue->startPos;
        pUrcQueue->wrapPos = pUrcQueue->bufferLen;
        pUrcQueue->isWrapped = false;
    }

    if (pUrcQueue->isWrapped) {
        return requiredSize <= pUrcQueue->readPos - pUrcQueue->writePos ?
               pUrcQueue->writePos : SIZE_MAX;
    }

    size_t tailSpace = pUrcQueue->bufferLen - pUrcQueue->writePos;
    size_t headSpace = pUrcQueue->readPos - pUrcQueue->startPos;
    if ((headSpace > tailSpace) && (requiredSize <= headSpace)) {
        pUrcQueue->wrapPos = pUrcQueue->writePos;
        pUrcQueue->isWrapped = true;
        pUrcQueue->writePos = pUrcQueue->startPos;
        return pUrcQueue->startPos;
    }
    if (requiredSize <= tailSpace) {
        return pUrcQueue->writePos;
    }
    if (requiredSize <= headSpace) {
        pUrcQueue->wrapPos = pUrcQueue->writePos;
        pUrcQueue->isWrapped = true;
        pUrcQueue->writePos = pUrcQueue->startPos;
        return pUrcQueue->startPos;
    }

    return SIZE_MAX;
}

/* ----------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * -------------------------------------------------------------- */

void uCxAtUrcQueueInit(uCxAtUrcQueue_t *pUrcQueue, void *pBuffer, size_t bufferLen)
{
    memset(pUrcQueue, 0, sizeof(uCxAtUrcQueue_t));
    U_CX_MUTEX_CREATE(pUrcQueue->queueMutex);
    U_CX_MUTEX_CREATE(pUrcQueue->dequeueMutex);
    pUrcQueue->pBuffer = pBuffer;
    pUrcQueue->bufferLen = bufferLen;
    pUrcQueue->startPos = (size_t)((uintptr_t)pBuffer &
                                   (sizeof(uint16_t) - 1));
    pUrcQueue->readPos = pUrcQueue->startPos;
    pUrcQueue->writePos = pUrcQueue->startPos;
    pUrcQueue->wrapPos = bufferLen;
}

void uCxAtUrcQueueDeInit(uCxAtUrcQueue_t *pUrcQueue)
{
    U_CX_MUTEX_DELETE(pUrcQueue->queueMutex);
    U_CX_MUTEX_DELETE(pUrcQueue->dequeueMutex);
}

bool uCxAtUrcQueueEnqueueBegin(uCxAtUrcQueue_t *pUrcQueue, const char *pUrcLine, size_t urcLineLen)
{
    bool ret;

    U_CX_MUTEX_LOCK(pUrcQueue->queueMutex);
    U_CX_AT_PORT_ASSERT(pUrcQueue->pEnqueueEntry == NULL);

    size_t entryOffset = SIZE_MAX;
    size_t entrySize = 0;
    if ((urcLineLen <= UINT16_MAX) &&
        (urcLineLen <= SIZE_MAX - sizeof(uUrcEntry_t) - 1)) {
        entrySize = sizeof(uUrcEntry_t) + urcLineLen + 1;
        entryOffset = getEnqueueOffset(pUrcQueue, entrySize);
    }
    if (entryOffset != SIZE_MAX) {
        uUrcEntry_t *pEntry = (uUrcEntry_t *)&pUrcQueue->pBuffer[entryOffset];
        memcpy(&pEntry->data[0], pUrcLine, urcLineLen);
        pEntry->data[urcLineLen] = 0; // Add null term
        pEntry->strLineLen = (uint16_t)urcLineLen;
        pEntry->payloadSize = 0;
        pUrcQueue->writePos += entrySize;
        pUrcQueue->usedBytes += entrySize;
        size_t segmentEnd = pUrcQueue->isWrapped ?
                            pUrcQueue->readPos : pUrcQueue->bufferLen;
        pUrcQueue->enqueueCapacity = segmentEnd - pUrcQueue->writePos;
        pUrcQueue->pEnqueueEntry = pEntry;
        ret = true;
    } else {
        // Not enough space available
        ret = false;
    }

    U_CX_MUTEX_UNLOCK(pUrcQueue->queueMutex);
    return ret;
}

uint16_t uCxAtUrcQueueEnqueueGetPayloadPtr(uCxAtUrcQueue_t *pUrcQueue, uint8_t **ppPayload)
{
    U_CX_AT_PORT_ASSERT(pUrcQueue->pEnqueueEntry);

    uUrcEntry_t *pEntry = pUrcQueue->pEnqueueEntry;
    *ppPayload = &pEntry->data[pEntry->strLineLen + 1];
    size_t payloadCapacity = pUrcQueue->enqueueCapacity;
    return payloadCapacity > UINT16_MAX ?
           UINT16_MAX : (uint16_t)payloadCapacity;
}

void uCxAtUrcQueueEnqueueEnd(uCxAtUrcQueue_t *pUrcQueue, uint16_t payloadSize)
{
    U_CX_MUTEX_LOCK(pUrcQueue->queueMutex);
    U_CX_AT_PORT_ASSERT(pUrcQueue->pEnqueueEntry);

    uUrcEntry_t *pEntry = pUrcQueue->pEnqueueEntry;
    size_t entryPrefixSize = sizeof(uUrcEntry_t) + pEntry->strLineLen + 1;
    size_t entrySize = U_URC_ALIGN_SIZE(entryPrefixSize + payloadSize);
    size_t additionalSize = entrySize - entryPrefixSize;
    if (additionalSize > pUrcQueue->enqueueCapacity) {
        additionalSize = payloadSize;
    }
    U_CX_AT_PORT_ASSERT(pUrcQueue->enqueueCapacity >= additionalSize);

    pEntry->payloadSize = payloadSize;
    pUrcQueue->writePos += additionalSize;
    pUrcQueue->usedBytes += additionalSize;
    pUrcQueue->enqueueCapacity = 0;
    pUrcQueue->pEnqueueEntry = NULL;
    U_CX_MUTEX_UNLOCK(pUrcQueue->queueMutex);
}

void uCxAtUrcQueueEnqueueAbort(uCxAtUrcQueue_t *pUrcQueue)
{
    U_CX_MUTEX_LOCK(pUrcQueue->queueMutex);
    U_CX_AT_PORT_ASSERT(pUrcQueue->pEnqueueEntry);

    uUrcEntry_t *pEntry = pUrcQueue->pEnqueueEntry;
    size_t entryOffset = (size_t)((uint8_t *)pEntry - pUrcQueue->pBuffer);
    size_t entrySize = sizeof(uUrcEntry_t) + pEntry->strLineLen + 1;
    pUrcQueue->usedBytes -= entrySize;
    if ((entryOffset == pUrcQueue->startPos) &&
        pUrcQueue->isWrapped) {
        pUrcQueue->writePos = pUrcQueue->wrapPos;
        pUrcQueue->wrapPos = pUrcQueue->bufferLen;
        pUrcQueue->isWrapped = false;
    } else {
        pUrcQueue->writePos = entryOffset;
    }
    pUrcQueue->enqueueCapacity = 0;
    pUrcQueue->pEnqueueEntry = NULL;
    U_CX_MUTEX_UNLOCK(pUrcQueue->queueMutex);
}

uUrcEntry_t *uCxAtUrcQueueDequeueBegin(uCxAtUrcQueue_t *pUrcQueue)
{
    uUrcEntry_t *pEntry = NULL;

    if (U_CX_MUTEX_TRY_LOCK(pUrcQueue->dequeueMutex, 0) == 0) {
        // If a dequeue is already in progress (reentrant call from URC callback
        // that triggers an AT command which calls processUrcs again), skip gracefully
        if (pUrcQueue->pDequeueEntry != NULL) {
            U_CX_MUTEX_UNLOCK(pUrcQueue->dequeueMutex);
            return NULL;
        }

        U_CX_MUTEX_LOCK(pUrcQueue->queueMutex);
        if ((pUrcQueue->usedBytes > 0) &&
            (pUrcQueue->pEnqueueEntry == NULL)) {
            pEntry = (uUrcEntry_t *)&pUrcQueue->pBuffer[pUrcQueue->readPos];
        }
        U_CX_MUTEX_UNLOCK(pUrcQueue->queueMutex);

        if (pEntry) {
            pUrcQueue->pDequeueEntry = pEntry;
        } else {
            U_CX_MUTEX_UNLOCK(pUrcQueue->dequeueMutex);
        }
    }

    return pEntry;
}

void uCxAtUrcQueueDequeueEnd(uCxAtUrcQueue_t *pUrcQueue, uUrcEntry_t *pEntry)
{
    U_CX_AT_PORT_ASSERT(pUrcQueue->pDequeueEntry != NULL);
    U_CX_AT_PORT_ASSERT(pUrcQueue->pDequeueEntry == pEntry);

    U_CX_MUTEX_LOCK(pUrcQueue->queueMutex);
    size_t entryOffset = (size_t)((uint8_t *)pEntry - pUrcQueue->pBuffer);
    size_t rawEntrySize = sizeof(uUrcEntry_t) + pEntry->strLineLen + 1 +
                          pEntry->payloadSize;
    size_t totEntrySize = U_URC_ALIGN_SIZE(rawEntrySize);
    size_t segmentEnd = pUrcQueue->isWrapped ?
                        pUrcQueue->wrapPos : pUrcQueue->bufferLen;
    if (entryOffset + totEntrySize > segmentEnd) {
        totEntrySize = rawEntrySize;
    }
    pUrcQueue->readPos += totEntrySize;
    pUrcQueue->usedBytes -= totEntrySize;
    if (pUrcQueue->isWrapped &&
        (pUrcQueue->readPos == pUrcQueue->wrapPos)) {
        pUrcQueue->readPos = pUrcQueue->startPos;
        pUrcQueue->wrapPos = pUrcQueue->bufferLen;
        pUrcQueue->isWrapped = false;
    }
    if (pUrcQueue->usedBytes == 0) {
        pUrcQueue->readPos = pUrcQueue->startPos;
        pUrcQueue->writePos = pUrcQueue->startPos;
        pUrcQueue->wrapPos = pUrcQueue->bufferLen;
        pUrcQueue->isWrapped = false;
    }
    U_CX_MUTEX_UNLOCK(pUrcQueue->queueMutex);

    pUrcQueue->pDequeueEntry = NULL;

    U_CX_MUTEX_UNLOCK(pUrcQueue->dequeueMutex);
}

#endif // U_CX_USE_URC_QUEUE == 1
