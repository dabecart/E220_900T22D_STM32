# E220-900T22D driver for the STM32 family of MCUs

*Written by @dabecart, 2025*

## Communication protocol

| Field       | Description                                       | Type      | Byte length   | Byte offset |
|-------------|---------------------------------------------------|-----------|---------------|-------------|
| Header      | Start of message: "$#"                            | char[2]   | 2             | 0           |
| Length      | Message's length: header + payload + CRC (14 + N) | uint16_t  | 2             | 2           |
| Source ID   | ID of the device sending the message              | uint32_t  | 4             | 4           |
| Receiver ID | ID of the device receiving the message            | uint32_t  | 4             | 8           |
| Payload     | Message's content                                 | uint8_t[] | N: 0 to 65535 | 12          |
| CRC         | CRC-CCITT                                         | uint16_t  | 2             | 12+N        |

All types are in little endian.

About the `Receiver ID`:
- Address `0x0000` is the address reserved for the master of the net.
- Address `0xFFFF` is for broadcasting messages.

