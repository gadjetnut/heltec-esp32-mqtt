/* 
Arduino code for heltec ESP-32 with OLED (www.heltec.cn)
After connection is made to WIFI a connection is made to an MQTT server
A subscription to the "temp" topic is made
MQTT Payload received is display
Works in conjunction with pubtemp.py example for Raspberry Pi & DS18B20 temperature probe

esp_temp_display_mqtt.ino 1.00 
---------------------------------------------------------------------------------                                                                                
 J. Evans        
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                       
                                                                                  
 Revision History                                                                  
 V1.00 - Created       
 ----------------------------------------------------------------------------------
*/

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` 
//#include "SH1106Wire.h" //If you need the 1106 driver then replace above line with this line

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH  128

SSD1306  display(0x3c, 4, 15);
//SH1106Wire display(ADDRESS, SDA, SDC); //If you need the 1106 driver then replace above line with this line

const char* ssid     = "BELL086";          // your network SSID (name of wifi network)
const char* password = "916E73E1A7E4";     // your network password
const char* mqtt_server = "192.168.2.201"; // your mqtt server ip
const int mqtt_port = 1883;                // your mqtt server port
const char* mqtt_topic = "temp";           // topic (don't change this)

WiFiClient client;
PubSubClient mqtt_client(client);

void println_oled(String txtPrint) {
  // Print to the screen
  display.setFont(ArialMT_Plain_10); // create more fonts at http://oleddisplay.squix.ch/
  display.clear();
  display.println(txtPrint);
  // Draw it to the internal screen buffer
  display.drawLogBuffer(0, 0);
  // Display it on the screen
  display.display();
  delay(500);
}

void print_oled(String txtPrint) {
  // Print to the screen
  display.setFont(ArialMT_Plain_10); // create more fonts at http://oleddisplay.squix.ch/
  display.clear();
  display.print(txtPrint);
  // Draw it to the internal screen buffer
  display.drawLogBuffer(0, 0);
  // Display it on the screen
  display.display();
  delay(500);
}

void callback(char* topic, byte * payload, unsigned int  length) {
  int i;
  String strPayload;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24); // create more fonts at http://oleddisplay.squix.ch/
  strPayload="";
  for (int i = 0; i < length; i++) {
    strPayload+=(char)payload[i];
  }
  display.drawString(20, 20, strPayload);
  display.display();
}


void setup() {
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  display.init();
  display.setLogBuffer(5, 30);
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10); // create more fonts at http://oleddisplay.squix.ch/
  println_oled("Connecting to SSID: ");
  println_oled(ssid);
  WiFi.begin(ssid, password);
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    print_oled(".");
    // wait 1 second for re-trying
    delay(1000);
  }
  println_oled("");
  println_oled("Success!");
  delay(1000);
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    println_oled("Making MQTT connection");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(clientId.c_str())) {
      println_oled("Connected!");
      // Once connected, publish an announcement...
      //mqtt_client.publish("outTopic", "hello world");
      // ... and resubscribe
      mqtt_client.subscribe(mqtt_topic);
    } else {
      print_oled("Failed, RC=");
      print_oled(String(mqtt_client.state()));
      println_oled("");
      println_oled("Try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!mqtt_client.connected()) {
    reconnect();
  }
  mqtt_client.loop();
  delay(50);
}



