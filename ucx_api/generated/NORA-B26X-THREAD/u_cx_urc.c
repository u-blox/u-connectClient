/*
* This file was automatically generated using csnake v0.4.0.
*
* This file should not be edited directly, any changes will be
* overwritten next time the script is run.
*
* Source code for csnake is available at:
* https://gitlab.com/andrejr/csnake
*
* csnake is also available on PyPI, at :
* https://pypi.org/project/csnake
*/
#include <stddef.h>
#include <string.h>
#include "u_cx.h"

/* ------------------------------------------------------------
 * PARSER FUNCTIONS
 * ---------------------------------------------------------- */

static int32_t parseSTARTUP(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "", U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.STARTUP) {
        puCxHandle->callbacks.STARTUP(puCxHandle);
    }
    return ret;
}

static int32_t parseUEBTC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    uBtLeAddress_t bd_addr;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "db", &conn_handle, &bd_addr, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTC) {
        puCxHandle->callbacks.UEBTC(puCxHandle, conn_handle, &bd_addr);
    }
    return ret;
}

static int32_t parseUEBTDC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &conn_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTDC) {
        puCxHandle->callbacks.UEBTDC(puCxHandle, conn_handle);
    }
    return ret;
}

static int32_t parseUEBTB(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    uBtLeAddress_t bd_addr;
    int32_t bond_status;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "bd", &bd_addr, &bond_status, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTB) {
        puCxHandle->callbacks.UEBTB(puCxHandle, &bd_addr, bond_status);
    }
    return ret;
}

static int32_t parseUEBTUC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    uBtLeAddress_t bd_addr;
    int32_t numeric_value;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "bd", &bd_addr, &numeric_value, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTUC) {
        puCxHandle->callbacks.UEBTUC(puCxHandle, &bd_addr, numeric_value);
    }
    return ret;
}

static int32_t parseUEBTUPD(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    uBtLeAddress_t bd_addr;
    int32_t numeric_value;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "bd", &bd_addr, &numeric_value, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTUPD) {
        puCxHandle->callbacks.UEBTUPD(puCxHandle, &bd_addr, numeric_value);
    }
    return ret;
}

static int32_t parseUEBTUPE(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    uBtLeAddress_t bd_addr;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "b", &bd_addr, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTUPE) {
        puCxHandle->callbacks.UEBTUPE(puCxHandle, &bd_addr);
    }
    return ret;
}

static int32_t parseUEBTPHYU(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t phy_status;
    int32_t tx_phy;
    int32_t rx_phy;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dddd", &conn_handle, &phy_status, &tx_phy, &rx_phy, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTPHYU) {
        puCxHandle->callbacks.UEBTPHYU(puCxHandle, conn_handle, phy_status, tx_phy, rx_phy);
    }
    return ret;
}

static int32_t parseUEBTBGD(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    uBtLeAddress_t bd_addr;
    int32_t rssi;
    const char * device_name;
    int32_t data_type;
    uByteArray_t data;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "bdsdh", &bd_addr, &rssi, &device_name, &data_type, &data, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTBGD) {
        puCxHandle->callbacks.UEBTBGD(puCxHandle, &bd_addr, rssi, device_name, data_type, &data);
    }
    return ret;
}

static int32_t parseUEBTGCN(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t value_handle;
    uByteArray_t hex_data;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "ddh", &conn_handle, &value_handle, &hex_data, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTGCN) {
        puCxHandle->callbacks.UEBTGCN(puCxHandle, conn_handle, value_handle, &hex_data);
    }
    return ret;
}

static int32_t parseUEBTGCI(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t value_handle;
    uByteArray_t hex_data;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "ddh", &conn_handle, &value_handle, &hex_data, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTGCI) {
        puCxHandle->callbacks.UEBTGCI(puCxHandle, conn_handle, value_handle, &hex_data);
    }
    return ret;
}

static int32_t parseUEBTGCW(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t value_handle;
    uByteArray_t value;
    int32_t options;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "ddhd", &conn_handle, &value_handle, &value, &options, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTGCW) {
        puCxHandle->callbacks.UEBTGCW(puCxHandle, conn_handle, value_handle, &value, options);
    }
    return ret;
}

static int32_t parseUEBTGRR(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t value_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dd", &conn_handle, &value_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTGRR) {
        puCxHandle->callbacks.UEBTGRR(puCxHandle, conn_handle, value_handle);
    }
    return ret;
}

static int32_t parseUEBTGIC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t char_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dd", &conn_handle, &char_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEBTGIC) {
        puCxHandle->callbacks.UEBTGIC(puCxHandle, conn_handle, char_handle);
    }
    return ret;
}

static int32_t parseUESPSC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &conn_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESPSC) {
        puCxHandle->callbacks.UESPSC(puCxHandle, conn_handle);
    }
    return ret;
}

static int32_t parseUESPSDC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &conn_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESPSDC) {
        puCxHandle->callbacks.UESPSDC(puCxHandle, conn_handle);
    }
    return ret;
}

static int32_t parseUESPSDS(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    uByteArray_t string_data;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d$", &conn_handle, &string_data, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESPSDS) {
        puCxHandle->callbacks.UESPSDS(puCxHandle, conn_handle, &string_data);
    }
    return ret;
}

static int32_t parseUESPSDB(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &conn_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESPSDB) {
        puCxHandle->callbacks.UESPSDB(puCxHandle, conn_handle);
    }
    return ret;
}

static int32_t parseUESPSDA(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t conn_handle;
    int32_t number_bytes;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dd", &conn_handle, &number_bytes, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESPSDA) {
        puCxHandle->callbacks.UESPSDA(puCxHandle, conn_handle, number_bytes);
    }
    return ret;
}

static int32_t parseUESOC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t socket_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &socket_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESOC) {
        puCxHandle->callbacks.UESOC(puCxHandle, socket_handle);
    }
    return ret;
}

static int32_t parseUESODA(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t socket_handle;
    int32_t number_bytes;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dd", &socket_handle, &number_bytes, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESODA) {
        puCxHandle->callbacks.UESODA(puCxHandle, socket_handle, number_bytes);
    }
    return ret;
}

static int32_t parseUESODS(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t socket_handle;
    uByteArray_t string_data;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d$", &socket_handle, &string_data, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESODS) {
        puCxHandle->callbacks.UESODS(puCxHandle, socket_handle, &string_data);
    }
    return ret;
}

static int32_t parseUESODSF(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t socket_handle;
    uSockIpAddress_t remote_ip;
    int32_t remote_port;
    uByteArray_t string_data;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "did$", &socket_handle, &remote_ip, &remote_port, &string_data, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESODSF) {
        puCxHandle->callbacks.UESODSF(puCxHandle, socket_handle, &remote_ip, remote_port, &string_data);
    }
    return ret;
}

static int32_t parseUESODB(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength,
                            uint8_t *pBinaryData, size_t binaryDataLen)
{
    (void)paramsLength;
    int32_t socket_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &socket_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESODB) {
        puCxHandle->callbacks.UESODB(puCxHandle, socket_handle, pBinaryData, binaryDataLen);
    }
    return ret;
}

static int32_t parseUESODBF(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength,
                            uint8_t *pBinaryData, size_t binaryDataLen)
{
    (void)paramsLength;
    int32_t socket_handle;
    uSockIpAddress_t remote_ip;
    int32_t remote_port;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "did", &socket_handle, &remote_ip, &remote_port, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESODBF) {
        puCxHandle->callbacks.UESODBF(puCxHandle, socket_handle, &remote_ip, remote_port, pBinaryData, binaryDataLen);
    }
    return ret;
}

static int32_t parseUESOCL(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t socket_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &socket_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESOCL) {
        puCxHandle->callbacks.UESOCL(puCxHandle, socket_handle);
    }
    return ret;
}

static int32_t parseUESOIC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t socket_handle;
    uSockIpAddress_t remote_ip;
    int32_t listening_socket_handle;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "did", &socket_handle, &remote_ip, &listening_socket_handle, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UESOIC) {
        puCxHandle->callbacks.UESOIC(puCxHandle, socket_handle, &remote_ip, listening_socket_handle);
    }
    return ret;
}

static int32_t parseUEDGPC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t transmitted_packets;
    int32_t received_packets;
    int32_t packet_loss_rate;
    int32_t avg_response_time;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dddd", &transmitted_packets, &received_packets, &packet_loss_rate, &avg_response_time, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEDGPC) {
        puCxHandle->callbacks.UEDGPC(puCxHandle, transmitted_packets, received_packets, packet_loss_rate, avg_response_time);
    }
    return ret;
}

static int32_t parseUEDGP(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t ping_response;
    int32_t response_time;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dd", &ping_response, &response_time, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEDGP) {
        puCxHandle->callbacks.UEDGP(puCxHandle, ping_response, response_time);
    }
    return ret;
}

static int32_t parseUEDGI(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    const char * iperf_output;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "s", &iperf_output, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UEDGI) {
        puCxHandle->callbacks.UEDGI(puCxHandle, iperf_output);
    }
    return ret;
}

static int32_t parseUETHU(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t device_role;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &device_role, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UETHU) {
        puCxHandle->callbacks.UETHU(puCxHandle, device_role);
    }
    return ret;
}

static int32_t parseUETHD(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "", U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UETHD) {
        puCxHandle->callbacks.UETHD(puCxHandle);
    }
    return ret;
}

static int32_t parseUETHRC(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t device_role;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &device_role, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UETHRC) {
        puCxHandle->callbacks.UETHRC(puCxHandle, device_role);
    }
    return ret;
}

static int32_t parseUETHSS(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t service_id;
    int32_t srp_state;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "dd", &service_id, &srp_state, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UETHSS) {
        puCxHandle->callbacks.UETHSS(puCxHandle, service_id, srp_state);
    }
    return ret;
}

static int32_t parseUETHJ(uCxHandle_t * puCxHandle, char * pParams, size_t paramsLength)
{
    (void)paramsLength;
    int32_t joiner_result;
    int32_t ret = uCxAtUtilParseParamsF(pParams, "d", &joiner_result, U_CX_AT_UTIL_PARAM_LAST);
    if ((ret >= 0) && puCxHandle->callbacks.UETHJ) {
        puCxHandle->callbacks.UETHJ(puCxHandle, joiner_result);
    }
    return ret;
}

/* ------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * ---------------------------------------------------------- */
int32_t uCxUrcParse(uCxHandle_t * puCxHandle, const char * pUrcName, char * pParams, size_t paramsLength,
                    uint8_t *pBinaryData, size_t binaryDataLen)
{
    if (strcmp(pUrcName, "+STARTUP") == 0) {
        return parseSTARTUP(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTC") == 0) {
        return parseUEBTC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTDC") == 0) {
        return parseUEBTDC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTB") == 0) {
        return parseUEBTB(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTUC") == 0) {
        return parseUEBTUC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTUPD") == 0) {
        return parseUEBTUPD(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTUPE") == 0) {
        return parseUEBTUPE(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTPHYU") == 0) {
        return parseUEBTPHYU(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTBGD") == 0) {
        return parseUEBTBGD(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTGCN") == 0) {
        return parseUEBTGCN(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTGCI") == 0) {
        return parseUEBTGCI(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTGCW") == 0) {
        return parseUEBTGCW(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTGRR") == 0) {
        return parseUEBTGRR(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEBTGIC") == 0) {
        return parseUEBTGIC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESPSC") == 0) {
        return parseUESPSC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESPSDC") == 0) {
        return parseUESPSDC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESPSDS") == 0) {
        return parseUESPSDS(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESPSDB") == 0) {
        return parseUESPSDB(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESPSDA") == 0) {
        return parseUESPSDA(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESOC") == 0) {
        return parseUESOC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESODA") == 0) {
        return parseUESODA(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESODS") == 0) {
        return parseUESODS(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESODSF") == 0) {
        return parseUESODSF(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESODB") == 0) {
        return parseUESODB(puCxHandle, pParams, paramsLength, pBinaryData, binaryDataLen);
    }
    if (strcmp(pUrcName, "+UESODBF") == 0) {
        return parseUESODBF(puCxHandle, pParams, paramsLength, pBinaryData, binaryDataLen);
    }
    if (strcmp(pUrcName, "+UESOCL") == 0) {
        return parseUESOCL(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UESOIC") == 0) {
        return parseUESOIC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEDGPC") == 0) {
        return parseUEDGPC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEDGP") == 0) {
        return parseUEDGP(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UEDGI") == 0) {
        return parseUEDGI(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UETHU") == 0) {
        return parseUETHU(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UETHD") == 0) {
        return parseUETHD(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UETHRC") == 0) {
        return parseUETHRC(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UETHSS") == 0) {
        return parseUETHSS(puCxHandle, pParams, paramsLength);
    }
    if (strcmp(pUrcName, "+UETHJ") == 0) {
        return parseUETHJ(puCxHandle, pParams, paramsLength);
    }
    return -1;
}

