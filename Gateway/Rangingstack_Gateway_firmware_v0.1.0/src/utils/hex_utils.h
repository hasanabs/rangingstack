#ifndef HEX_UTILS_H
#define HEX_UTILS_H
#include <Arduino.h>

/**
 * @brief Converts a hex‐encoded String into bytes.
 *
 * Parses two characters at a time into a uint8_t until maxBytes is reached.
 *
 * @param hexStr   Input hex string.
 * @param byteArray Output array to fill with parsed bytes.
 * @param maxBytes  Number of bytes to write.
 */
extern void hexStringToByteArray(const String &hexStr, uint8_t *byteArray, size_t maxBytes);

/**
 * @brief Converts a byte array into a contiguous hex String.
 *
 * No separators; each byte becomes two hex characters.
 *
 * @param byteArray Input byte array.
 * @param length    Number of bytes to convert.
 * @return Hex-encoded String.
 */
extern String bytesToHexString(uint8_t *byteArray, size_t length);

/**
 * @brief Extracts a little-endian 16-bit address from the first 4 hex chars (2 Bytes).
 *
 * @param hexStr Input hex string.
 * @return Parsed 16-bit address, or 0 on error.
 */
extern uint16_t extractAddrFromHexString(const String &hexStr); // Used for MQTT Response

/**
 * @brief Extracts the remaining bytes after the first two bytes of address.
 *
 * @param hexStr   Input hex string.
 * @param byteArray Output buffer to fill.
 * @param maxBytes  number of bytes to write.
 */
extern void extractRestFromHexString(const String &hexStr, uint8_t *byteArray, size_t maxBytes); // Used for MQTT Response

/**
 * @brief Function to format data byte (payload) as a single string
 *
 *
 * @param source_addr_val    Pointer node address sending the data.
 * @param opcode_val         Pointer operation code determining the action.
 * @param byteArray   Pointer to payload data.
 * @param byteArrayLen Payload length (after opcode).
 * @return Payload (source_addr + opcode + remaining data) as a string.
 */
extern String formatDataAsString(uint8_t *source_addr_val, uint8_t *opcode_val, uint8_t *byteArray, size_t byteArrayLen);

#endif // HEX_UTILS_H