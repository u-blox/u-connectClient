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
#ifndef _U_CX_THREAD_H_
#define _U_CX_THREAD_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "u_cx_types.h"
#include "u_cx.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------
 * RESPONSES
 * ---------------------------------------------------------- */

typedef struct
{
    const char * network_name; /**< Thread network name. */
    int32_t channel;           /**< IEEE 802.15.4 channel. */
    int32_t pan_id;            /**< IEEE 802.15.4 PAN ID. */
    uByteArray_t ext_pan_id;   /**< Thread extended PAN ID (8 bytes). */
    uByteArray_t network_key;  /**< Thread network key (16 bytes). */
} uCxThreadGetNetworkCredentials_t;

typedef struct
{
    int32_t device_type; /**< Thread device type. */
    int32_t poll_period; /**< Poll period in milliseconds for a sleepy end device, 10 or more.
                              0 lets the stack choose the poll period. */
} uCxThreadGetDeviceType_t;

typedef struct
{
    int32_t device_role;       /**< Thread device role. */
    int32_t rloc16;            /**< Routing locator (RLOC16) of the device. */
    uByteArray_t partition_id; /**< Thread partition ID (4 bytes). */
    uByteArray_t ext_addr;     /**< IEEE 802.15.4 extended address (8 bytes). */
} uCxThreadGetStatus_t;

typedef struct
{
    uSockIpAddress_t ipv6_addr; /**< IPv6 address. */
    int32_t addr_origin;        /**< Origin of the IPv6 address. */
} uCxThreadListAddresses_t;

typedef struct
{
    uByteArray_t ext_addr; /**< IEEE 802.15.4 extended address (8 bytes). */
    int32_t rloc16;        /**< Routing locator (RLOC16) of the device. */
    int32_t neighbor_role; /**< Role of the neighbor. */
    int32_t rssi;          /**< RSSI in dBm. For neighbors this is the average RSSI. */
    int32_t link_quality;  /**< Link quality in (0-3). */
} uCxThreadListNeighbors_t;

typedef struct
{
    int32_t router_id;        /**< Router ID (0-62). */
    int32_t rloc16;           /**< Routing locator (RLOC16) of the device. */
    uByteArray_t ext_addr;    /**< IEEE 802.15.4 extended address (8 bytes). */
    int32_t next_hop;         /**< Router ID of the next hop towards the router, 63 if there is no route. */
    int32_t path_cost;        /**< Path cost to the router. */
    int32_t link_quality;     /**< Link quality in (0-3). */
    int32_t link_quality_out; /**< Link quality out (0-3). */
    int32_t age;              /**< Seconds since the router was last heard. */
    int32_t link_established; /**< 1 if this device has a direct link to the router, else 0. */
} uCxThreadListRouters_t;

typedef struct
{
    uByteArray_t partition_id;   /**< Thread partition ID (4 bytes). */
    int32_t weighting;           /**< Leader weighting. */
    int32_t data_version;        /**< Full network data version. */
    int32_t stable_data_version; /**< Stable network data version. */
    int32_t leader_router_id;    /**< Router ID of the leader. */
} uCxThreadGetLeaderData_t;

typedef struct
{
    int32_t counter_id;    /**< Thread counter. The counters match the Thread Network Diagnostics cluster of Matter. */
    int32_t counter_value; /**< Counter value. */
} uCxThreadListCounters_t;

typedef struct
{
    const char * network_name; /**< Thread network name. */
    uByteArray_t ext_pan_id;   /**< Thread extended PAN ID (8 bytes). */
    int32_t pan_id;            /**< IEEE 802.15.4 PAN ID. */
    int32_t channel;           /**< IEEE 802.15.4 channel. */
    int32_t rssi;              /**< RSSI in dBm. For neighbors this is the average RSSI. */
    int32_t lqi;               /**< Link quality indicator (0-255) of the received discovery response. */
    uByteArray_t ext_addr;     /**< IEEE 802.15.4 extended address (8 bytes). */
    int32_t thread_version;    /**< Thread version of the network (2 = 1.1, 3 = 1.2, 4 = 1.3, 5 = 1.4). */
    int32_t joinable;          /**< 1 if the network allows joining (Thread commissioning), else 0. */
} uCxThreadScan_t;

typedef struct
{
    int32_t service_id;         /**< Service handle returned by AT+UTHSSA. */
    const char * service_type;  /**< Service type, for example _matter._tcp or _matterc._udp.
                                     The protocol label (_tcp or _udp) must be included.
                                     Subtypes can be appended separated by commas, for example "_matterc._udp,_S3,_L840". */
    int32_t port;               /**< Port number the service listens on. */
    const char * instance_name; /**< Service instance name. */
    int32_t srp_state;          /**< SRP registration state. */
} uCxThreadSrpServiceList_t;

typedef struct
{
    int32_t service_id;     /**< Service handle returned by AT+UTHSSA. */
    const char * txt_key;   /**< TXT record key (e.g. "VP", "D", "SII"). */
    const char * txt_value; /**< TXT record value. */
} uCxThreadSrpTxtRecordList_t;

typedef struct
{
    const char * dns_hostname;  /**< Host name. A name without a dot gets the ".default.service.arpa" domain of SRP
                                     registered hosts appended. */
    int32_t port;               /**< Port number the service listens on. */
    uSockIpAddress_t ipv6_addr; /**< IPv6 address. */
    uByteArray_t txt_data;      /**< TXT record data in DNS format: a sequence of key=value strings, each prefixed
                                     by its length byte. Empty if the service has no TXT record. */
} uCxThreadDnsResolve_t;


/* ------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * ---------------------------------------------------------- */

/**
 * Enable or disable Thread.
 * 
 * Notes:
 * Can be stored using AT&W.
 * 
 * Output AT command:
 * > AT+UTHE=<enabled>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      enabled:    
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSetEnabled(uCxHandle_t * puCxHandle, uEnabled_t enabled);

/**
 * Read if Thread is enabled.
 * 
 * Output AT command:
 * > AT+UTHE?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param[out] pEnabled:   
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadGetEnabled(uCxHandle_t * puCxHandle, uEnabled_t * pEnabled);

/**
 * Set the active dataset.
 * 
 * Output AT command:
 * > AT+UTHDS=<dataset>,<dataset_len>
 *
 * @param[in]  puCxHandle:  uCX API handle
 * @param      dataset:     Thread Operational Dataset in TLV format, as defined by the Thread
 *                          specification. Contains network key, network name, channel, PAN ID,
 *                          extended PAN ID, mesh local prefix, PSKc and security policy.
 * @param      dataset_len: length of dataset
 * @return                  0 on success, negative value on error.
 */
int32_t uCxThreadSetDataset(uCxHandle_t * puCxHandle, const uint8_t * dataset, int32_t dataset_len);

/**
 * Read the active dataset.
 * 
 * Output AT command:
 * > AT+UTHDS?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param[out] pDataset:   Thread Operational Dataset in TLV format, as defined by the Thread
 *                         specification. Contains network key, network name, channel, PAN ID,
 *                         extended PAN ID, mesh local prefix, PSKc and security policy.
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
bool uCxThreadGetDatasetBegin(uCxHandle_t * puCxHandle, uByteArray_t * pDataset);

/**
 * Create a new active dataset.
 * 
 * Output AT command:
 * > AT+UTHDSN
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadDatasetNew(uCxHandle_t * puCxHandle);

/**
 * Clear the Thread network information.
 * 
 * Output AT command:
 * > AT+UTHDSC
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadDatasetClear(uCxHandle_t * puCxHandle);

/**
 * Set the network credentials.
 * 
 * Output AT command:
 * > AT+UTHNC=<network_name>,<channel>,<pan_id>,<ext_pan_id>,<ext_pan_id_len>,<network_key>,<network_key_len>
 *
 * @param[in]  puCxHandle:      uCX API handle
 * @param      network_name:    Thread network name.
 * @param      channel:         IEEE 802.15.4 channel.
 * @param      pan_id:          IEEE 802.15.4 PAN ID.
 * @param      ext_pan_id:      Thread extended PAN ID (8 bytes).
 * @param      ext_pan_id_len:  length of ext_pan_id
 * @param      network_key:     Thread network key (16 bytes).
 * @param      network_key_len: length of network_key
 * @return                      0 on success, negative value on error.
 */
int32_t uCxThreadSetNetworkCredentials(uCxHandle_t * puCxHandle, const char * network_name, int32_t channel, int32_t pan_id, const uint8_t * ext_pan_id, int32_t ext_pan_id_len, const uint8_t * network_key, int32_t network_key_len);

/**
 * Read the network credentials.
 * 
 * Output AT command:
 * > AT+UTHNC?
 *
 * @param[in]  puCxHandle:                      uCX API handle
 * @param[out] pThreadGetNetworkCredentialsRsp: Please see \ref uCxThreadGetNetworkCredentials_t
 * @return                                      true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
bool uCxThreadGetNetworkCredentialsBegin(uCxHandle_t * puCxHandle, uCxThreadGetNetworkCredentials_t * pThreadGetNetworkCredentialsRsp);

/**
 * Set the device type.
 * 
 * Notes:
 * Can be stored using AT&W.
 * 
 * Output AT command:
 * > AT+UTHDT=<device_type>
 *
 * @param[in]  puCxHandle:  uCX API handle
 * @param      device_type: Thread device type.
 * @return                  0 on success, negative value on error.
 */
int32_t uCxThreadSetDeviceType(uCxHandle_t * puCxHandle, uThreadDeviceType_t device_type);

/**
 * Set the device type and the poll period of a sleepy end device.
 * 
 * Notes:
 * Can be stored using AT&W.
 * 
 * Output AT command:
 * > AT+UTHDT=<device_type>,<poll_period>
 *
 * @param[in]  puCxHandle:  uCX API handle
 * @param      device_type: Thread device type.
 * @param      poll_period: Poll period in milliseconds for a sleepy end device, 10 or more.
 *                          0 lets the stack choose the poll period.
 * @return                  0 on success, negative value on error.
 */
int32_t uCxThreadSetDeviceTypePollPeriod(uCxHandle_t * puCxHandle, uThreadDeviceType_t device_type, int32_t poll_period);

/**
 * Read the device type.
 * 
 * Output AT command:
 * > AT+UTHDT?
 *
 * @param[in]  puCxHandle:              uCX API handle
 * @param[out] pThreadGetDeviceTypeRsp: Please see \ref uCxThreadGetDeviceType_t
 * @return                              0 on success, negative value on error.
 */
int32_t uCxThreadGetDeviceType(uCxHandle_t * puCxHandle, uCxThreadGetDeviceType_t * pThreadGetDeviceTypeRsp);

/**
 * Read the Thread status.
 * 
 * Output AT command:
 * > AT+UTHS?
 *
 * @param[in]  puCxHandle:          uCX API handle
 * @param[out] pThreadGetStatusRsp: Please see \ref uCxThreadGetStatus_t
 * @return                          true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
bool uCxThreadGetStatusBegin(uCxHandle_t * puCxHandle, uCxThreadGetStatus_t * pThreadGetStatusRsp);

/**
 * List the IPv6 addresses.
 * 
 * Output AT command:
 * > AT+UTHA?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadListAddressesBegin(uCxHandle_t * puCxHandle);

/**
 * 
 *
 * @param[in]  puCxHandle:              uCX API handle
 * @param[out] pThreadListAddressesRsp: Please see \ref uCxThreadListAddresses_t
 * @return                              true on success, false when there are no more entries or on error (uCxEnd() will return
 *                                      error code in this case).
 */
bool uCxThreadListAddressesGetNext(uCxHandle_t * puCxHandle, uCxThreadListAddresses_t * pThreadListAddressesRsp);

/**
 * List the neighbors.
 * 
 * Output AT command:
 * > AT+UTHNL?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadListNeighborsBegin(uCxHandle_t * puCxHandle);

/**
 * 
 *
 * @param[in]  puCxHandle:              uCX API handle
 * @param[out] pThreadListNeighborsRsp: Please see \ref uCxThreadListNeighbors_t
 * @return                              true on success, false when there are no more entries or on error (uCxEnd() will return
 *                                      error code in this case).
 */
bool uCxThreadListNeighborsGetNext(uCxHandle_t * puCxHandle, uCxThreadListNeighbors_t * pThreadListNeighborsRsp);

/**
 * List the routers.
 * 
 * Output AT command:
 * > AT+UTHRT?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadListRoutersBegin(uCxHandle_t * puCxHandle);

/**
 * 
 *
 * @param[in]  puCxHandle:            uCX API handle
 * @param[out] pThreadListRoutersRsp: Please see \ref uCxThreadListRouters_t
 * @return                            true on success, false when there are no more entries or on error (uCxEnd() will return
 *                                    error code in this case).
 */
bool uCxThreadListRoutersGetNext(uCxHandle_t * puCxHandle, uCxThreadListRouters_t * pThreadListRoutersRsp);

/**
 * Read the leader data.
 * 
 * Output AT command:
 * > AT+UTHLD?
 *
 * @param[in]  puCxHandle:              uCX API handle
 * @param[out] pThreadGetLeaderDataRsp: Please see \ref uCxThreadGetLeaderData_t
 * @return                              true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
bool uCxThreadGetLeaderDataBegin(uCxHandle_t * puCxHandle, uCxThreadGetLeaderData_t * pThreadGetLeaderDataRsp);

/**
 * List all counters.
 * 
 * Output AT command:
 * > AT+UTHCNT?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadListCountersBegin(uCxHandle_t * puCxHandle);

/**
 * 
 *
 * @param[in]  puCxHandle:             uCX API handle
 * @param[out] pThreadListCountersRsp: Please see \ref uCxThreadListCounters_t
 * @return                             true on success, false when there are no more entries or on error (uCxEnd() will return
 *                                     error code in this case).
 */
bool uCxThreadListCountersGetNext(uCxHandle_t * puCxHandle, uCxThreadListCounters_t * pThreadListCountersRsp);

/**
 * Reset all counters.
 * 
 * Output AT command:
 * > AT+UTHCNTR
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadResetCounters(uCxHandle_t * puCxHandle);

/**
 * Start a scan.
 * 
 * Output AT command:
 * > AT+UTHSC
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadScanBegin(uCxHandle_t * puCxHandle);

/**
 * 
 *
 * @param[in]  puCxHandle:     uCX API handle
 * @param[out] pThreadScanRsp: Please see \ref uCxThreadScan_t
 * @return                     true on success, false when there are no more entries or on error (uCxEnd() will return
 *                             error code in this case).
 */
bool uCxThreadScanGetNext(uCxHandle_t * puCxHandle, uCxThreadScan_t * pThreadScanRsp);

/**
 * Start the joiner.
 * 
 * Output AT command:
 * > AT+UTHJS=<pskd>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      pskd:       Joiner pre-shared key (PSKd), 6 to 32 characters of uppercase letters and digits,
 *                         excluding I, O, Q and Z.
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadJoinerStart(uCxHandle_t * puCxHandle, const char * pskd);

/**
 * Stop the joiner.
 * 
 * Output AT command:
 * > AT+UTHJSP
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadJoinerStop(uCxHandle_t * puCxHandle);

/**
 * Enable or disable the SRP client.
 * 
 * Output AT command:
 * > AT+UTHSE=<enabled>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      enabled:    
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSrpSetEnabled(uCxHandle_t * puCxHandle, uEnabled_t enabled);

/**
 * Read if the SRP client is enabled.
 * 
 * Output AT command:
 * > AT+UTHSE?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param[out] pEnabled:   
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSrpGetEnabled(uCxHandle_t * puCxHandle, uEnabled_t * pEnabled);

/**
 * Set the SRP host name.
 * 
 * Output AT command:
 * > AT+UTHSHN=<srp_hostname>
 *
 * @param[in]  puCxHandle:   uCX API handle
 * @param      srp_hostname: SRP host name. The ".default.service.arpa" domain is appended by the SRP server.
 *                           If not set, a name based on the extended address is used.
 * @return                   0 on success, negative value on error.
 */
int32_t uCxThreadSrpSetHostname(uCxHandle_t * puCxHandle, const char * srp_hostname);

/**
 * Read the SRP host name.
 * 
 * Output AT command:
 * > AT+UTHSHN?
 *
 * @param[in]  puCxHandle:    uCX API handle
 * @param[out] ppSrpHostname: SRP host name. The ".default.service.arpa" domain is appended by the SRP server.
 *                            If not set, a name based on the extended address is used.
 * @return                    true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
bool uCxThreadSrpGetHostnameBegin(uCxHandle_t * puCxHandle, const char ** ppSrpHostname);

/**
 * Add a service.
 * 
 * Output AT command:
 * > AT+UTHSSA=<service_type>,<port>,<instance_name>
 *
 * @param[in]  puCxHandle:    uCX API handle
 * @param      service_type:  Service type, for example _matter._tcp or _matterc._udp.
 *                            The protocol label (_tcp or _udp) must be included.
 *                            Subtypes can be appended separated by commas, for example "_matterc._udp,_S3,_L840".
 * @param      port:          Port number the service listens on.
 * @param      instance_name: Service instance name.
 * @param[out] pServiceId:    Service handle returned by AT+UTHSSA.
 * @return                    0 on success, negative value on error.
 */
int32_t uCxThreadSrpServiceAdd(uCxHandle_t * puCxHandle, const char * service_type, int32_t port, const char * instance_name, int32_t * pServiceId);

/**
 * Remove the specified service.
 * 
 * Output AT command:
 * > AT+UTHSSR=<service_id>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      service_id: Service handle returned by AT+UTHSSA.
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSrpServiceRemove(uCxHandle_t * puCxHandle, int32_t service_id);

/**
 * List all services.
 * 
 * Output AT command:
 * > AT+UTHSSL?
 *
 * @param[in]  puCxHandle: uCX API handle
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadSrpServiceListBegin(uCxHandle_t * puCxHandle);

/**
 * 
 *
 * @param[in]  puCxHandle:               uCX API handle
 * @param[out] pThreadSrpServiceListRsp: Please see \ref uCxThreadSrpServiceList_t
 * @return                               true on success, false when there are no more entries or on error (uCxEnd() will return
 *                                       error code in this case).
 */
bool uCxThreadSrpServiceListGetNext(uCxHandle_t * puCxHandle, uCxThreadSrpServiceList_t * pThreadSrpServiceListRsp);

/**
 * Add or update a TXT record.
 * 
 * Output AT command:
 * > AT+UTHSTRA=<service_id>,<txt_key>,<txt_value>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      service_id: Service handle returned by AT+UTHSSA.
 * @param      txt_key:    TXT record key (e.g. "VP", "D", "SII").
 * @param      txt_value:  TXT record value.
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSrpTxtRecordAdd(uCxHandle_t * puCxHandle, int32_t service_id, const char * txt_key, const char * txt_value);

/**
 * Remove the TXT record with the specified key.
 * 
 * Output AT command:
 * > AT+UTHSTRR=<service_id>,<txt_key>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      service_id: Service handle returned by AT+UTHSSA.
 * @param      txt_key:    TXT record key (e.g. "VP", "D", "SII").
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSrpTxtRecordRemove(uCxHandle_t * puCxHandle, int32_t service_id, const char * txt_key);

/**
 * List the TXT records of the specified service.
 * 
 * Output AT command:
 * > AT+UTHSTRL=<service_id>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      service_id: Service handle returned by AT+UTHSSA.
 * @return                 true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadSrpTxtRecordListBegin(uCxHandle_t * puCxHandle, int32_t service_id);

/**
 * 
 *
 * @param[in]  puCxHandle:                 uCX API handle
 * @param[out] pThreadSrpTxtRecordListRsp: Please see \ref uCxThreadSrpTxtRecordList_t
 * @return                                 true on success, false when there are no more entries or on error (uCxEnd() will return
 *                                         error code in this case).
 */
bool uCxThreadSrpTxtRecordListGetNext(uCxHandle_t * puCxHandle, uCxThreadSrpTxtRecordList_t * pThreadSrpTxtRecordListRsp);

/**
 * Clear all TXT records of the specified service.
 * 
 * Output AT command:
 * > AT+UTHSTRC=<service_id>
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      service_id: Service handle returned by AT+UTHSSA.
 * @return                 0 on success, negative value on error.
 */
int32_t uCxThreadSrpTxtRecordClear(uCxHandle_t * puCxHandle, int32_t service_id);

/**
 * Browse for a service type.
 * 
 * Output AT command:
 * > AT+UTHDB=<dns_service_type>
 *
 * @param[in]  puCxHandle:       uCX API handle
 * @param      dns_service_type: Service type with the protocol label, for example _matter._tcp or _matterc._udp.
 *                               Use <subtype>._sub.<service type> to browse a subtype, for example
 *                               _L840._sub._matterc._udp.
 *                               The ".default.service.arpa" domain is appended.
 * @return                       true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadDnsBrowseBegin(uCxHandle_t * puCxHandle, const char * dns_service_type);

/**
 * 
 *
 * @param[in]  puCxHandle:     uCX API handle
 * @param[out] ppInstanceName: Service instance name.
 * @return                     true on success, false when there are no more entries or on error (uCxEnd() will return
 *                             error code in this case).
 */
bool uCxThreadDnsBrowseGetNext(uCxHandle_t * puCxHandle, const char ** ppInstanceName);

/**
 * Resolve a service instance.
 * 
 * Output AT command:
 * > AT+UTHDR=<instance_name>,<dns_service_type>
 *
 * @param[in]  puCxHandle:           uCX API handle
 * @param      instance_name:        Service instance name.
 * @param      dns_service_type:     Service type with the protocol label, for example _matter._tcp or _matterc._udp.
 *                                   Use <subtype>._sub.<service type> to browse a subtype, for example
 *                                   _L840._sub._matterc._udp.
 *                                   The ".default.service.arpa" domain is appended.
 * @param[out] pThreadDnsResolveRsp: Please see \ref uCxThreadDnsResolve_t
 * @return                           true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
bool uCxThreadDnsResolveBegin(uCxHandle_t * puCxHandle, const char * instance_name, const char * dns_service_type, uCxThreadDnsResolve_t * pThreadDnsResolveRsp);

/**
 * Resolve a host name.
 * 
 * Output AT command:
 * > AT+UTHDH=<dns_hostname>
 *
 * @param[in]  puCxHandle:   uCX API handle
 * @param      dns_hostname: Host name. A name without a dot gets the ".default.service.arpa" domain of SRP
 *                           registered hosts appended.
 * @return                   true on success, false on error (error code will be returned by uCxEnd()).
 *
 * NOTES:
 * Must be terminated by calling uCxEnd()
 */
void uCxThreadDnsResolveHostBegin(uCxHandle_t * puCxHandle, const char * dns_hostname);

/**
 * 
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param[out] pIpv6Addr:  IPv6 address.
 * @return                 true on success, false when there are no more entries or on error (uCxEnd() will return
 *                         error code in this case).
 */
bool uCxThreadDnsResolveHostGetNext(uCxHandle_t * puCxHandle, uSockIpAddress_t * pIpv6Addr);

/**
 * Register NetworkUp event callback
 * 
 * Emitted when the module has attached to a Thread network.
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      callback:   callback to register. Set to NULL to unregister.
 */
void uCxThreadRegisterNetworkUp(uCxHandle_t * puCxHandle, uUETHU_t callback);

/**
 * Register NetworkDown event callback
 * 
 * Emitted when the module has detached from the Thread network or Thread was disabled.
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      callback:   callback to register. Set to NULL to unregister.
 */
void uCxThreadRegisterNetworkDown(uCxHandle_t * puCxHandle, uUETHD_t callback);

/**
 * Register RoleChanged event callback
 * 
 * Emitted when the Thread device role changes while attached, for example child to router.
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      callback:   callback to register. Set to NULL to unregister.
 */
void uCxThreadRegisterRoleChanged(uCxHandle_t * puCxHandle, uUETHRC_t callback);

/**
 * Register SrpServiceState event callback
 * 
 * Emitted when the SRP registration state of a service changes.
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      callback:   callback to register. Set to NULL to unregister.
 */
void uCxThreadRegisterSrpServiceState(uCxHandle_t * puCxHandle, uUETHSS_t callback);

/**
 * Register JoinerResult event callback
 * 
 * Emitted when the joiner started with AT+UTHJS has finished.
 *
 * @param[in]  puCxHandle: uCX API handle
 * @param      callback:   callback to register. Set to NULL to unregister.
 */
void uCxThreadRegisterJoinerResult(uCxHandle_t * puCxHandle, uUETHJ_t callback);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _U_CX_THREAD_H_ */