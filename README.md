# Album
Album is a 36-key split keyboard with a custom firmware for the Atmega32u4.  
  
It has a column-stagger, two thumb keys and a TOTEM inspired pinky key. The inital version did have a significant splay on the outer columns and the thumb keys, however my 3D printer had enough issues with layer shifting and thus I had to simplify it for V1.  
  
Album firmware currently runs the master code on the `LEFT` and the slave on the `RIGHT`.  
Master can be connected to any computer over the Atmega32u4 Micro-B port. The master and the slave parts can be connected over the TRRS jack.  

## On the Source  
The source code will be published along with the corresponding dependencies for compiling everything along with the USB layer/keymap/macro updater scripts.  
  
## Case and Materials  
The `STL` files can be found here `link`.  
The bill-of-materials can be found here `link`.  
  
## Photos  
![Album on Wooden Table](/img/album_table.jpg "Album on Wooden Table")  
![Album on Green Cutting Mat](/img/album_cutmat.jpg "Album on Green Cutting Mat")  

## Credits
Really cool initial inspiration from the TOTEM keyboard in terms of visual design.  
[TOTEM Keyboard by GEIGEIGEIST](https://github.com/GEIGEIGEIST/TOTEM)  
  
Great jumping off point to be able to start working on a deeper implementation of USB-HID on the Atmega32u4.  
[KMani314's Atmega32u4 HID Keyboard](https://github.com/kmani314/ATMega32u4-HID-Keyboard)
