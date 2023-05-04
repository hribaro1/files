#include <SPI.h>
#include <Controllino.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "Shutter.h"


#define pinUp CONTROLLINO_D0
#define pinDown CONTROLLINO_D1


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 1, 198};  // <- change to match your network

const char* mqtt_server = "46.101.198.211";
char Controllino_RTC_init();

EthernetClient ethClient;
PubSubClient client(ethClient);
Shutter rolo1(pinUp, pinDown);

// Roller shutter states

void setup() {
  Serial.begin(115200);
  Serial.println("Starting bairCon123456");
  
  Ethernet.begin(mac,ip);
  Serial.println(Ethernet.localIP());
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("After ethernet");

  rolo1.init();

  //pinMode(shutter1_up_pin, OUTPUT);
  //pinMode(shutter1_down_pin, OUTPUT);

  // Initialize relay control pins to LOW
  //digitalWrite(shutter1_up_pin, LOW);
  //digitalWrite(shutter1_down_pin, LOW);
}



void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message

  if (String(topic) == "rolo1") {
    Serial.print("rolo1");
    if(messageTemp == "up"){
      Serial.println("up");
      rolo1.up();
    }
    else if(messageTemp == "down"){
      Serial.println("down");
      rolo1.down();

    }
    else if(messageTemp == "stop"){
      Serial.println("stop");
      rolo1.stop();
    }
  }

  }



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Controllino")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("rolo1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// the loop function runs over and over again forever
void loop() {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();


}
