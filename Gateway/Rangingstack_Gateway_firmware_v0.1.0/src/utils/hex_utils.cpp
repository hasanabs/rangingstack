#include "hex_utils.h"

void hexStringToByteArray(const String &hexStr, uint8_t *byteArray, size_t maxBytes)
{
    size_t byteCount = 0;

    for (size_t i = 0; i < hexStr.length() && byteCount < maxBytes; i += 2)
    {
        String byteString = hexStr.substring(i, i + 2);
        byteArray[byteCount++] = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
    }
}

String bytesToHexString(uint8_t *byteArray, size_t length)
{
    String hexString = "";
    for (size_t i = 0; i < length; ++i)
    {
        if (byteArray[i] < 0x10)
            hexString += "0"; // Pad single hex digits with leading zero
        hexString += String(byteArray[i], HEX);
    }
    return hexString;
}

uint16_t extractAddrFromHexString(const String &hexStr)
{ // Usage for taking first two bytes of mqtt response
    if (hexStr.length() < 4)
        return 0;                        // Not enough data
    String msb = hexStr.substring(0, 2); // "03"
    String lsb = hexStr.substring(2, 4); // "00"
    uint8_t high = (uint8_t)strtol(msb.c_str(), nullptr, 16);
    uint8_t low = (uint8_t)strtol(lsb.c_str(), nullptr, 16);
    return (low << 8) | high; //(little-endian)
}

void extractRestFromHexString(const String &hexStr, uint8_t *byteArray, size_t maxBytes)
{ // Usage for extracting the rest bytes of mqtt response
    size_t byteCount = 0;

    for (size_t i = 4; i < hexStr.length() && byteCount < maxBytes; i += 2)
    {
        String byteString = hexStr.substring(i, i + 2);
        byteArray[byteCount++] = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
    }
}

String formatDataAsString(uint8_t *source_addr_val, uint8_t *opcode_val, uint8_t *byteArray, size_t byteArrayLen)
{
    String dataString = bytesToHexString(source_addr_val, 2) + bytesToHexString(opcode_val, 1) + bytesToHexString(byteArray, byteArrayLen);
    return dataString;
}