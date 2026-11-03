/* Merged from NORA-W36X, NORA-B26X, NORA-B26X-THREAD by ucx_api/merge_api.py; do not edit. */
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
#include <string.h>
#include "u_cx_at_client.h"
#include "u_cx_thread.h"

int32_t uCxThreadSetEnabled(uCxHandle_t * puCxHandle, uEnabled_t enabled)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHE=", "d", enabled, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadGetEnabled(uCxHandle_t * puCxHandle, uEnabled_t * pEnabled)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHE?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHE:", NULL, NULL, "d", pEnabled, U_CX_AT_UTIL_PARAM_LAST);
    {
        // Always call uCxAtClientCmdEnd() even if any previous function failed
        int32_t endRet = uCxAtClientCmdEnd(pAtClient);
        if (ret >= 0) {
            ret = endRet;
        }
    }
    return ret;
}

int32_t uCxThreadSetDataset(uCxHandle_t * puCxHandle, const uint8_t * dataset, int32_t dataset_len)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHDS=", "h", dataset, dataset_len, U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadGetDatasetBegin(uCxHandle_t * puCxHandle, uByteArray_t * pDataset)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHDS?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHDS:", NULL, NULL, "h", pDataset, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadDatasetNew(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHDSN", "", U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadDatasetClear(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHDSC", "", U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadSetNetworkCredentials(uCxHandle_t * puCxHandle, const char * network_name, int32_t channel, int32_t pan_id, const uint8_t * ext_pan_id, int32_t ext_pan_id_len, const uint8_t * network_key, int32_t network_key_len)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHNC=", "sddhh", network_name, channel, pan_id, ext_pan_id, ext_pan_id_len, network_key, network_key_len, U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadGetNetworkCredentialsBegin(uCxHandle_t * puCxHandle, uCxThreadGetNetworkCredentials_t * pThreadGetNetworkCredentialsRsp)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHNC?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHNC:", NULL, NULL, "sddhh", &pThreadGetNetworkCredentialsRsp->network_name, &pThreadGetNetworkCredentialsRsp->channel, &pThreadGetNetworkCredentialsRsp->pan_id, &pThreadGetNetworkCredentialsRsp->ext_pan_id, &pThreadGetNetworkCredentialsRsp->network_key, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadSetDeviceType(uCxHandle_t * puCxHandle, uThreadDeviceType_t device_type)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHDT=", "d", device_type, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadSetDeviceTypePollPeriod(uCxHandle_t * puCxHandle, uThreadDeviceType_t device_type, int32_t poll_period)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHDT=", "dd", device_type, poll_period, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadGetDeviceType(uCxHandle_t * puCxHandle, uCxThreadGetDeviceType_t * pThreadGetDeviceTypeRsp)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHDT?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHDT:", NULL, NULL, "dd", &pThreadGetDeviceTypeRsp->device_type, &pThreadGetDeviceTypeRsp->poll_period, U_CX_AT_UTIL_PARAM_LAST);
    {
        // Always call uCxAtClientCmdEnd() even if any previous function failed
        int32_t endRet = uCxAtClientCmdEnd(pAtClient);
        if (ret >= 0) {
            ret = endRet;
        }
    }
    return ret;
}

bool uCxThreadGetStatusBegin(uCxHandle_t * puCxHandle, uCxThreadGetStatus_t * pThreadGetStatusRsp)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHS?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHS:", NULL, NULL, "ddhh", &pThreadGetStatusRsp->device_role, &pThreadGetStatusRsp->rloc16, &pThreadGetStatusRsp->partition_id, &pThreadGetStatusRsp->ext_addr, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

void uCxThreadListAddressesBegin(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHA?", "", U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadListAddressesGetNext(uCxHandle_t * puCxHandle, uCxThreadListAddresses_t * pThreadListAddressesRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHA:", NULL, NULL, "id", &pThreadListAddressesRsp->ipv6_addr, &pThreadListAddressesRsp->addr_origin, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

void uCxThreadListNeighborsBegin(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHNL?", "", U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadListNeighborsGetNext(uCxHandle_t * puCxHandle, uCxThreadListNeighbors_t * pThreadListNeighborsRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHNL:", NULL, NULL, "hdddd", &pThreadListNeighborsRsp->ext_addr, &pThreadListNeighborsRsp->rloc16, &pThreadListNeighborsRsp->neighbor_role, &pThreadListNeighborsRsp->rssi, &pThreadListNeighborsRsp->link_quality, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

void uCxThreadListRoutersBegin(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHRT?", "", U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadListRoutersGetNext(uCxHandle_t * puCxHandle, uCxThreadListRouters_t * pThreadListRoutersRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHRT:", NULL, NULL, "ddhdddddd", &pThreadListRoutersRsp->router_id, &pThreadListRoutersRsp->rloc16, &pThreadListRoutersRsp->ext_addr, &pThreadListRoutersRsp->next_hop, &pThreadListRoutersRsp->path_cost, &pThreadListRoutersRsp->link_quality, &pThreadListRoutersRsp->link_quality_out, &pThreadListRoutersRsp->age, &pThreadListRoutersRsp->link_established, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

bool uCxThreadGetLeaderDataBegin(uCxHandle_t * puCxHandle, uCxThreadGetLeaderData_t * pThreadGetLeaderDataRsp)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHLD?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHLD:", NULL, NULL, "hdddd", &pThreadGetLeaderDataRsp->partition_id, &pThreadGetLeaderDataRsp->weighting, &pThreadGetLeaderDataRsp->data_version, &pThreadGetLeaderDataRsp->stable_data_version, &pThreadGetLeaderDataRsp->leader_router_id, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

void uCxThreadListCountersBegin(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHCNT?", "", U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadListCountersGetNext(uCxHandle_t * puCxHandle, uCxThreadListCounters_t * pThreadListCountersRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHCNT:", NULL, NULL, "dd", &pThreadListCountersRsp->counter_id, &pThreadListCountersRsp->counter_value, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadResetCounters(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHCNTR", "", U_CX_AT_UTIL_PARAM_LAST);
}

void uCxThreadScanBegin(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHSC", "", U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadScanGetNext(uCxHandle_t * puCxHandle, uCxThreadScan_t * pThreadScanRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHSC:", NULL, NULL, "shddddhdd", &pThreadScanRsp->network_name, &pThreadScanRsp->ext_pan_id, &pThreadScanRsp->pan_id, &pThreadScanRsp->channel, &pThreadScanRsp->rssi, &pThreadScanRsp->lqi, &pThreadScanRsp->ext_addr, &pThreadScanRsp->thread_version, &pThreadScanRsp->joinable, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadJoinerStart(uCxHandle_t * puCxHandle, const char * pskd)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHJS=", "s", pskd, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadJoinerStop(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHJSP", "", U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadSrpSetEnabled(uCxHandle_t * puCxHandle, uEnabled_t enabled)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHSE=", "d", enabled, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadSrpGetEnabled(uCxHandle_t * puCxHandle, uEnabled_t * pEnabled)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHSE?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHSE:", NULL, NULL, "d", pEnabled, U_CX_AT_UTIL_PARAM_LAST);
    {
        // Always call uCxAtClientCmdEnd() even if any previous function failed
        int32_t endRet = uCxAtClientCmdEnd(pAtClient);
        if (ret >= 0) {
            ret = endRet;
        }
    }
    return ret;
}

int32_t uCxThreadSrpSetHostname(uCxHandle_t * puCxHandle, const char * srp_hostname)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHSHN=", "s", srp_hostname, U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadSrpGetHostnameBegin(uCxHandle_t * puCxHandle, const char ** ppSrpHostname)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHSHN?", "", U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHSHN:", NULL, NULL, "s", ppSrpHostname, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadSrpServiceAdd(uCxHandle_t * puCxHandle, const char * service_type, int32_t port, const char * instance_name, int32_t * pServiceId)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHSSA=", "sds", service_type, port, instance_name, U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHSSA:", NULL, NULL, "d", pServiceId, U_CX_AT_UTIL_PARAM_LAST);
    {
        // Always call uCxAtClientCmdEnd() even if any previous function failed
        int32_t endRet = uCxAtClientCmdEnd(pAtClient);
        if (ret >= 0) {
            ret = endRet;
        }
    }
    return ret;
}

int32_t uCxThreadSrpServiceRemove(uCxHandle_t * puCxHandle, int32_t service_id)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHSSR=", "d", service_id, U_CX_AT_UTIL_PARAM_LAST);
}

void uCxThreadSrpServiceListBegin(uCxHandle_t * puCxHandle)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHSSL?", "", U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadSrpServiceListGetNext(uCxHandle_t * puCxHandle, uCxThreadSrpServiceList_t * pThreadSrpServiceListRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHSSL:", NULL, NULL, "dsdsd", &pThreadSrpServiceListRsp->service_id, &pThreadSrpServiceListRsp->service_type, &pThreadSrpServiceListRsp->port, &pThreadSrpServiceListRsp->instance_name, &pThreadSrpServiceListRsp->srp_state, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadSrpTxtRecordAdd(uCxHandle_t * puCxHandle, int32_t service_id, const char * txt_key, const char * txt_value)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHSTRA=", "dss", service_id, txt_key, txt_value, U_CX_AT_UTIL_PARAM_LAST);
}

int32_t uCxThreadSrpTxtRecordRemove(uCxHandle_t * puCxHandle, int32_t service_id, const char * txt_key)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHSTRR=", "ds", service_id, txt_key, U_CX_AT_UTIL_PARAM_LAST);
}

void uCxThreadSrpTxtRecordListBegin(uCxHandle_t * puCxHandle, int32_t service_id)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHSTRL=", "d", service_id, U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadSrpTxtRecordListGetNext(uCxHandle_t * puCxHandle, uCxThreadSrpTxtRecordList_t * pThreadSrpTxtRecordListRsp)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHSTRL:", NULL, NULL, "dss", &pThreadSrpTxtRecordListRsp->service_id, &pThreadSrpTxtRecordListRsp->txt_key, &pThreadSrpTxtRecordListRsp->txt_value, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

int32_t uCxThreadSrpTxtRecordClear(uCxHandle_t * puCxHandle, int32_t service_id)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    return uCxAtClientExecSimpleCmdF(pAtClient, "AT+UTHSTRC=", "d", service_id, U_CX_AT_UTIL_PARAM_LAST);
}

void uCxThreadDnsBrowseBegin(uCxHandle_t * puCxHandle, const char * dns_service_type)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHDB=", "s", dns_service_type, U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadDnsBrowseGetNext(uCxHandle_t * puCxHandle, const char ** ppInstanceName)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHDB:", NULL, NULL, "s", ppInstanceName, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

bool uCxThreadDnsResolveBegin(uCxHandle_t * puCxHandle, const char * instance_name, const char * dns_service_type, uCxThreadDnsResolve_t * pThreadDnsResolveRsp)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    int32_t ret;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHDR=", "ss", instance_name, dns_service_type, U_CX_AT_UTIL_PARAM_LAST);
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHDR:", NULL, NULL, "sdih", &pThreadDnsResolveRsp->dns_hostname, &pThreadDnsResolveRsp->port, &pThreadDnsResolveRsp->ipv6_addr, &pThreadDnsResolveRsp->txt_data, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

void uCxThreadDnsResolveHostBegin(uCxHandle_t * puCxHandle, const char * dns_hostname)
{
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    uCxAtClientCmdBeginF(pAtClient, "AT+UTHDH=", "s", dns_hostname, U_CX_AT_UTIL_PARAM_LAST);
}

bool uCxThreadDnsResolveHostGetNext(uCxHandle_t * puCxHandle, uSockIpAddress_t * pIpv6Addr)
{
    int32_t ret;
    uCxAtClient_t *pAtClient = puCxHandle->pAtClient;
    ret = uCxAtClientCmdGetRspParamsF(pAtClient, "+UTHDH:", NULL, NULL, "i", pIpv6Addr, U_CX_AT_UTIL_PARAM_LAST);
    return ret >= 0;
}

void uCxThreadRegisterNetworkUp(uCxHandle_t * puCxHandle, uUETHU_t callback)
{
    puCxHandle->callbacks.UETHU = callback;
}

void uCxThreadRegisterNetworkDown(uCxHandle_t * puCxHandle, uUETHD_t callback)
{
    puCxHandle->callbacks.UETHD = callback;
}

void uCxThreadRegisterRoleChanged(uCxHandle_t * puCxHandle, uUETHRC_t callback)
{
    puCxHandle->callbacks.UETHRC = callback;
}

void uCxThreadRegisterSrpServiceState(uCxHandle_t * puCxHandle, uUETHSS_t callback)
{
    puCxHandle->callbacks.UETHSS = callback;
}

void uCxThreadRegisterJoinerResult(uCxHandle_t * puCxHandle, uUETHJ_t callback)
{
    puCxHandle->callbacks.UETHJ = callback;
}
