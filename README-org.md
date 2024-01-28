# RC-Light-T815-DKR-86
An Arduino NANO projekt to support HW lights system on my RC model 1:13 of Tatra 815 1986 DAKAR

## Main lights configuration
* front 3 lights circuits
  * consists of 2x, 2x and 8x white led 5mm 20mA
* read 2 break and back light circuits
  * consists of 2x 2 red led 5mm 20ma
* rear 1 reverce curcuit
  * consists of 1x wire led 5mm 20mA

## Light features
It supports 6 states:
 * off
 * front outer 2 leds in bumber + 2 outer rear red leds
 * fron outer and inner bumber leds + 2 outer read red leds
 * all front leds (2+2+8) + 2 outer read red leds
 * maximum brightnes of all front leds (2+2+8)

## States
it reads all 3channels of RC receiver and checks for stgates:
* going forward
* going forward but breaking
* no move
* going back
* going back + breaking


In dependency of there it turns on or off state lights such are breaking red lights or reverse white led

## programming
TBD