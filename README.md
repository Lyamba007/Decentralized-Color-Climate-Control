# Monitor-Light-Control

A prototype of a modified split climate control system, "smart" climate, using a hardware platform, based on a microcontroller device and a server part, based on the HTTP protocol.
The control device is implemented on the basis of the microcontroller ESP-12E (ESP8266) NodeMCU. The microcontroller is programmed in the SI programming language (Wiring) in the Arduino IDE using the standard description of the software implementation in this IDE. The selected microcontroller can be replaced with an Arduino board if further modifications to the human interaction are not associated with Wi-Fi technology.
Based on the protocol of data transmission according to official documentation and the use of modern solutions for interaction with RGB-LEDs, the connection between the MC and the LED strip was implemented using the functionality of the NeoPixel library of Adafruit.
The layout of the outdoor lighting advertising control system is implemented on the basis of a circuit implementation using an LED strip based on WS2812B LEDs.
The results of the layout confirmed the efficiency and effectiveness of the developed microcontroller control device for outdoor illuminated advertising.
____________________________________________________________________________________________________________________________________________
# Repository content
The repository consist of: 

Control device realization on C/C++ (Wiring) based on ESP MCUs series 
Presentation notes in English
Project notes (specification) in Ukrainian
____________________________________________________________________________________________________________________________________________