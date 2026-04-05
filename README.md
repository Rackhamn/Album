![Album Logo Demo (Font is Cinzel Regular by ndiscovered)](/img/Album_logo_demo.png)  

# ALBUM
Album is a handwired 36-key split keyboard with a custom firmware for the Atmega32u4 MCU.  
> Specifically Adafriut's ItsyBitsy Atmega32u4 16MHz 5V version found [here](https://www.adafruit.com/product/3677).  
  
It has a column-stagger with each half having two thumb keys and a [TOTEM](https://github.com/GEIGEIGEIST/TOTEM) inspired pinky key.  
The inital version did have a significant splay on the outer columns and the thumb keys, however my 3D printer had enough issues with layer shifting and thus I had to simplify it for V1.  
  
Album firmware currently runs the master code on the `LEFT` and the slave on the `RIGHT`.  
Master can be connected to any computer over the Atmega32u4 Micro-B port.  
The master and the slave parts can be connected over the TRRS jack.  
  
The Album firmware does not plan to rival others like [QMK](https://github.com/qmk/qmk_firmware), instead it was a challenging but fun process to plan and build out the features that I personally wanted to use.  
A previous `DEMO` version used a single 3x3 macro-pad style keyboard and a [VIAL](https://github.com/vial-kb/vial-qmk)-like keymap updater (through the USB-HID Vendor Reports) that does not need to compile or re-flash the firmware just to update a single keymap value change. It recieved a correct HID Report in a format according to a very simple protocol and overwrote the keymap in `SRAM` but also the stored keymap in `EEPROM`.  
*Very nice*.  
  
I am working on the new version for the V1 release and so far it seems very promising!  

## On the Source  
This is a final project for an __IoT- and Embedded Systems Degree__, the final source will be published at the end of the project.  
The source code will be come with the corresponding dependencies and build scripts for compiling everything along with the USB layer/keymap/macro updater scripts.  
  
## Case and Materials  
The `STL` files can be found [here](/case/).  
The bill-of-materials can be found here `link`.  
  
## Photos  
![Album on Wooden Table](/img/album_table.jpg "Album on Wooden Table")  
![Album on Green Cutting Mat](/img/album_cutmat.jpg "Album on Green Cutting Mat")  

## Credits
Really cool initial inspiration from the TOTEM keyboard in terms of visual design.  
[TOTEM Keyboard by GEIGEIGEIST](https://github.com/GEIGEIGEIST/TOTEM)  

Fantastic website with lots of references for keyboard design!  
[kbd.news](https://kbd.news)  
  
Great jumping off point to be able to start working on a deeper implementation of USB-HID on the Atmega32u4.  
[KMani314's Atmega32u4 HID Keyboard](https://github.com/kmani314/ATMega32u4-HID-Keyboard)  
  
Font is Cinzel Regular by ndiscovered, you can download it [here](https://www.1001fonts.com/cinzel-font.html)  

Switch and Keycaps:  
[Switch (Kailh Choc low-profile white/clicky)](https://chosfox.com/products/kailh-chocs)  
[Keycaps (Choc low-profile 1U white)](https://chosfox.com/products/chocfox-cfx-choc-keycaps)  
> If you are going to make your own, I would recommend using slient tactile switches instead. The clicky ones are slightly echo-y in the V1 case unless you add support foam etc...
  
