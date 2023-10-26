✅❌❓

# Test matrix - Core
|   |Left|Right|Notes|
|---|---|---|---|
|Keys|✅|✅||
|Pedals|❓|❓||
|Encoders|❓|❓||
|OLED|❓|❓||
|RGB|✅|✅|RGB operates normally with red "breath" pattern, no artifacts visible|
|Flash|✅|✅|QMK can be flashed successfully|
|Audio|❓|❓||
|CC|❓|❓||
|Split comms|❌|❌|QMK can successfully communicate with the other half|
|TRRS shorting|❓|❓||

# Test matrix - Myriad breakout
|   |Left|Right|Notes|
|---|---|---|---|
|5V|✅|❓|Voltage present|
|3V|✅|❓|Voltage present|
|VBUS|✅|❓|Voltage present|
|ADC|✅|❓||
|PWM|✅|❓||
|GPIO|✅|❓|Line idle OK, MCU can write high/low|
|I2C|❓|❓||
|SPI|❓|❓||
|UART|❓|❓||
|RGB out|✅|❓|Default no data present, data present for additional LEDs when configured (use `RGBLIGHT_MODE_SNAKE`)|
|Reset|✅|❓|Host board enters bootloader on tapping twice|
|Presence|✅|❓|`!readPin(MYRIAD_PRESENT)` is `false` when not connected, `true` when shorted to ground|

## RGB out
I previously saw this behaviour, but it seems I can't reproduce it anymore???

PRE line shows 4 blocks (of identical size) starting 1 block earlier,
1 block 1.6 ms later, then 1 block every 5 ms later until 22 ms before next first block

POST line shows 3 data blocks (each 176 us, 432 us apart) every 520 ms

# Test matrix - Myriad modules
|   |Left|Right|Notes|
|---|---|---|---|
|Encoder|❓|❓||
|Buttons|❓|❓||
|Joystick|❓|❓||
|Protoboard|❓|❓||

# Test matrix - Myriad controller
|   |Left|Right|Notes|
|---|---|---|---|
|MCU disable|✅|❓||
|Reset|❓|❓|Pressing host reset button resets the controller card|
|Matrix|❓|❓||
|RGB in|❓|❓||
|OLED|❓|❓||
|Split comms|❓|❓||
|USB|❓|❓||
|CC|❓|❓||
|I2C MCU data|❓|❓|Host emulates Zetta ZD24C02A on address `0x50` (`1010 000 +r/w`)|
