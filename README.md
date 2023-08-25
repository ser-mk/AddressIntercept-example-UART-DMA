# Address Intercept example UART-DMA

Habrahabr articles https://habr.com/ru/post/433504/ & https://habr.com/ru/post/434080/

This is an example of using [AddressIntercept technology](https://github.com/ser-mk/AddressIntercept "AddressIntercept technology") to control microcontroller periphery from a host PC. The example tunes periphery of microcontroller STM32F103C8 , namely, UART1, UART2, DMA and GPIO LED (PC13 pin). Then UART1 sends byte message and UART2 receives it using DMA. Also the led blinks. At the end, received and sent message are checked.

Here ​Standard Peripherals Library​ is applied for high-level interaction with MCU periphery. Thanks to  [AddressIntercept](https://github.com/ser-mk/AddressIntercept) , we can use this MCU library on a host PC. There is nothing that can prevent linking any library from the  host PC.

 My test stand
 ![alt text](img/stand.jpg "My stand")

The green board is a Linux machine where OpenOCD server was ran.
 `You can run an OpenOCD server on host PC if you use another local USB JTAG/SWD adapter (example st-link, jlink, etc)`
 The blue board is a [bluepill](https://www.google.com/search?client=ubuntu&channel=fs&q=bluepill&ie=utf-8&oe=utf-8 "bluepill").
I connected Tx UART1 (PA9 pin) with Rx UART2 (PA3 pin) using the white wire.

Let's build and run this example...
My host PC is Ubuntu 16.04 64-bit and the test stand above (JTAG/SWD adapter and prepared STM32F103C8 (PA9 pin connected to PA3 pin).

А simple way:

1. Download [pintool v3.7](https://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool "pintool  v3.7") 
2. Unpack archive
3. cd pin-3.7-97619-g0d0c92f4f-gcc-linux/source/tools/
4. git clone git@github.com:ser-mk/AddressIntercept.git
5. cd AddressIntercept
6. make TARGET=obj-ia32 # If you have 64-bit system, need to install some 32-bit packets
7. git clone git@github.com:ser-mk/AddressIntercept-example-UART-DMA.git
8. cd AddressIntercept-example-UART-DMA
9. make
10. cd - # to  pin-3.7-97619-g0d0c92f4f-gcc-linux/source/tools/
11. mkfifo in.fifo out.fifo
12. python3.5m OCDclient.py -ip $IP_YOUR_OPENOCD_SERVER &
13. ../../../pin -t obj-ia32/addrIntercept.so -- AddressIntercept-example-UART-DMA/test.elf

and we'll have:
![alt text](img/uart.png "Show out")


Article in Russian Language https://habr.com/post/434080/
