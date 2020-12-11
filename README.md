# Text Meter

This is the GitHub repository for https://buildcomics.com/uncategorized/g-rinder-and-the-textmeter/. It contains arduino code for the arduino nano in the folder named "arduino".\
The used plates and GIMP files are in Plates/

## Parts
Please note that the LTC4946m is pretty rare, but I have a bunch of them lying around. There are however many other 8 segment displays that will work perfectly with an Arduino for this purpose, but they do need different wiring.

## Wiring
The wiring is fairly flexible, use the PIN definitions in the arduino code to change it to your needs. Here is the same info as the arduino file:
```
 Connect DS3231 SCL to A5 on Arduino Nano
 Connect DS3231 SDA to A4 on Arduino Nano
 Connect Button 1 to A1 on Arduino Nano
 Connect Button 2 to A0 on Arduino Nano

Display pinout LTC4946m:
1  2  3   4  5   6  7   8   9  10  11  12  13  14  15
G  C  P1  B  P2  A  P3  F   E   D  CAx CA1 CA2 CA3 CA4
P1     P2     P3        DP1 DP2

P1, P2, P3 triangle red LEDS with Uf=1,7V
dp1, dp2 and green LEDS with Uf = 1,9V

CAx = Common Anode xtra (red P1,P2,P3 points dp1,dp2)
CA1 = Common Anode Digit1
CA2 = Common Anode Digit2
CA3 = Common Anode Digit3
CA4 = Common Anode Digit4

           P1              P2        P3

|--A--|   |--A--|        |--A--|   |--A--|
F     B   F     B  dp1   F     B   F     B
|     |   |     |        |     |   |     |
--G--     --G--          --G--     --G--
|     |   |     |        |     |   |     |
E     C   E     C  dp2   E     C   E     C
|--D--|   |--D--|        |--D--|   |--D--|

Digit1   Digit2         Digit3    Digit4
```
## License
MIT License

Copyright (c) 2020 buildcomics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
