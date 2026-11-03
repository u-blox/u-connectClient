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
#ifndef _U_CX_TYPES_H_
#define _U_CX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------
 * ENUMS
 * ---------------------------------------------------------- */

typedef enum
{
    U_GENERAL_GREETING_MODE_OFF,   /**< Turn off the greeting text. */
    U_GENERAL_GREETING_MODE_ON = 1 /**< Turn on the greeting text. */
} uGeneralGreetingMode_t;

typedef enum
{
    U_INTERFACE_ID_BLUETOOTH /**< Bluetooth */
} uInterfaceId_t;

typedef enum
{
    U_SYS_EXTENDED_ERRORS_OFF,   /**< (Factory default) Extended error codes will not be displayed */
    U_SYS_EXTENDED_ERRORS_ON = 1 /**< Extended error code will be displayed on every error */
} uSysExtendedErrors_t;

typedef enum
{
    U_SYS_ECHO_ON_OFF,   /**< Module does not echo the characters */
    U_SYS_ECHO_ON_ON = 1 /**< (Factory default) Module echoes the characters */
} uSysEchoOn_t;

typedef enum
{
    U_BT_MODE_DISABLED,              /**< Disabled. */
    U_BT_MODE_CENTRAL = 1,           /**< Bluetooth Low Energy Central.
                                          In this mode, starting advertisements, direct advertisements and other functions
                                          associated
                                          with the Peripheral role is not possible. */
    U_BT_MODE_PERIPHERAL = 2,        /**< Bluetooth Low Energy Peripheral.
                                          In this mode, initiating connections, discovery and other functions associated with
                                          the Central role is not possible. */
    U_BT_MODE_CENTRAL_PERIPHERAL = 3 /**< Bluetooth Low Energy Simultaneous Central and Peripheral. This is the factory default. */
} uBtMode_t;

typedef enum
{
    U_BT_DATA_TYPE_SCAN_RESPONSE,              /**< Scan response data. */
    U_BT_DATA_TYPE_ADVERTISE_DATA = 1,         /**< Advertise data. */
    U_BT_DATA_TYPE_EXTENDED_ADVERTISE_DATA = 2 /**< Extended advertise data. */
} uBtDataType_t;

typedef enum
{
    U_BT_DISCOVERY_TYPE_ALL,                          /**< Displays all found devices; devices can be displayed multiple times. */
    U_BT_DISCOVERY_TYPE_ALL_NO_DUPLICATES = 1,        /**< Displays all found devices; each device is displayed only once. */
    U_BT_DISCOVERY_TYPE_DATA_FILTER = 2,              /**< Filter discovery results using the configured data filters. Only devices matching a data
                                                           filter configured with AT+UBTDFD will be reported. Devices can be displayed multiple
                                                           times. */
    U_BT_DISCOVERY_TYPE_DATA_FILTER_NO_DUPLICATES = 3 /**< Filter discovery results using the configured data filters. Only devices matching a data
                                                           filter configured with AT+UBTDFD will be reported. Each device is displayed only once. */
} uBtDiscoveryType_t;

typedef enum
{
    U_BT_DISCOVERY_MODE_ACTIVE,     /**< Active discovery. */
    U_BT_DISCOVERY_MODE_PASSIVE = 1 /**< Passive, no scan response data will be received. */
} uBtDiscoveryMode_t;

typedef enum
{
    U_BT_OUTPUT_EVENTS_DISABLED,   /**< Disable output events during background discovery */
    U_BT_OUTPUT_EVENTS_ENABLED = 1 /**< Enable output events during background discovery */
} uBtOutputEvents_t;

typedef enum
{
    U_BT_DISCOVERY_STATE_NOT_ACTIVE, /**< Background discovery is not active */
    U_BT_DISCOVERY_STATE_ACTIVE = 1  /**< Background discovery is active */
} uBtDiscoveryState_t;

typedef enum
{
    U_BT_CONNECT_TO_DIRECTED_ADVERTISEMENTS_DISABLED,   /**< Connect to directed advertisements during background discovery disabled */
    U_BT_CONNECT_TO_DIRECTED_ADVERTISEMENTS_ENABLED = 1 /**< Connect to directed advertisements during background discovery enabled */
} uBtConnectToDirectedAdvertisements_t;

typedef enum
{
    U_BT_PROP_ID_CONNECTION_INTERVAL,       /**< Connection interval used on this connection.
                                                  Range: 6 to 3200
                                                  Time = status_val * 1.25 ms
                                                  Time range: 7.5 ms to 4000 ms */
    U_BT_PROP_ID_PERIPHERAL_LATENCY = 1,    /**< Peripheral latency for the connection in number of connection events. Range: 0 to 499 */
    U_BT_PROP_ID_SUPERVISION_TIMEOUT = 2,   /**< Supervision timeout (in ms) for this connections. Range: 100 ms to 32000 ms */
    U_BT_PROP_ID_MTU_SIZE = 3,              /**< MTU size for this connections. */
    U_BT_PROP_ID_PDU_TX_PAYLOAD_LENGTH = 4, /**< Data Channel TX PDU Payload Length. */
    U_BT_PROP_ID_PDU_RX_PAYLOAD_LENGTH = 5, /**< Data Channel RX PDU Payload Length. */
    U_BT_PROP_ID_DATA_LENGTH_EXTENSION = 6, /**< Data Length Extension state. 0: Data Length Extension Off \ 1: Data Length Extension On */
    U_BT_PROP_ID_LOCAL_ROLE = 7,            /**< Local role in this connection. 1: Low Energy Central \ 2: Low Energy Peripheral */
    U_BT_PROP_ID_TX_PHY = 8,                /**< TX Phy used in this connection
                                                 Bit 0: 1 Mbps
                                                 Bit 1: 2 Mbps
                                                 Bit 2: Coded */
    U_BT_PROP_ID_RX_PHY = 9                 /**< RX Phy used in this connection
                                                 Bit 0: 1 Mbps
                                                 Bit 1: 2 Mbps
                                                 Bit 2: Coded */
} uBtPropId_t;

typedef enum
{
    U_BT_LEGACY_ADV_DISABLED,   /**< Legacy Advertisement Not Running */
    U_BT_LEGACY_ADV_ENABLED = 1 /**< Legacy Advertisement Running */
} uBtLegacyAdv_t;

typedef enum
{
    U_BT_DIRECTED_ADV_DISABLED,   /**< Directed Advertisement Not Running */
    U_BT_DIRECTED_ADV_ENABLED = 1 /**< Directed Advertisement Running */
} uBtDirectedAdv_t;

typedef enum
{
    U_BT_ADV_TYPE_CONNECTABLE_SCANNABLE,            /**< Connectable and Scannable */
    U_BT_ADV_TYPE_NON_CONNECTABLE_SCANNABLE = 1,    /**< Non-Connectable Scannable */
    U_BT_ADV_TYPE_NON_CONNECTABLE_NON_SCANNABLE = 2 /**< Non-Connectable Non-Scannable */
} uBtAdvType_t;

typedef enum
{
    U_BT_EXT_ADV_TYPE_CONNECTABLE,        /**< Connectable */
    U_BT_EXT_ADV_TYPE_NON_CONNECTABLE = 1 /**< Non-Connectable */
} uBtExtAdvType_t;

typedef enum
{
    U_BT_IO_CAP_NO_INPUT_NO_OUTPUT,  /**< Set I/O Capabilities to No Input No Output. */
    U_BT_IO_CAP_DISPLAY_ONLY = 1,    /**< Set I/O Capabilities to Display Only. */
    U_BT_IO_CAP_DISPLAY_YES_NO = 2,  /**< Set I/O Capabilities to Display Yes/No */
    U_BT_IO_CAP_KEYBOARD_ONLY = 3,   /**< Set I/O Capabilities to Keyboard Only. */
    U_BT_IO_CAP_KEYBOARD_DISPLAY = 4 /**< Set I/O Capabilities to Keyboard Display. */
} uBtIoCap_t;

typedef enum
{
    U_BT_SECURITY_MODE_NONE,                                    /**< Security not required. No encryption enforced. */
    U_BT_SECURITY_MODE_UNAUTHENTICATED = 1,                     /**< Require at least unauthenticated bonding. */
    U_BT_SECURITY_MODE_AUTHENTICATED = 2,                       /**< Require authenticated bonding. No secure connections. */
    U_BT_SECURITY_MODE_AUTHENTICATED_SECURE_CONNECTION = 3,     /**< Require authenticated bonding. Support secure connections. Fallback to simple pairing if
                                                                     the remote side does not support secure connections. */
    U_BT_SECURITY_MODE_AUTHENTICATED_SECURE_CONNECTION_ONLY = 4 /**< Require authenticated bonding. Strictly uses secure connections. */
} uBtSecurityMode_t;

typedef enum
{
    U_BT_PAIRING_MODE_DISABLE,   /**< Disable pairing mode. */
    U_BT_PAIRING_MODE_ENABLE = 1 /**< Enable pairing mode. */
} uBtPairingMode_t;

typedef enum
{
    U_BT_CONFIRM_NO,     /**< Deny bonding. */
    U_BT_CONFIRM_YES = 1 /**< Confirm bonding. */
} uBtConfirm_t;

typedef enum
{
    U_BT_CHAR_ID_MANUFACTURER_NAME,     /**< Manufacturer name string. Maximum length of the custom string is 31 characters. */
    U_BT_CHAR_ID_MODEL_NAME = 1,        /**< Model name string. Maximum length of the custom string is 20 characters. */
    U_BT_CHAR_ID_FIRMWARE_REVISION = 2, /**< Firmware revision string. Maximum length of the custom string is 20 characters. */
    U_BT_CHAR_ID_SOFTWARE_REVISION = 3  /**< Software revision string. Maximum length of the custom string is 20 characters. */
} uBtCharId_t;

typedef enum
{
    U_GATT_CLIENT_CONFIG_NONE,                     /**< None */
    U_GATT_CLIENT_CONFIG_ENABLE_NOTIFICATIONS = 1, /**< Enable notifications */
    U_GATT_CLIENT_CONFIG_ENABLE_INDICATIONS = 2,   /**< Enable indications */
    U_GATT_CLIENT_CONFIG_ENABLE_NOT_IND = 3        /**< Enable notifications and indications */
} uGattClientConfig_t;

typedef enum
{
    U_GATT_CLIENT_RELIABLE_NO,     /**< Not reliable */
    U_GATT_CLIENT_RELIABLE_YES = 1 /**< Reliable */
} uGattClientReliable_t;

typedef enum
{
    U_GATT_CLIENT_FLAG_FINAL_DATA,    /**< Final data */
    U_GATT_CLIENT_FLAG_MORE_DATA = 1, /**< More data */
    U_GATT_CLIENT_FLAG_CANCEL = 2     /**< Cancel data writing */
} uGattClientFlag_t;

typedef enum
{
    U_GATT_SERVER_READ_SECURITY_NONE = 1,            /**< No encryption required. */
    U_GATT_SERVER_READ_SECURITY_UNAUTHENTICATED = 2, /**< Unauthenticated encryption required. */
    U_GATT_SERVER_READ_SECURITY_AUTHENTICATED = 3    /**< Authenticated encryption required. */
} uGattServerReadSecurity_t;

typedef enum
{
    U_GATT_SERVER_WRITE_SECURITY_NONE = 1,            /**< No encryption required. */
    U_GATT_SERVER_WRITE_SECURITY_UNAUTHENTICATED = 2, /**< Unauthenticated encryption required. */
    U_GATT_SERVER_WRITE_SECURITY_AUTHENTICATED = 3    /**< Authenticated encryption required. */
} uGattServerWriteSecurity_t;

typedef enum
{
    U_SPS_SERVICE_OPTION_DISABLE,   /**< This option disables the SPS service after saving the configuration and restarting the
                                         device. (Default) */
    U_SPS_SERVICE_OPTION_ENABLE = 1 /**< This option enables the SPS service directly.
                                         If this option is set, and the configuration is saved,
                                         SPS will be enabled after reboot. */
} uSpsServiceOption_t;

typedef enum
{
    U_READ_MODE_BUFFERED,          /**< Buffered mode */
    U_READ_MODE_DIRECT_STRING = 1, /**< Direct String mode */
    U_READ_MODE_DIRECT_BINARY = 2  /**< Direct Binary Mode */
} uReadMode_t;

typedef enum
{
    U_POWER_WAKEUP_MODE_WAKEUP_GPIO,   /**< Wakeup by pulling the module wakeup pin low. */
    U_POWER_WAKEUP_MODE_RESET_PIN = 1, /**< Wakeup by module reset pin. */
    U_POWER_WAKEUP_MODE_UART_RX = 2    /**< Wakeup by module UART RX pin. */
} uPowerWakeupMode_t;

typedef enum
{
    U_SOCKET_PROTOCOL_TCP = 6, /**< TCP */
    U_SOCKET_PROTOCOL_UDP = 17 /**< UDP */
} uSocketProtocol_t;

typedef enum
{
    U_SOCKET_PREF_IP_VER_IPV4,    /**< IPv4 address. */
    U_SOCKET_PREF_IP_VER_IPV6 = 1 /**< IPv6 address. */
} uSocketPrefIpVer_t;

typedef enum
{
    U_WIFI_TLS_VERSION_NO_TLS,              /**< Disable TLS */
    U_WIFI_TLS_VERSION_TLS1_2 = 1,          /**< TLS 1.2 */
    U_WIFI_TLS_VERSION_TLS1_3 = 2,          /**< TLS 1.3 */
    U_WIFI_TLS_VERSION_TLS1_2_OR_TLS1_3 = 3 /**< TLS 1.2 or 1.3 (negotiate highest) */
} uWifiTlsVersion_t;

typedef enum
{
    U_SOCKET_STATUS_NOT_CONNECTED, /**< Not Connected */
    U_SOCKET_STATUS_LISTENING = 1, /**< Listening */
    U_SOCKET_STATUS_CONNECTED = 2  /**< Connected */
} uSocketStatus_t;

typedef enum
{
    U_SOCKET_OPTION_NO_DELAY,       /**< Turn on/off No delay feature for TCP sockets.
                                         Integer flag: 0 = off (i.e. Nagle algorithm enabled), 1 = on  (i.e. Nagle algorithm
                                         disabled)
                                         Default: to 0 */
    U_SOCKET_OPTION_BLOCK = 1,      /**< Set socket to be blocking or non blocking.
                                         Integer flag: 0 = off, 1 = on.
                                         Sockets are non-blocking by default (Note that read/write will always be non-blocking).
                                         Can only be set while the socket is in a non connected state.
                                         Note: Only valid for non-persistent TCP sockets, will have no effect on UDP sockets */
    U_SOCKET_OPTION_KEEP_ALIVE = 2, /**< Keep connections alive by sending keepalive probes.
                                         Integer flag: 0 = off, 1 = on.
                                         To calculate the keepalive time us this formula KeepIdle + (KeepIntvl * KeepCnt).
                                         Defaults to 1.
                                         Note: Only valid for TCP sockets. */
    U_SOCKET_OPTION_KEEP_IDLE = 3,  /**< Set Keep Idle value for the socket.
                                         This specifies the amount of time (in sec) that the connection must be idle before sending
                                         keepalive probes (if keepalive is enabled).
                                         Defaults to 3.
                                         Note: Only valid for TCP sockets. */
    U_SOCKET_OPTION_KEEP_INTVL = 4, /**< Set keep alive interval value for the socket. This is the time in seconds between two
                                         successive keepalive retransmissions.
                                         Defaults to 3.
                                         Note: Only valid for TCP sockets. */
    U_SOCKET_OPTION_KEEP_CNT = 5,   /**< Set keep alive counter value for the socket.
                                         The number of unanswered probes required to force closure of the socket.
                                         Defaults to 3.
                                         Note: Only valid for TCP sockets. */
    U_SOCKET_OPTION_BROADCAST = 6   /**< Set broadcast capability for UDP sockets.
                                         Integer flag: 0 = off, 1 = on.
                                         When enabled, allows sending and receiving UDP packets to/from broadcast addresses.
                                         Defaults to 0 (disabled).
                                         Note: Only valid for UDP sockets. */
} uSocketOption_t;

typedef enum
{
    U_SOCKET_GROUP_ACTION_LEAVE,   /**< Leave the multicast group. */
    U_SOCKET_GROUP_ACTION_JOIN = 1 /**< Join the multicast group. */
} uSocketGroupAction_t;

typedef enum
{
    U_SEC_CERT_TYPE_ROOT,       /**< Root certificate */
    U_SEC_CERT_TYPE_CLIENT = 1, /**< Client certificate */
    U_SEC_CERT_TYPE_KEY = 2     /**< Client private key */
} uSecCertType_t;

typedef enum
{
    U_SEC_CERT_DETAIL_ID_FINGERPRINT,          /**< The fingerprint of the certificate, returns hex_value */
    U_SEC_CERT_DETAIL_ID_CERTIFICATE_SIZE = 1, /**< The size of the certificate, returns int_value */
    U_SEC_CERT_DETAIL_ID_NOT_BEFORE_DATE = 2,  /**< Certificate not valid before date, returns hex_value */
    U_SEC_CERT_DETAIL_ID_NOT_AFTER_DATE = 3    /**< Certificate not valid after date, returns hex_value */
} uSecCertDetailId_t;

typedef enum
{
    U_SEC_EXTENSION_SNI,              /**< Server Name Extension */
    U_SEC_EXTENSION_FRAGMENTATION = 1 /**< Handshake fragmentation */
} uSecExtension_t;

typedef enum
{
    U_ENABLED_NO,     /**< Disabled */
    U_ENABLED_YES = 1 /**< Enabled */
} uEnabled_t;

typedef enum
{
    U_DIAG_IPERF_ACTION_START = 1, /**< Start iperf */
    U_DIAG_IPERF_ACTION_STOP = 2   /**< Stop iperf */
} uDiagIperfAction_t;

typedef enum
{
    U_DIAG_PROTOCOL_TYPE_TCP = 1, /**< TCP */
    U_DIAG_PROTOCOL_TYPE_UDP = 2  /**< UDP */
} uDiagProtocolType_t;

typedef enum
{
    U_DIAG_ROLE_SERVER = 1, /**< Server */
    U_DIAG_ROLE_CLIENT = 2  /**< Client */
} uDiagRole_t;

typedef enum
{
    U_DIAG_BIDIRECTIONAL_OFF,   /**< Off */
    U_DIAG_BIDIRECTIONAL_ON = 1 /**< On
                                     When starting bidirectional TCP test, start a server on both tester and DUT, then start a
                                     client with bidirectional flag on the DUT.
                                     If doing bidirectional UDP test, start a server on both DUT and tester and then start a
                                     client with a bidirectional flag on both. */
} uDiagBidirectional_t;

typedef enum
{
    U_THREAD_DEVICE_TYPE_ROUTER,               /**< Full Thread Device, router eligible. Can form a network and become leader. */
    U_THREAD_DEVICE_TYPE_END_DEVICE = 1,       /**< Minimal End Device, receiver always on. */
    U_THREAD_DEVICE_TYPE_SLEEPY_END_DEVICE = 2 /**< Sleepy End Device, polls its parent with the poll period. */
} uThreadDeviceType_t;

typedef enum
{
    U_THREAD_DEVICE_ROLE_DISABLED,     /**< Thread is disabled. */
    U_THREAD_DEVICE_ROLE_DETACHED = 1, /**< Not attached to a Thread network. */
    U_THREAD_DEVICE_ROLE_CHILD = 2,    /**< Attached as a child (end device). */
    U_THREAD_DEVICE_ROLE_ROUTER = 3,   /**< Attached as a router. */
    U_THREAD_DEVICE_ROLE_LEADER = 4    /**< Attached as the leader of the Thread partition. */
} uThreadDeviceRole_t;

typedef enum
{
    U_THREAD_ADDR_ORIGIN_THREAD,     /**< Thread internal address (link-local, mesh-local EID or RLOC/ALOC). */
    U_THREAD_ADDR_ORIGIN_SLAAC = 1,  /**< Address created with SLAAC from an on-mesh prefix (for example from a Border Router). */
    U_THREAD_ADDR_ORIGIN_DHCPV6 = 2, /**< Address assigned with DHCPv6. */
    U_THREAD_ADDR_ORIGIN_MANUAL = 3  /**< Manually added address. */
} uThreadAddrOrigin_t;

typedef enum
{
    U_THREAD_NEIGHBOR_ROLE_CHILD,     /**< The neighbor is a child of this device. */
    U_THREAD_NEIGHBOR_ROLE_ROUTER = 1 /**< The neighbor is a router. */
} uThreadNeighborRole_t;

typedef enum
{
    U_THREAD_COUNTER_ID_DETACHED_ROLE,                        /**< Number of times the device entered the detached role. */
    U_THREAD_COUNTER_ID_CHILD_ROLE = 1,                       /**< Number of times the device entered the child role. */
    U_THREAD_COUNTER_ID_ROUTER_ROLE = 2,                      /**< Number of times the device entered the router role. */
    U_THREAD_COUNTER_ID_LEADER_ROLE = 3,                      /**< Number of times the device entered the leader role. */
    U_THREAD_COUNTER_ID_ATTACH_ATTEMPTS = 4,                  /**< Number of attach attempts while detached. */
    U_THREAD_COUNTER_ID_PARTITION_ID_CHANGES = 5,             /**< Number of partition ID changes. */
    U_THREAD_COUNTER_ID_BETTER_PARTITION_ATTACH_ATTEMPTS = 6, /**< Number of attempts to attach to a better partition. */
    U_THREAD_COUNTER_ID_PARENT_CHANGES = 7,                   /**< Number of parent changes. */
    U_THREAD_COUNTER_ID_TX_TOTAL = 8,                         /**< Total number of MAC frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_UNICAST = 9,                       /**< Number of unicast MAC frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_BROADCAST = 10,                    /**< Number of broadcast MAC frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_ACK_REQUESTED = 11,                /**< Number of MAC frame transmission requests with acknowledgment requested. */
    U_THREAD_COUNTER_ID_TX_ACKED = 12,                        /**< Number of MAC frame transmission requests that were acknowledged. */
    U_THREAD_COUNTER_ID_TX_NO_ACK_REQUESTED = 13,             /**< Number of MAC frame transmission requests without acknowledgment requested. */
    U_THREAD_COUNTER_ID_TX_DATA = 14,                         /**< Number of MAC data frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_DATA_POLL = 15,                    /**< Number of MAC data poll frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_BEACON = 16,                       /**< Number of MAC beacon frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_BEACON_REQUEST = 17,               /**< Number of MAC beacon request frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_OTHER = 18,                        /**< Number of other MAC frame transmission requests. */
    U_THREAD_COUNTER_ID_TX_RETRY = 19,                        /**< Number of MAC retransmission attempts. */
    U_THREAD_COUNTER_ID_TX_DIRECT_MAX_RETRY_EXPIRY = 20,      /**< Number of direct transmissions that reached the retry limit. */
    U_THREAD_COUNTER_ID_TX_INDIRECT_MAX_RETRY_EXPIRY = 21,    /**< Number of indirect transmissions that reached the retry limit. */
    U_THREAD_COUNTER_ID_TX_ERR_CCA = 22,                      /**< Number of CCA failures. */
    U_THREAD_COUNTER_ID_TX_ERR_ABORT = 23,                    /**< Number of transmissions failed with an abort error. */
    U_THREAD_COUNTER_ID_TX_ERR_BUSY_CHANNEL = 24,             /**< Number of transmissions failed because of a busy channel. */
    U_THREAD_COUNTER_ID_RX_TOTAL = 25,                        /**< Total number of received frames. */
    U_THREAD_COUNTER_ID_RX_UNICAST = 26,                      /**< Number of received unicast frames. */
    U_THREAD_COUNTER_ID_RX_BROADCAST = 27,                    /**< Number of received broadcast frames. */
    U_THREAD_COUNTER_ID_RX_DATA = 28,                         /**< Number of received MAC data frames. */
    U_THREAD_COUNTER_ID_RX_DATA_POLL = 29,                    /**< Number of received MAC data poll frames. */
    U_THREAD_COUNTER_ID_RX_BEACON = 30,                       /**< Number of received MAC beacon frames. */
    U_THREAD_COUNTER_ID_RX_BEACON_REQUEST = 31,               /**< Number of received MAC beacon request frames. */
    U_THREAD_COUNTER_ID_RX_OTHER = 32,                        /**< Number of other received frames. */
    U_THREAD_COUNTER_ID_RX_ADDRESS_FILTERED = 33,             /**< Number of frames dropped by the MAC address filter. */
    U_THREAD_COUNTER_ID_RX_DEST_ADDR_FILTERED = 34,           /**< Number of frames dropped by the destination address check. */
    U_THREAD_COUNTER_ID_RX_DUPLICATED = 35,                   /**< Number of duplicated frames dropped. */
    U_THREAD_COUNTER_ID_RX_ERR_NO_FRAME = 36,                 /**< Number of frames dropped because of missing or malformed content. */
    U_THREAD_COUNTER_ID_RX_ERR_UNKNOWN_NEIGHBOR = 37,         /**< Number of frames dropped because of an unknown neighbor. */
    U_THREAD_COUNTER_ID_RX_ERR_INVALID_SRC_ADDR = 38,         /**< Number of frames dropped because of an invalid source address. */
    U_THREAD_COUNTER_ID_RX_ERR_SEC = 39,                      /**< Number of frames dropped because of a security error. */
    U_THREAD_COUNTER_ID_RX_ERR_FCS = 40,                      /**< Number of frames dropped because of an invalid FCS. */
    U_THREAD_COUNTER_ID_RX_ERR_OTHER = 41                     /**< Number of frames dropped because of other errors. */
} uThreadCounterId_t;

typedef enum
{
    U_THREAD_SRP_STATE_REGISTERING,    /**< Registration (or refresh) with the SRP server is ongoing. */
    U_THREAD_SRP_STATE_REGISTERED = 1, /**< Registered with the SRP server. */
    U_THREAD_SRP_STATE_REMOVING = 2,   /**< Removal from the SRP server is ongoing. */
    U_THREAD_SRP_STATE_REMOVED = 3,    /**< Removed from the SRP server. */
    U_THREAD_SRP_STATE_FAILED = 4      /**< The SRP server rejected the registration (for example name conflict). */
} uThreadSrpState_t;

typedef enum
{
    U_BT_BOND_STATUS_BONDING_SUCCEEDED,                /**< Bonding procedure succeeded. */
    U_BT_BOND_STATUS_BONDING_FAILED_TIMEOUT = 1,       /**< Bonding procedure failed due to page timeout. */
    U_BT_BOND_STATUS_BONDING_FAILED_AUTH = 2,          /**< Bonding failed because of authentication or pairing failed. This could be due to incorrect
                                                            PIN/passkey. */
    U_BT_BOND_STATUS_BONDING_FAILED_MITM = 3,          /**< Bonding failed because the protection against Man-In-The-Middle attack could not be
                                                            guaranteed; the generated link key was too weak. */
    U_BT_BOND_STATUS_BONDING_FAILED_PEER_LOST_BOND = 4 /**< Bonding failed because peer have lost the bonding info. Use AT+UBTUB to delete the local
                                                            bond to allow re-bonding. */
} uBtBondStatus_t;

typedef enum
{
    U_GATT_SERVER_OPTIONS_WRITE_WITH_OUT_RESPONSE, /**< Write without Response performed */
    U_GATT_SERVER_OPTIONS_WRITE_WITH_RESPONSE = 1, /**< Write with Response performed */
    U_GATT_SERVER_OPTIONS_WRITE_LONG = 2           /**< Write long performed */
} uGattServerOptions_t;

typedef enum
{
    U_DIAG_PING_RESPONSE_FALSE,   /**< Ping failed. */
    U_DIAG_PING_RESPONSE_TRUE = 1 /**< Ping succeeded. */
} uDiagPingResponse_t;

typedef enum
{
    U_THREAD_JOINER_RESULT_SUCCESS,       /**< Joined, the dataset is stored. Enable Thread with AT+UTHE to attach. */
    U_THREAD_JOINER_RESULT_SECURITY = 1,  /**< The commissioner rejected the PSKd. */
    U_THREAD_JOINER_RESULT_NOT_FOUND = 2, /**< No joinable network found. */
    U_THREAD_JOINER_RESULT_TIMEOUT = 3,   /**< The commissioner did not respond. */
    U_THREAD_JOINER_RESULT_FAILED = 4     /**< Joining failed for another reason. */
} uThreadJoinerResult_t;

/* ------------------------------------------------------------
 * CALLBACK TYPES
 * ---------------------------------------------------------- */

struct uCxHandle;
typedef void (*uSTARTUP_t)(struct uCxHandle *puCxHandle);
typedef void (*uUEBTC_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, uBtLeAddress_t *bd_addr);
typedef void (*uUEBTDC_t)(struct uCxHandle *puCxHandle, int32_t conn_handle);
typedef void (*uUEBTB_t)(struct uCxHandle *puCxHandle, uBtLeAddress_t *bd_addr, uBtBondStatus_t bond_status);
typedef void (*uUEBTUC_t)(struct uCxHandle *puCxHandle, uBtLeAddress_t *bd_addr, int32_t numeric_value);
typedef void (*uUEBTUPD_t)(struct uCxHandle *puCxHandle, uBtLeAddress_t *bd_addr, int32_t numeric_value);
typedef void (*uUEBTUPE_t)(struct uCxHandle *puCxHandle, uBtLeAddress_t *bd_addr);
typedef void (*uUEBTPHYU_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t phy_status, int32_t tx_phy, int32_t rx_phy);
typedef void (*uUEBTBGD_t)(struct uCxHandle *puCxHandle, uBtLeAddress_t *bd_addr, int32_t rssi, const char * device_name, uBtDataType_t data_type, uByteArray_t *data);
typedef void (*uUEBTGCN_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t value_handle, uByteArray_t *hex_data);
typedef void (*uUEBTGCI_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t value_handle, uByteArray_t *hex_data);
typedef void (*uUEBTGCW_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t value_handle, uByteArray_t *value, uGattServerOptions_t options);
typedef void (*uUEBTGRR_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t value_handle);
typedef void (*uUEBTGIC_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t char_handle);
typedef void (*uUESPSC_t)(struct uCxHandle *puCxHandle, int32_t conn_handle);
typedef void (*uUESPSDC_t)(struct uCxHandle *puCxHandle, int32_t conn_handle);
typedef void (*uUESPSDS_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, uByteArray_t *string_data);
typedef void (*uUESPSDB_t)(struct uCxHandle *puCxHandle, int32_t conn_handle);
typedef void (*uUESPSDA_t)(struct uCxHandle *puCxHandle, int32_t conn_handle, int32_t number_bytes);
typedef void (*uUESOC_t)(struct uCxHandle *puCxHandle, int32_t socket_handle);
typedef void (*uUESODA_t)(struct uCxHandle *puCxHandle, int32_t socket_handle, int32_t number_bytes);
typedef void (*uUESODS_t)(struct uCxHandle *puCxHandle, int32_t socket_handle, uByteArray_t *string_data);
typedef void (*uUESODSF_t)(struct uCxHandle *puCxHandle, int32_t socket_handle, uSockIpAddress_t *remote_ip, int32_t remote_port, uByteArray_t *string_data);
typedef void (*uUESODB_t)(struct uCxHandle *puCxHandle, int32_t socket_handle, uint8_t *pBinaryData, size_t binaryDataLen);
typedef void (*uUESODBF_t)(struct uCxHandle *puCxHandle, int32_t socket_handle, uSockIpAddress_t *remote_ip, int32_t remote_port, uint8_t *pBinaryData, size_t binaryDataLen);
typedef void (*uUESOCL_t)(struct uCxHandle *puCxHandle, int32_t socket_handle);
typedef void (*uUESOIC_t)(struct uCxHandle *puCxHandle, int32_t socket_handle, uSockIpAddress_t *remote_ip, int32_t listening_socket_handle);
typedef void (*uUEDGPC_t)(struct uCxHandle *puCxHandle, int32_t transmitted_packets, int32_t received_packets, int32_t packet_loss_rate, int32_t avg_response_time);
typedef void (*uUEDGP_t)(struct uCxHandle *puCxHandle, uDiagPingResponse_t ping_response, int32_t response_time);
typedef void (*uUEDGI_t)(struct uCxHandle *puCxHandle, const char * iperf_output);
typedef void (*uUETHU_t)(struct uCxHandle *puCxHandle, uThreadDeviceRole_t device_role);
typedef void (*uUETHD_t)(struct uCxHandle *puCxHandle);
typedef void (*uUETHRC_t)(struct uCxHandle *puCxHandle, uThreadDeviceRole_t device_role);
typedef void (*uUETHSS_t)(struct uCxHandle *puCxHandle, int32_t service_id, uThreadSrpState_t srp_state);
typedef void (*uUETHJ_t)(struct uCxHandle *puCxHandle, uThreadJoinerResult_t joiner_result);

/* ------------------------------------------------------------
 * INTERNAL CALLBACK STRUCT
 * ---------------------------------------------------------- */

typedef struct
{
    uSTARTUP_t STARTUP;
    uUEBTC_t UEBTC;
    uUEBTDC_t UEBTDC;
    uUEBTB_t UEBTB;
    uUEBTUC_t UEBTUC;
    uUEBTUPD_t UEBTUPD;
    uUEBTUPE_t UEBTUPE;
    uUEBTPHYU_t UEBTPHYU;
    uUEBTBGD_t UEBTBGD;
    uUEBTGCN_t UEBTGCN;
    uUEBTGCI_t UEBTGCI;
    uUEBTGCW_t UEBTGCW;
    uUEBTGRR_t UEBTGRR;
    uUEBTGIC_t UEBTGIC;
    uUESPSC_t UESPSC;
    uUESPSDC_t UESPSDC;
    uUESPSDS_t UESPSDS;
    uUESPSDB_t UESPSDB;
    uUESPSDA_t UESPSDA;
    uUESOC_t UESOC;
    uUESODA_t UESODA;
    uUESODS_t UESODS;
    uUESODSF_t UESODSF;
    uUESODB_t UESODB;
    uUESODBF_t UESODBF;
    uUESOCL_t UESOCL;
    uUESOIC_t UESOIC;
    uUEDGPC_t UEDGPC;
    uUEDGP_t UEDGP;
    uUEDGI_t UEDGI;
    uUETHU_t UETHU;
    uUETHD_t UETHD;
    uUETHRC_t UETHRC;
    uUETHSS_t UETHSS;
    uUETHJ_t UETHJ;
} uUrcCallbacks;
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _U_CX_TYPES_H_ */