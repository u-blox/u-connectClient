/** @file
 * @brief u-connectXpress API
 */

#ifndef U_CX_H
#define U_CX_H

#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "u_cx_at_client.h"
#include "u_cx_types.h"

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------- */

struct uCxHandle;

/** Channel Sounding initiator events (u_cx_channel_sounding_initiator.h); not generated. */
typedef void (*uCxCsDistance_t)(struct uCxHandle *puCxHandle, int32_t connHandle, int32_t distanceMm);
typedef void (*uCxCsPct_t)(struct uCxHandle *puCxHandle, int32_t connHandle, int32_t antenna,
                           int32_t validChannels, const uint8_t *pData, size_t dataLen);

typedef struct uCxHandle {
    uCxAtClient_t *pAtClient;
    uUrcCallbacks callbacks;
    uCxCsDistance_t csDistance;
    uCxCsPct_t csPct;
} uCxHandle_t;

/* ----------------------------------------------------------------
 * VARIABLES
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * -------------------------------------------------------------- */

/**
  * @brief Initialize the u-connectXpress API
  *
  * @param[in]  pAtClient:   AT client
  * @param[out] puCxHandle:  the output u-connectXpress API handle
  */
void uCxInit(uCxAtClient_t *pAtClient, uCxHandle_t *puCxHandle);

/**
  * @brief  End AT command started with any of the uCxXXXBegin() functions
  *
  * All functions named uCxXXXBegin() must be terminated by calling this function.
  * The function will wait for the AT status ("OK" or "ERROR").
  *
  * @param[in]  pClient:   the handle from uCxInit().
  * @retval                0 on status OK, -1 on status ERROR, negative value on error.
  */
int32_t uCxEnd(uCxHandle_t *puCxHandle);

#ifdef __cplusplus
}
#endif

#endif // U_CX_H
