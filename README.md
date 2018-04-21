# heltec-esp32-mqtt
Publish temperature readings from Raspberry Pi &amp; DS18B20 Temperature probe to be displayed on the Heltec ESP32 OLED. Requires a WIFI connection and uses MQTT to publish from Raspberry Pi and subscribe and display on the Heltec device OLED

# Installation

1. Raspberry Pi Installation
Follow wiring diagram: <a href="http://goo.gl/zYhm6f">Wiring Diagram</a>

sudo nano /boot/config.txt 
Page down to the bottom of the file and paste or type in these two lines

# 1-wire settings
dtoverlay=w1-gpio,gpiopin=4

Press CTRL-X Y ENTER to save exit. 
Reboot your Pi

Test that the DS18B20 is working:

Type:
 
sudo modprobe w1-gpio
sudo modprobe w1-therm
cd /sys/bus/w1/devices/
ls 

you should see a folders like this:
28-0417506434ff  w1_bus_master1

Copy this folder name for later  : "28-0417506434ff" (yours will be different)

copy pubtemp.py to your Raspberry Pi and edit

sudo nano pubtemp.py

Update these three lines with your details:

mqtt_server = "192.168.2.201" #This is the IP address of your Raspberry Pi
ds18B20 = "28-0417506434ff"   #Enter the folder of your DS18B20 sensor in between the quotes
fahrenheit=True  			        #Change False to True if you want the temperature displayed in Fahrenheit

Press CTRL-X Y ENTER to save exit. 

Install MQTT server:
sudo apt-get install mosquitto

Install PAHO client:
pip install paho-mqtt

Run pubtemp.py:

python pubtemp.py

# Arduino Installation

Copy 




