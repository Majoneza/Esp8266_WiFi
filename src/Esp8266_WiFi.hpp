#pragma once

#include "Esp8266_Communicator.hpp"

enum class Mode : int8_t {
    // 0: Null mode. Wi-Fi RF will be disabled.
    DISABLED = 0,
    // 1: Station mode.
    STATION = 1,
    // 2: SoftAP mode.
    SOFT_AP = 2,
    // 3: SoftAP+Station mode.
    SOFT_AP_STATION = 3
};

enum class State : int8_t {
    // 0: ESP32 station has not started any Wi-Fi connection.
    NOT_CONNECTED = 0,
    // 1: ESP32 station has connected to an AP, but does not get an IPv4 address yet.
    CONNECTED_UNASSIGNED = 1,
    // 2: ESP32 station has connected to an AP, and got an IPv4 address.
    CONNECTED_ASSIGNED = 2,
    // 3: ESP32 station is in Wi-Fi connecting or reconnecting state.
    RE_CONNECTING = 3,
    // 4: ESP32 station is in Wi-Fi disconnected state.
    DISCONNECTED = 4
};

enum class PCIAuthentication : int8_t {
    // 0: The ESP32 station will connect APs with all encryption methods, including OPEN and WEP.
    INCLUDE_OPEN_WEP = 0,
    // 1: The ESP32 station will connect APs with all encryption methods, except OPEN and WEP.
    EXCLUDE_OPEN_WEP = 1
};

enum class ScanMode : int8_t {
    // 0: fast scan. It will end after finding the targeted AP. The ESP32 station will connect to the first scanned AP.
    FAST = 0,
    // 1: all-channel scan. It will end after all the channels are scanned. The device will connect to the scanned AP with the strongest signal.
    FULL = 1
};

enum class PMF : int8_t {
    // 0 means disable PMF.
    DISABLED = 0,
    // bit 0: PMF capable, advertizes support for protected management frame. Device will prefer to connect in PMF mode if other device also advertizes PMF capability.
    SUPPORTED = 1 << 0,
    // bit 1: PMF required, advertizes that protected management frame is required. Device will not associate to non-PMF capable devices.
    REQUIRED = 1 << 1
};

enum class ScanType : int8_t {
    // 0: active scan.
    ACTIVE = 0,
    // 1: passive scan.
    PASSIVE = 1
};

struct Connection {
    // <ssid>: the SSID of the target AP.
    const char* ssid;
    // <bssid>: the MAC address of the target AP. It cannot be omitted when multiple APs have the same SSID.
    const char* bssid;
    // <channel>: channel.
    int8_t channel;
    // <rssi>: signal strength.
    int8_t rssi;
    // <pci_en>: PCI Authentication.
    PCIAuthentication pci_en;
    // <reconn_interval>: the interval between Wi-Fi reconnections. Unit: second. Default: 1. Maximum: 7200.
    short reconn_interval;
    // <listen_interval>: the interval of listening to the AP’s beacon. Unit: AP beacon intervals. Default: 3. Range: [1,100].
    short listen_interval;
    // <scan_mode>: Scan mode.
    ScanMode scan_mode;
    // <pmf>: Protected Management Frames. Default: 1.
    PMF pmf;
};

struct ServerStatus {
    // <mode>: indicates whether the server is operational.
    bool mode;
    // <port>: represents the port number. Default: 333.
    unsigned short port;
    // <type>: server type: “TCP”, “TCPv6”, “SSL”, or “SSLv6”. Default: “TCP”.
    const char* type;
    // <CA enable>: indicates whether CA is enabled.
    bool ca_enable;
};

struct ReconnectConfig {
    // <interval_second>: the interval between Wi-Fi reconnections. Unit: second. Default: 0. Maximum: 7200.
    short interval_second;
    // <repeat_count>: the number of attempts the ESP32 makes to reconnect to the AP. This parameter only works when the parameter <interval_second> is not 0. Default: 0. Maximum: 1000.
    short repeat_count;
};

struct ConnectArgs {
    // <ssid>: the SSID of the target AP.
    const char* ssid = nullptr;
    // <pwd>: password, MAX: 63-byte ASCII.
    const char* pwd = nullptr;
    // <bssid>: the MAC address of the target AP. It cannot be omitted when multiple APs have the same SSID.
    const char* bssid = nullptr;
    // <pci_en>: PCI Authentication.
    PCIAuthentication pci_en = static_cast<PCIAuthentication>(-1);
    // <reconn_interval>: the interval between Wi-Fi reconnections. Unit: second. Default: 1. Maximum: 7200.
    short reconn_interval = -1;
    // <listen_interval>: the interval of listening to the AP’s beacon. Unit: AP beacon intervals. Default: 3. Range: [1,100].
    short listen_interval = -1;
    // <scan_mode>: Scan mode.
    ScanMode scan_mode = static_cast<ScanMode>(-1);
    // <timeout>: maximum timeout for connect command. Unit: second. Default: 15. Range: [3,600].
    short timeout = -1;
    // <pmf>: Protected Management Frames. Default: 1.
    PMF pmf = static_cast<PMF>(-1);
};

struct FetchArgs {
    // <ssid>: string parameter showing SSID of the AP.
    const char* ssid = nullptr;
    // <mac>: string parameter showing MAC address of the AP.
    const char* mac = nullptr;
    // <channel>: channel.
    short channel = -1;
    // <scan_type>: Wi-Fi scan type. Default: 0.
    ScanType scan_type = static_cast<ScanType>(-1);
    // <scan_time_min>: the minimum active scan time per channel. Unit: millisecond. Range [0,1500]. If the scan type is passive, this parameter is invalid.
    short scan_time_min = -1;
    // <scan_time_max>: the maximum active scan time per channel. Unit: millisecond. Range [0,1500]. If this parameter is 0, the firmware will use the default time: 120 ms for active scan; 360 ms for passive scan.
    short scan_time_max = -1;
};

struct CreateServerArgs {
    // <port>: represents the port number. Default: 333.
    unsigned short port = 0;
    // <type>: server type: “TCP”, “TCPv6”, “SSL”, or “SSLv6”. Default: “TCP”.
    const char* type = nullptr;
    // <CA enable>: indicates whether CA is enabled.
    int8_t ca_enable = -1;
};

struct DeleteServerArgs {
    // <closeAll>: represents whether the server closes all connections. Default: 0.
    int8_t closeAll = -1;
    // <type>: server type: “TCP”, “TCPv6”, “SSL”, or “SSLv6”. Default: “TCP”.
    const char* type = nullptr;
    // <CA enable>: indicates whether CA is enabled.
    int8_t ca_enable = -1;
};

class Esp8266_WiFi : public Esp8266_Communicator {
private:
    bool sendBasicCommand(const __FlashStringHelper* command, unsigned long timeout);
    bool sendBasicCommand(unsigned long timeout);
    
    size_t sendBasicCommandWithReply(const __FlashStringHelper* command, unsigned long timeout);
    size_t sendBasicCommandWithReply(unsigned long timeout);
public:
    char buffer[255];

    using Esp8266_Communicator::Esp8266_Communicator;
    
    bool setMode(const Mode mode, const bool auto_connect);
    bool setMode(const Mode mode);
    bool getMode(Mode& mode);

    bool getState(State& state);

    bool connectAP();
    bool connectAP(const char* ssid, const char* pwd);
    bool connectAP(const ConnectArgs& args);
    bool getAP(Connection& connection);

    bool setReconnectConfig(const short interval_second, const short repeat_count);
    bool getReconnectConfig(ReconnectConfig& config);

    bool setMultipleConnections(const bool allowMultiple);
    bool getMultipleConnections(bool& allowMultiple);

    bool createServer(const CreateServerArgs& args);
    bool deleteServer(const DeleteServerArgs& args);
    bool getServerStatus(ServerStatus& status);
};
