Right half 1:
    Flashes, rgb breathes, types but corrupted
    -> corruption was due to SPI speed with clock divider 2,
       when divider is set to 128 it works just fine

Right half 2:
    Flashes, rgb breathes, no typing
    Shows up in `lsusb`, `hello world` works fine
    CS is fine, CLK only rises 240mV above ground, MISO looks weird too
    Measuring via Myriad port, CLK/MISO have resistance in the 10s of MOhm range to gnd/vcc, so no dead short

    No shorts or bridged pads visible on visual PCB inspection either

    MicroPython test via Myriad breakout
    Wired SDO to GP8, SCK to GP10, MAT to GP6
```python
from machine import Pin, SPI

def readSwitches():
  cs = Pin(6, Pin.OUT, value=1)
  cs.off()
  spi = SPI(1, baudrate=100_000, sck=Pin(10), miso=Pin(8))
  print(spi.read(6))
  cs.on()
```
  This works as expected, the matrix is in order!
  There must be an issue between RP2040 and Myriad socket
  No visible trace damage
  Left side pins of the RP2040 look to be a bit short on solder
    - Seems to be affecting the SCK pin too

  Conclusion: manufacturing error, try hot-air reworking to see if we can fix it

Left half 1:
    shows up to flash, but flashed firmware does not work
    does not show up in `lsusb`, neither qmk nor `hello world`

    Solder bridge at RP2040, QSPI SD0 & SD2 - this of course breaks flashing...

    Matrix works fine when teted via Myriad approach above

Left half 2:
    does not show up to flash

    Solder seems to be missing on right and bottom side of RP2040

```
# minicom -b 115200 -o -D /dev/ttyACM0
```

```python
from machine import Pin
tx = Pin(20, Pin.OUT, value=1)
rx = Pin(21, Pin.IN)
```

```python
from machine import UART, Pin
uart1 = UART(1, baudrate=9600, tx=Pin(20), rx=Pin(21))
uart1.read(5)
```

```python
from machine import UART, Pin
uart1 = UART(1, baudrate=9600, tx=Pin(20), rx=Pin(21))
uart1.write('hello')
```

```python
from machine import Pin
p = Pin(21, Pin.OUT, value=0)
p.toggle()
```

Serial sort-of works, but LOTS of corrupted data???
Weird oscillation?
https://e2e.ti.com/support/logic-group/logic/f/logic-forum/505096/txb0104-oscillation-problem
but that is at 10ns, which is 3 meters??

floating TXB pin? Tie both sides to GND?

https://e2e.ti.com/support/logic-group/logic/f/logic-forum/1130627/txb0104-can-it-be-used-by-connecting-3-components?tisearch=e2e-sitesearch&keymatch=TXB0104#

TXB cannot be connected together?!

Replace with TXU0204 / TXU0103? I thought I looked into this before???.


What happened to the SWD silk screen text????
It shows up in KiCad, but is not in the Gerbers??