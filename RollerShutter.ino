#include <SPI.h>
#include <Controllino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 1, 198};  // <- change to match your network

int inputState = 0; 
const char* mqtt_server = "46.101.198.211";

char Controllino_RTC_init();
EthernetClient ethClient;
PubSubClient client(ethClient);

long lastMsg = 0;
char msg[50];
int value = 0;
int initalise = 0;


#define shutter1_up_pin CONTROLLINO_D0
#define shutter1_down_pin CONTROLLINO_D1

// Roller shutter states
#define shutter1_idle 0
#define shutter1_going_up 1
#define shutter1_going_down 2

int shutter1State = shutter1_idle;

void setup() {
  Serial.begin(115200);

  Serial.println("Starting bairCon123456");
  Serial.println(Ethernet.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Set relay control pins as outputs
  pinMode(shutter1_up_pin, OUTPUT);
  pinMode(shutter1_down_pin, OUTPUT);

  // Initialize relay control pins to LOW
  digitalWrite(shutter1_up_pin, LOW);
  digitalWrite(shutter1_down_pin, LOW);
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

  // If a message is received on the topic shutter1, you check if the message is either "up" or "down". 
  // Changes the output state according to the message
  if (String(topic) == "shutter1") {
    Serial.print("shutter1 ");
    if(messageTemp == "up"){
      Serial.println("up");
      digitalWrite(shutter1_up_pin LOW);
      delay(100); //small delay to ensure not both outputs are high at the same time     
      digitalWrite(shutter1_down_pin, HIGH);
    }
    else if(messageTemp == "down"){
      Serial.println("down");
      digitalWrite(shutter1_up_pin, LOW);
      delay(100);      
      digitalWrite(shutter1_down_pin, HIGH);
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
      client.subscribe("shutter1");
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