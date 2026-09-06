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

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "unity.h"
#include "u_cx_at_urc_queue.h"

/* ----------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------- */

typedef struct {
    const char *pString;
    const uSockIpAddress_t expectedAddr;
} uIpTestEntry_t;

/* ----------------------------------------------------------------
 * STATIC VARIABLES
 * -------------------------------------------------------------- */

static uint8_t gBuffer[512];
static uCxAtUrcQueue_t gQueue;

/* ----------------------------------------------------------------
 * STATIC FUNCTIONS
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * TEST FUNCTIONS
 * -------------------------------------------------------------- */

void setUp(void)
{
    memset(&gBuffer[0], 1, sizeof(gBuffer));
    uCxAtUrcQueueInit(&gQueue, &gBuffer[0], sizeof(gBuffer));
}

void tearDown(void)
{
    uCxAtUrcQueueDeInit(&gQueue);
}

void test_queueingOfNonNullString_expectNullTermString(void)
{
    char myString[] = "FOO123!";
    char expected[] = "FOO123";
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString, strlen(expected)));
    uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);

    TEST_ASSERT_EQUAL(strlen(expected), pEntry->strLineLen);
    TEST_ASSERT_EQUAL_STRING(expected, pEntry->data);
}

void test_queueingWithPayload_expectPayload(void)
{
    char myString[] = "FOO123";
    uint8_t myPayload[] = { 0x00, 0x01, 0x02 };
    uint8_t *pPayload = NULL;

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString, strlen(myString)));
    size_t length = uCxAtUrcQueueEnqueueGetPayloadPtr(&gQueue, &pPayload);
    TEST_ASSERT_EQUAL(sizeof(gBuffer) - sizeof(uUrcEntry_t) - 1 - strlen(myString), length);
    memcpy(pPayload, &myPayload[0], sizeof(myPayload));
    uCxAtUrcQueueEnqueueEnd(&gQueue, sizeof(myPayload));

    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);
    TEST_ASSERT_NOT_NULL(pEntry);
    TEST_ASSERT_EQUAL(strlen(myString), pEntry->strLineLen);
    TEST_ASSERT_EQUAL_STRING(myString, pEntry->data);
    pPayload = &pEntry->data[pEntry->strLineLen + 1];
    TEST_ASSERT_EQUAL(sizeof(myPayload), pEntry->payloadSize);
    TEST_ASSERT_EQUAL_MEMORY(myPayload, pPayload, sizeof(myPayload));
}

void test_queueingMultiple_expectMultiple(void)
{
    char *myStrings[3] = { "FOO1", "FOO2", "FOO3" };
    uint8_t myPayloads[3][3] = {
        { 0x01, 0x01, 0x01 },
        { 0x02, 0x02, 0x02 },
        { 0x03, 0x03, 0x03 }
    };
    uint8_t *pPayload = NULL;

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myStrings[i], strlen(myStrings[i])));
        uCxAtUrcQueueEnqueueGetPayloadPtr(&gQueue, &pPayload);
        memcpy(pPayload, &myPayloads[i][0], sizeof(myPayloads[i]));
        uCxAtUrcQueueEnqueueEnd(&gQueue, sizeof(myPayloads[i]));
    }

    for (int i = 0; i < 3; i++) {
        uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);
        TEST_ASSERT_NOT_NULL(pEntry);
        TEST_ASSERT_EQUAL(strlen(myStrings[i]), pEntry->strLineLen);
        TEST_ASSERT_EQUAL_STRING(myStrings[i], pEntry->data);
        pPayload = &pEntry->data[pEntry->strLineLen + 1];
        TEST_ASSERT_EQUAL(sizeof(myPayloads[i]), pEntry->payloadSize);
        TEST_ASSERT_EQUAL_MEMORY(myPayloads[i], pPayload, sizeof(myPayloads[i]));
        uCxAtUrcQueueDequeueEnd(&gQueue, pEntry);
    }
}

void test_abortedQueueing_expectEmptyQueue(void)
{
    char myString[] = "FOO123";
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString, strlen(myString)));
    uCxAtUrcQueueEnqueueAbort(&gQueue);
    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);

    TEST_ASSERT_EQUAL(NULL, pEntry);
}

void test_incompleteQueueing_expectHiddenUntilCompletedOrAborted(void)
{
    char myString[] = "FOO123";

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString,
                                               strlen(myString)));
    TEST_ASSERT_NULL(uCxAtUrcQueueDequeueBegin(&gQueue));
    uCxAtUrcQueueEnqueueAbort(&gQueue);

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString,
                                               strlen(myString)));
    uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    TEST_ASSERT_NOT_NULL(uCxAtUrcQueueDequeueBegin(&gQueue));
}

void test_uCxAtUrcQueueEnqueueBegin_withFullQueue_expectFailure(void)
{
    char myString[sizeof(gBuffer) / 2];
    memset(&myString[0], 'A', sizeof(myString));
    // Make sure we fill up the queue by queueing a string that is half the buffer size
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString, sizeof(myString)));
    uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    TEST_ASSERT_FALSE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString, sizeof(myString)));
}

void test_uCxAtUrcQueueDequeueBegin_withEmptyQueue_expectNull(void)
{
    TEST_ASSERT_NULL(uCxAtUrcQueueDequeueBegin(&gQueue));
}

void test_uCxAtUrcQueueDequeueBegin_calledTwiceWithNonEmptyQueue_expectNull(void)
{
    char myString[] = "FOO123";
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, myString, strlen(myString)));
    uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    TEST_ASSERT_NOT_NULL(uCxAtUrcQueueDequeueBegin(&gQueue));
    TEST_ASSERT_NULL(uCxAtUrcQueueDequeueBegin(&gQueue));
}

void test_queueWrap_preservesOrderPayloadAndEntryAddresses(void)
{
    char lines[4][151];
    for (size_t index = 0; index < 4; index++) {
        memset(lines[index], (int)('A' + index), sizeof(lines[index]) - 1);
        lines[index][sizeof(lines[index]) - 1] = 0;
    }

    uUrcEntry_t *pEntries[4];
    for (size_t index = 0; index < 3; index++) {
        TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, lines[index],
                                                   strlen(lines[index])));
        pEntries[index] = gQueue.pEnqueueEntry;
        uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    }

    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);
    TEST_ASSERT_EQUAL_PTR(pEntries[0], pEntry);
    uCxAtUrcQueueDequeueEnd(&gQueue, pEntry);

    uint8_t *pPayload;
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, lines[3], 100));
    pEntries[3] = gQueue.pEnqueueEntry;
    TEST_ASSERT_EQUAL_PTR(gBuffer, pEntries[3]);
    TEST_ASSERT_GREATER_OR_EQUAL(32,
                                 uCxAtUrcQueueEnqueueGetPayloadPtr(&gQueue,
                                                                  &pPayload));
    memset(pPayload, 0xa5, 32);
    uCxAtUrcQueueEnqueueEnd(&gQueue, 32);

    for (size_t index = 1; index < 4; index++) {
        pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);
        TEST_ASSERT_EQUAL_PTR(pEntries[index], pEntry);
        TEST_ASSERT_EQUAL_CHAR('A' + (int)index, pEntry->data[0]);
        if (index == 3) {
            TEST_ASSERT_EQUAL(32, pEntry->payloadSize);
            TEST_ASSERT_EACH_EQUAL_HEX8(0xa5,
                                        &pEntry->data[pEntry->strLineLen + 1],
                                        pEntry->payloadSize);
        }
        uCxAtUrcQueueDequeueEnd(&gQueue, pEntry);
    }
    TEST_ASSERT_NULL(uCxAtUrcQueueDequeueBegin(&gQueue));
}

void test_abortedWrappedEnqueue_restoresTailSpace(void)
{
    char longLine[151];
    char shortLine[40];
    memset(longLine, 'A', sizeof(longLine) - 1);
    longLine[sizeof(longLine) - 1] = 0;
    memset(shortLine, 'B', sizeof(shortLine) - 1);
    shortLine[sizeof(shortLine) - 1] = 0;

    for (size_t index = 0; index < 3; index++) {
        TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, longLine,
                                                   strlen(longLine)));
        uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    }
    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);
    uCxAtUrcQueueDequeueEnd(&gQueue, pEntry);

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, longLine, 100));
    TEST_ASSERT_EQUAL_PTR(gBuffer, gQueue.pEnqueueEntry);
    uCxAtUrcQueueEnqueueAbort(&gQueue);
    TEST_ASSERT_EQUAL(468, gQueue.writePos);
    TEST_ASSERT_EQUAL(sizeof(gBuffer), gQueue.wrapPos);

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, shortLine,
                                               strlen(shortLine)));
    TEST_ASSERT_EQUAL_PTR(gBuffer, gQueue.pEnqueueEntry);
    uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
}

void test_payloadUsingReportedCapacity_fitsWithAlignment(void)
{
    uint16_t alignedStorage[256];
    uint8_t *pOddBuffer = (uint8_t *)alignedStorage;
    uCxAtUrcQueue_t oddQueue;
    char line[] = "A";
    uint8_t *pPayload;

    uCxAtUrcQueueInit(&oddQueue, pOddBuffer, sizeof(alignedStorage) - 1);
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&oddQueue, line, strlen(line)));
    uint16_t capacity = uCxAtUrcQueueEnqueueGetPayloadPtr(&oddQueue, &pPayload);
    TEST_ASSERT_EQUAL(505, capacity);
    memset(pPayload, 0x5a, capacity);
    uCxAtUrcQueueEnqueueEnd(&oddQueue, capacity);

    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&oddQueue);
    TEST_ASSERT_NOT_NULL(pEntry);
    TEST_ASSERT_EQUAL(capacity, pEntry->payloadSize);
    TEST_ASSERT_EACH_EQUAL_HEX8(0x5a,
                                &pEntry->data[pEntry->strLineLen + 1],
                                capacity);
    uCxAtUrcQueueDequeueEnd(&oddQueue, pEntry);
    TEST_ASSERT_NULL(uCxAtUrcQueueDequeueBegin(&oddQueue));
    uCxAtUrcQueueDeInit(&oddQueue);
}

void test_unpaddedTailEntry_wrapsToHead(void)
{
    uint16_t alignedStorage[32];
    uCxAtUrcQueue_t queue;
    uint8_t *pPayload;

    uCxAtUrcQueueInit(&queue, alignedStorage, sizeof(alignedStorage) - 1);
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, "first", 5));
    uCxAtUrcQueueEnqueueEnd(&queue, 0);
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, "tail", 4));
    TEST_ASSERT_EQUAL(44,
                      uCxAtUrcQueueEnqueueGetPayloadPtr(&queue, &pPayload));
    memset(pPayload, 0x5a, 44);
    uCxAtUrcQueueEnqueueEnd(&queue, 44);

    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL_STRING("first", pEntry->data);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, "head", 4));
    uCxAtUrcQueueEnqueueEnd(&queue, 0);

    pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL_STRING("tail", pEntry->data);
    TEST_ASSERT_EQUAL(44, pEntry->payloadSize);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);
    pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL_STRING("head", pEntry->data);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);
    TEST_ASSERT_NULL(uCxAtUrcQueueDequeueBegin(&queue));
    uCxAtUrcQueueDeInit(&queue);
}

void test_wrappedHeadFilledToReadPosition_rejectsOverwrite(void)
{
    uint16_t alignedStorage[32];
    uCxAtUrcQueue_t queue;
    char tailLine[49];

    memset(tailLine, 'T', sizeof(tailLine) - 1);
    tailLine[sizeof(tailLine) - 1] = 0;

    uCxAtUrcQueueInit(&queue, alignedStorage, sizeof(alignedStorage));
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, "first", 5));
    uCxAtUrcQueueEnqueueEnd(&queue, 0);
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, tailLine, 48));
    uCxAtUrcQueueEnqueueEnd(&queue, 0);

    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL_STRING("first", pEntry->data);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);

    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, "head", 5));
    uCxAtUrcQueueEnqueueEnd(&queue, 0);
    TEST_ASSERT_EQUAL(queue.readPos, queue.writePos);
    TEST_ASSERT_FALSE(uCxAtUrcQueueEnqueueBegin(&queue, "x", 1));

    pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL_STRING(tailLine, pEntry->data);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);
    pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL_STRING("head", pEntry->data);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);
    uCxAtUrcQueueDeInit(&queue);
}

void test_unalignedBuffer_alignsEntryHeader(void)
{
    uint8_t storage[64];
    uint8_t *pBuffer = ((uintptr_t)storage & 1U) == 0 ?
                       &storage[1] : &storage[0];
    uCxAtUrcQueue_t queue;

    TEST_ASSERT_EQUAL(1, (uintptr_t)pBuffer & 1U);
    uCxAtUrcQueueInit(&queue, pBuffer, sizeof(storage) - 1);
    TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&queue, "ABC", 3));
    uCxAtUrcQueueEnqueueEnd(&queue, 0);

    uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&queue);
    TEST_ASSERT_EQUAL(0, (uintptr_t)pEntry & 1U);
    TEST_ASSERT_EQUAL_STRING("ABC", pEntry->data);
    uCxAtUrcQueueDequeueEnd(&queue, pEntry);
    uCxAtUrcQueueDeInit(&queue);
}

void test_repeatedWrapCycles_preserveFifoOrder(void)
{
    char line[32];

    for (int value = 0; value < 8; value++) {
        int length = snprintf(line, sizeof(line), "URC-%04d-abcdefghijkl", value);
        TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, line,
                                                   (size_t)length));
        uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    }

    for (int value = 0; value < 1000; value++) {
        uUrcEntry_t *pEntry = uCxAtUrcQueueDequeueBegin(&gQueue);
        TEST_ASSERT_NOT_NULL(pEntry);
        int length = snprintf(line, sizeof(line), "URC-%04d-abcdefghijkl", value);
        TEST_ASSERT_EQUAL_STRING(line, pEntry->data);
        uCxAtUrcQueueDequeueEnd(&gQueue, pEntry);

        length = snprintf(line, sizeof(line), "URC-%04d-abcdefghijkl", value + 8);
        TEST_ASSERT_TRUE(uCxAtUrcQueueEnqueueBegin(&gQueue, line,
                                                   (size_t)length));
        uCxAtUrcQueueEnqueueEnd(&gQueue, 0);
    }
}

void test_unrepresentableLineLength_isRejected(void)
{
    TEST_ASSERT_FALSE(uCxAtUrcQueueEnqueueBegin(&gQueue, "A",
                                                (size_t)UINT16_MAX + 1));
}
