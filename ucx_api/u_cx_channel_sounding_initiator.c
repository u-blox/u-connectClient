/** @file
 * @brief Channel Sounding initiator (NORA-B26 cs_initiator evaluation firmware, experimental)
 */

#include <string.h>

#include "u_cx_at_client.h"
#include "u_cx_at_util.h"
#include "u_cx_channel_sounding_initiator.h"

static int32_t getInts(uCxHandle_t *puCxHandle, const char *pCmd, const char *pRsp, const char *pFmt,
                       int32_t *pA, int32_t *pB, int32_t *pC)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, pCmd, "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, pRsp, NULL, NULL, pFmt, pA, pB, pC, U_CX_AT_UTIL_PARAM_LAST);
    int32_t endRet = uCxAtClientCmdEnd(pAtClient);
    return ret >= 0 ? endRet : ret;
}

int32_t uCxChannelSoundingSetRate(uCxHandle_t *puCxHandle, int32_t rate)
{
    return uCxAtClientExecSimpleCmdF(puCxHandle->pAtClient, "AT+UBTCSRT=", "d", rate, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxChannelSoundingGetRate(uCxHandle_t *puCxHandle, int32_t *pRate)
{
    return getInts(puCxHandle, "AT+UBTCSRT?", "+UBTCSRT:", "d", pRate, NULL, NULL);
}

int32_t uCxChannelSoundingSetProcedure(uCxHandle_t *puCxHandle, int32_t connHandle, bool enable)
{
    return uCxAtClientExecSimpleCmdF(puCxHandle->pAtClient, "AT+UBTCSP=", "dd", connHandle, enable ? 1 : 0,
                                     U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxChannelSoundingGetProcedure(uCxHandle_t *puCxHandle, int32_t connHandle, uCxCsProcedureState_t *pState)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t state = 0;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UBTCSP=", "d", connHandle, U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UBTCSP:", NULL, NULL, "d", &state, U_CX_AT_UTIL_PARAM_LAST);
    int32_t endRet = uCxAtClientCmdEnd(pAtClient);
    if (ret >= 0) {
        ret = endRet;
    }
    if (ret >= 0) {
        *pState = (uCxCsProcedureState_t)state;
    }
    return ret;
}

int32_t uCxChannelSoundingSetPctReporting(uCxHandle_t *puCxHandle, bool enable)
{
    return uCxAtClientExecSimpleCmdF(puCxHandle->pAtClient, "AT+UBTCSIQ=", "d", enable ? 1 : 0,
                                     U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxChannelSoundingGetPctReporting(uCxHandle_t *puCxHandle, bool *pEnable)
{
    int32_t on = 0;
    int32_t ret = getInts(puCxHandle, "AT+UBTCSIQ?", "+UBTCSIQ:", "d", &on, NULL, NULL);
    if (ret >= 0) {
        *pEnable = on != 0;
    }
    return ret;
}

int32_t uCxChannelSoundingSetReference(uCxHandle_t *puCxHandle, uCxCsReference_t preset)
{
    return uCxAtClientExecSimpleCmdF(puCxHandle->pAtClient, "AT+UBTCSREF=", "d", (int32_t)preset,
                                     U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxChannelSoundingGetReference(uCxHandle_t *puCxHandle, uCxCsReference_t *pPreset)
{
    int32_t preset = 0;
    int32_t ret = getInts(puCxHandle, "AT+UBTCSREF?", "+UBTCSREF:", "d", &preset, NULL, NULL);
    if (ret >= 0) {
        *pPreset = (uCxCsReference_t)preset;
    }
    return ret;
}

int32_t uCxChannelSoundingSetKalman(uCxHandle_t *puCxHandle, const uCxCsKalman_t *pKalman)
{
    return uCxAtClientExecSimpleCmdF(puCxHandle->pAtClient, "AT+UBTCSKF=", "ddd", pKalman->processNoise,
                                     pKalman->measurementNoise, pKalman->enabled ? 1 : 0, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxChannelSoundingGetKalman(uCxHandle_t *puCxHandle, uCxCsKalman_t *pKalman)
{
    int32_t q = 0;
    int32_t r = 0;
    int32_t on = 0;
    int32_t ret = getInts(puCxHandle, "AT+UBTCSKF?", "+UBTCSKF:", "ddd", &q, &r, &on);
    if (ret >= 0) {
        pKalman->processNoise = q;
        pKalman->measurementNoise = r;
        pKalman->enabled = on != 0;
    }
    return ret;
}

void uCxChannelSoundingRegisterDistance(uCxHandle_t *puCxHandle, uCxCsDistance_t callback)
{
    puCxHandle->csDistance = callback;
}

void uCxChannelSoundingRegisterPct(uCxHandle_t *puCxHandle, uCxCsPct_t callback)
{
    puCxHandle->csPct = callback;
}

bool uCxChannelSoundingInitiatorUrc(uCxHandle_t *puCxHandle, const char *pName, char *pParams,
                                    const uint8_t *pBinary, size_t binaryLen)
{
    int32_t conn = 0;
    int32_t a = 0;
    int32_t b = 0;
    if (strcmp(pName, "+UEBTCSM") == 0) {
        if (uCxAtUtilParseParamsF(pParams, "dd", &conn, &a, U_CX_AT_UTIL_PARAM_LAST) >= 0 && puCxHandle->csDistance) {
            puCxHandle->csDistance(puCxHandle, conn, a);
        }
        return true;
    }
    if (strcmp(pName, "+UEBTCSIQ") == 0) {
        if (uCxAtUtilParseParamsF(pParams, "ddd", &conn, &a, &b, U_CX_AT_UTIL_PARAM_LAST) >= 0 && puCxHandle->csPct) {
            /* Never hand out more entries than the payload holds. */
            size_t whole = (pBinary != NULL) ? binaryLen / U_CX_CS_PCT_ENTRY_SIZE : 0;
            if (b < 0 || (size_t)b > whole) {
                b = (int32_t)whole;
            }
            puCxHandle->csPct(puCxHandle, conn, a, b, pBinary, (size_t)b * U_CX_CS_PCT_ENTRY_SIZE);
        }
        return true;
    }
    return false;
}
