/** @file
 * @brief Channel Sounding initiator (NORA-B26 cs_initiator evaluation firmware, experimental)
 *
 * Hand-written, not generated: the initiator role, the ranging procedure and the distance
 * and PCT events exist only in the NORA-B26 3.5.0 cs_initiator evaluation firmware. The
 * reflector role and the status event are in the generated u_cx_channel_sounding.h.
 * The module computes the distance itself (median filter, optional Kalman filter).
 */

#ifndef U_CX_CHANNEL_SOUNDING_INITIATOR_H
#define U_CX_CHANNEL_SOUNDING_INITIATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "u_cx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Channel Sounding mode value for the initiator role (uCxChannelSoundingSetChannelSoundingMode). */
#define U_CX_CS_MODE_INITIATOR ((uBtChannelSoundingMode_t)2)

/** Bytes per channel entry in the PCT event payload: channel index, iPct and qPct (int32 LE). */
#define U_CX_CS_PCT_ENTRY_SIZE 9

typedef enum {
    U_CX_CS_PROCEDURE_IDLE = 0,     /**< No procedure active. */
    U_CX_CS_PROCEDURE_SETUP = 1,    /**< Setup in progress. */
    U_CX_CS_PROCEDURE_ACTIVE = 2    /**< Ranging active. */
} uCxCsProcedureState_t;

typedef enum {
    U_CX_CS_REFERENCE_OFF = 0,      /**< Real measurement data. */
    U_CX_CS_REFERENCE_500MM = 1,
    U_CX_CS_REFERENCE_1000MM = 2,
    U_CX_CS_REFERENCE_10000MM = 3
} uCxCsReference_t;

/** Kalman filter of the module's distance estimator; Q and R are in units of 0.001. */
typedef struct {
    int32_t processNoise;           /**< Q, 0..1000, default 1 (0.001). */
    int32_t measurementNoise;       /**< R, 10..10000, default 120 (0.12). */
    bool enabled;                   /**< Default true; false reports raw distances. */
} uCxCsKalman_t;

/** Distance event (uCxCsDistance_t in u_cx.h): the module's filtered distance in millimetres.
 *  PCT event (uCxCsPct_t): validChannels entries of U_CX_CS_PCT_ENTRY_SIZE bytes for one antenna path. */

/** Measurement interval in connection intervals (1..65535, default 20). */
int32_t uCxChannelSoundingSetRate(uCxHandle_t *puCxHandle, int32_t rate);
int32_t uCxChannelSoundingGetRate(uCxHandle_t *puCxHandle, int32_t *pRate);

/** Start (true) or stop ranging on a bonded connection; progress comes in the status event. */
int32_t uCxChannelSoundingSetProcedure(uCxHandle_t *puCxHandle, int32_t connHandle, bool enable);
int32_t uCxChannelSoundingGetProcedure(uCxHandle_t *puCxHandle, int32_t connHandle,
                                       uCxCsProcedureState_t *pState);

/** Combined PCT reporting (off by default). */
int32_t uCxChannelSoundingSetPctReporting(uCxHandle_t *puCxHandle, bool enable);
int32_t uCxChannelSoundingGetPctReporting(uCxHandle_t *puCxHandle, bool *pEnable);

/** Replace measured PCT data with a stored reference (debug; needs PCT reporting). */
int32_t uCxChannelSoundingSetReference(uCxHandle_t *puCxHandle, uCxCsReference_t preset);
int32_t uCxChannelSoundingGetReference(uCxHandle_t *puCxHandle, uCxCsReference_t *pPreset);

int32_t uCxChannelSoundingSetKalman(uCxHandle_t *puCxHandle, const uCxCsKalman_t *pKalman);
int32_t uCxChannelSoundingGetKalman(uCxHandle_t *puCxHandle, uCxCsKalman_t *pKalman);

void uCxChannelSoundingRegisterDistance(uCxHandle_t *puCxHandle, uCxCsDistance_t callback);
void uCxChannelSoundingRegisterPct(uCxHandle_t *puCxHandle, uCxCsPct_t callback);

/** Called from the URC dispatcher in u_cx.c; returns true when the line was a Channel Sounding initiator event. */
bool uCxChannelSoundingInitiatorUrc(uCxHandle_t *puCxHandle, const char *pName, char *pParams,
                                    const uint8_t *pBinary, size_t binaryLen);

#ifdef __cplusplus
}
#endif

#endif // U_CX_CHANNEL_SOUNDING_INITIATOR_H
