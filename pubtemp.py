#!/usr/bin/env python
"""
pubtemp.py 1.00 Dallas 1 wire interface to MQTT
---------------------------------------------------------------------------------                                                                                
 J. Evans 2018  
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                       
                                                                                  
 Revision History                                                                  
 V1.00 - Created       
 ----------------------------------------------------------------------------------
"""

import paho.mqtt.client as paho
import time
import subprocess

import paho.mqtt.client as paho

mqtt_server = "192.168.2.201" #Enter the IP address of your MQTT server in between the quotes
ds18B20 = "28-0417506434ff"   #Enter the folder of your DS18B20 sensor in between the quotes
fahrenheit=True               #Change False to True if you want the temperature displayed in Fahrenheit

def fileexists(filename):
        try:
                with open(filename): pass
        except IOError:
                return False 
        return True

def GetTemperature(no):
	#Routine to read the temperature
	#Read the sensor 10 times checking the CRC until we have a good read 
	for retries in range(0, 10):        
			subprocess.call(['modprobe', 'w1-gpio'])
			subprocess.call(['modprobe', 'w1-therm'])
			
			# Open the file that we viewed earlier so that python can see what is in it. Replace the serial number as before.
			filename = "/sys/bus/w1/devices/"+ds18B20+"/w1_slave"
			if (fileexists(filename)):
					tfile = open(filename)
			else:
					return 0
			# Read all of the text in the file.
			text = tfile.read()
			# Close the file now that the text has been read.
			tfile.close()
			#Perform a CRC Check
			firstline  = text.split("\n")[0]
			crc_check = text.split("crc=")[1]
			crc_check = crc_check.split(" ")[1]
			if crc_check.find("YES")>=0:
					break
	#If after 10 tries we were unable to get a good read return 0
	if retries==9:
			return(0)
	# Split the text with new lines (\n) and select the second line.
	secondline = text.split("\n")[1]
	# Split the line into words, referring to the spaces, and select the 10th word (counting from 0).
	temperaturedata = secondline.split(" ")[9]
	# The first two characters are "t=", so get rid of those and convert the temperature from a string to a number.
	temperature = float(temperaturedata[2:])
	# Put the decimal point in the right place and display it.
	temperature = temperature / 1000
	temp = float(temperature)
	# Do the Farhenheit conversion if required
	if fahrenheit:
			temp=temp*1.8+32
	temp = "{0:.2f}".format(round(temp,2))
	if fahrenheit:
		temp+="F"
	else:
		temp+="C"
	return(temp)

def main():
	PrevPayload=""
	while True:
		MsgPayload = GetTemperature(0);
		if MsgPayload <> PrevPayload:
			client = paho.Client("pi_temp")
			client.connect(mqtt_server, port=1883)
			client.publish("temp", MsgPayload)
			client.disconnect()
			PrevPayload = MsgPayload
		time.sleep(1)        
	
if __name__ == "__main__":   # run the program from the command line
	main()
