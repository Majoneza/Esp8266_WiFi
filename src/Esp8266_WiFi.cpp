#include "Esp8266_WiFi.hpp"

#include "utils/BufferUtil.hpp"
#include "utils/ArgumentsUtil.hpp"

bool Esp8266_WiFi::sendBasicCommand(const __FlashStringHelper* command, unsigned long timeout)
{
    auto count = sendCommand(command, buffer, sizeof(buffer), timeout);
    if (count < 4) return false;
    if (buffer[count - 4] != 'O' || buffer[count - 3] != 'K') return false;
    return true;
}

bool Esp8266_WiFi::sendBasicCommand(unsigned long timeout)
{
    auto count = sendCommand(buffer, sizeof(buffer), timeout);
    if (count < 4) return false;
    if (buffer[count - 4] != 'O' || buffer[count - 3] != 'K') return false;
    return true;
}

size_t Esp8266_WiFi::sendBasicCommandWithReply(const __FlashStringHelper* command, unsigned long timeout)
{
    auto count = sendCommand(command, buffer, sizeof(buffer), timeout);
    if (count < 6) return 0;
    if (buffer[count - 4] != 'O' || buffer[count - 3] != 'K') return 0;
    return count - 6;
}

size_t Esp8266_WiFi::sendBasicCommandWithReply(unsigned long timeout)
{
    auto count = sendCommand(buffer, sizeof(buffer), timeout);
    if (count < 6) return 0;
    if (buffer[count - 4] != 'O' || buffer[count - 3] != 'K') return 0;
    return count - 6;
}

bool Esp8266_WiFi::setMode(const Mode mode, const bool auto_connect)
{
    // AT+CWMODE=<mode>[,<auto_connect>]
    if (!createCommand<false>(buffer, sizeof(buffer), PSTR("AT+CWMODE="),
        (int8_t)mode, auto_connect)) return false;
    return sendBasicCommand(500);
}

bool Esp8266_WiFi::setMode(const Mode mode)
{
    // AT+CWMODE=<mode>[,<auto_connect>]
    if (!createCommand<false>(buffer, sizeof(buffer), PSTR("AT+CWMODE="),
        (int8_t)mode)) return false;
    return sendBasicCommand(500);
}

bool Esp8266_WiFi::getMode(Mode& mode)
{
    // AT+CWMODE?
    auto count = sendBasicCommandWithReply(F("AT+CWMODE?"), 500);
    if (count == 0) return false;
    // +CWMODE:<mode>\r\n
    mode = static_cast<Mode>(buffer[8] - '0');
    return true;
}

bool Esp8266_WiFi::getState(State& state)
{
    // AT+CWSTATE?
    auto count = sendBasicCommandWithReply(F("AT+CWSTATE?"), 500);
    if (count == 0) return false;
    // +CWSTATE:<state>,<"ssid">\n\r
    state = static_cast<State>(buffer[9] - '0');
    return true;
}

bool Esp8266_WiFi::connectAP()
{
    // AT+CWJAP
    return Esp8266_WiFi::sendBasicCommand(F("AT+CWJAP"), 1000);
}

bool Esp8266_WiFi::connectAP(const char* ssid, const char* pwd)
{
    // AT+CWJAP=[<ssid>],[<pwd>][,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<scan_mode>][,<jap_timeout>][,<pmf>]
    if (!createCommand<false>(buffer, sizeof(buffer), PSTR("AT+CWJAP="),
        ssid, pwd)) return false;
    return sendBasicCommand(3000);
}

bool Esp8266_WiFi::connectAP(const ConnectArgs& args)
{
    // AT+CWJAP=[<ssid>],[<pwd>][,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<scan_mode>][,<jap_timeout>][,<pmf>]
    if (!createCommand<true>(buffer, sizeof(buffer), PSTR("AT+CWJAP="),
        args.ssid, args.pwd, args.bssid, (int8_t)args.pci_en,
        args.reconn_interval, args.listen_interval, (int8_t)args.scan_mode,
        args.timeout, (int8_t)args.pmf)) return false;
    return sendBasicCommand(3000);
}

bool Esp8266_WiFi::getAP(Connection& connection)
{
    // AT+CWJAP?
    auto c = sendBasicCommandWithReply(F("AT+CWJAP?"), 500);
    if (c == 0) return false;
    // +CWJAP:<ssid>,<bssid>,<channel>,<rssi>,<pci_en>,<reconn_interval>,<listen_interval>,<scan_mode>,<pmf>
    return parseArguments(buffer + 7, c - 7,
        connection.ssid, connection.bssid, connection.channel,
        connection.rssi, (int8_t&)connection.pci_en, connection.reconn_interval,
        connection.listen_interval, (int8_t&)connection.scan_mode, (int8_t&)connection.pmf);
}

bool Esp8266_WiFi::setReconnectConfig(short interval_second, short repeat_count)
{
    // AT+CWRECONNCFG=<interval_second>,<repeat_count>
    if (!createCommand<false>(buffer, sizeof(buffer), PSTR("AT+CWRECONNCFG="),
        interval_second, repeat_count)) return false;
    return sendBasicCommand(500);
}

bool Esp8266_WiFi::getReconnectConfig(ReconnectConfig& config)
{
    // AT+CWRECONNCFG?
    auto c = sendBasicCommandWithReply(F("AT+CWRECONNCFG?"), 500);
    if (c == 0) return false;
    // +CWRECONNCFG:<interval_second>,<repeat_count>
    return parseArguments(buffer + 13, c - 13,
        config.interval_second, config.repeat_count);
}

bool Esp8266_WiFi::setMultipleConnections(bool allowMultiple)
{
    // AT+CIPMUX=<mode>
    return sendBasicCommand(allowMultiple ? F("AT+CIPMUX=1") : F("AT+CIPMUX=0"), 500);
}

bool Esp8266_WiFi::getMultipleConnections(bool& allowMultiple)
{
    // AT+CWMODE?
    auto count = sendBasicCommandWithReply(F("AT+CWMODE?"), 500);
    if (count == 0) return false;
    // +CIPMUX:<mode>\r\n
    allowMultiple = buffer[8] == '1';
    return true;
}

bool Esp8266_WiFi::createServer(const CreateServerArgs& args)
{
    // AT+CIPSERVER=<mode>[,<param2>][,<"type">][,<CA enable>]
    if (!createCommand<true>(buffer, sizeof(buffer), PSTR("AT+CWJAP=1,"),
        args.port, args.type, args.ca_enable)) return false;
    return sendBasicCommand(3000);
}

bool Esp8266_WiFi::deleteServer(const DeleteServerArgs& args)
{
    // AT+CIPSERVER=<mode>[,<param2>][,<"type">][,<CA enable>]
    if (!createCommand<true>(buffer, sizeof(buffer), PSTR("AT+CWJAP=0,"),
        args.closeAll, args.type, args.ca_enable)) return false;
    return sendBasicCommand(3000);
}

bool Esp8266_WiFi::getServerStatus(ServerStatus& status)
{
    // AT+CIPSERVER?
    auto c = sendBasicCommandWithReply(F("AT+CIPSERVER?"), 500);
    if (c == 0) return false;
    // +CIPSERVER:<mode>[,<port>,<"type">][,<CA enable>]
    return parseArguments(buffer + 11, c - 11,
        status.mode, status.port, status.type, status.ca_enable);
}
