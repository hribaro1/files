#include <SPI.h>
#include <Controllino.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "Shutter.h"


#define pinUp CONTROLLINO_R0
#define pinDown CONTROLLINO_R1
#define pinUpInput CONTROLLINO_A0
#define pinDownInput CONTROLLINO_A1



byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 1, 198};  // <- change to match your network

const char* mqtt_server = "46.101.198.211";
char Controllino_RTC_init();

int goingUp = 0;
int goingDown = 0;
int buttonStateUp = 0;         // Variable to store the current button state
int lastButtonStateUp = 0;     // Variable to store the last button state
int buttonStateDown = 0;         // Variable to store the current button state
int lastButtonStateDown = 0;     // Variable to store the last button state
int calibrationMode = 0;        //Variable to know wheather calibrationMode is active
int calibrated = 0;

const unsigned long timeLimit = 10000; // Time limit in milliseconds (10 seconds)
unsigned long startTime; // Variable to store the start time
unsigned long startTimeCalibrationUp;
unsigned long startTimeCalibrationDown;
unsigned long calibratedTimeUp;
unsigned long calibratedTimeDown;
unsigned long rollTime;
unsigned long rollStartTime;
int setPosition = 0;
int currentPosition = 0;
int newPosition = 0;

EthernetClient ethClient;
PubSubClient client(ethClient);

Shutter rolo1(pinUp, pinDown);


void setup() {
  Serial.begin(115200);
  Serial.println("Starting bairCon123456");

  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("After ethernet");

  rolo1.init();

  pinMode(pinUpInput, INPUT);
  pinMode(pinDownInput, INPUT);


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
    if (messageTemp == "up") {
      Serial.println("up");
      rolo1.up();
      goingDown = 0;
      goingUp = 1;

    }
    else if (messageTemp == "down") {
      Serial.println("down");
      rolo1.down();
      goingDown = 1;
      goingUp = 0;


    }
    else if (messageTemp == "stop") {
      Serial.println("stop");
      rolo1.stop();
      goingDown = 0;
      goingUp = 0;
    }

    else if ((messageTemp != "up") && (messageTemp != "down") && (messageTemp != "stop")) {
      newPosition = messageTemp.toInt();
      Serial.println("Nastavitev setPosition: ");
      Serial.println(newPosition);
      Serial.println(currentPosition);
      Serial.println(calibratedTimeUp);
      Serial.println(calibratedTimeDown);

      if (newPosition > currentPosition) {
        rollTime = calibratedTimeUp * (newPosition - currentPosition) / 100;
        rollStartTime = millis();
        Serial.print("rollTime: ");
        Serial.println(rollTime);
        Serial.print("rollStartTime: ");
        Serial.println(rollStartTime);
        rolo1.up();
      } else {
        rollTime = calibratedTimeDown * (currentPosition - newPosition) / 100;
        rollStartTime = millis();
        Serial.print("rollTime: ");
        Serial.println(rollTime);
        Serial.print("rollStartTime: ");
        Serial.println(rollStartTime);
        rolo1.down();
      }
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


  if (calibrated) {
    if (calibrationMode == 1) {

    } else {

      if (newPosition != currentPosition) {
        //Serial.print("rolling: ");
        if ((millis() - rollStartTime) > rollTime) {
          rolo1.stop();
          currentPosition = newPosition;
          Serial.print("Nastavljeno na novo pozicijo: ");
          Serial.println(currentPosition);
        }
       } 
    }
    //up, down, stop, setPosition
    //can enter calibrationMode

  } else {

    // if not calibrated
    //up, down, stop
    //can enter calibrationMode

    if (calibrationMode == 1) {           //2nd part of calibration mode registering times to go up and down
      buttonStateDown = digitalRead(pinDownInput);
      if (lastButtonStateDown == LOW && buttonStateDown == HIGH) {
        Serial.println("pinDownInput je HIGH from LOW to HIGH - recording calibratedTimeUp, moving down");
        if (!goingDown) {
          calibratedTimeUp = millis() - startTimeCalibrationUp;
          Serial.println("Calibrated time for shutter to go up: ");
          Serial.println(calibratedTimeUp);
          rolo1.down();
          goingUp = 0;
          goingDown = 1;
          startTimeCalibrationDown = millis();
        } else {
          calibratedTimeDown = millis() - startTimeCalibrationDown;
          rolo1.stop();
          goingDown = 0;
          goingUp = 0;
          Serial.println("stoped goingDown and calibration");
          Serial.println("Calibrated time for shutter to go down: ");
          Serial.println(calibratedTimeDown);
          delay(10);  //short delay, just to avoid double triggering of if clause
          calibrationMode = 0;
          calibrated = 1;
          currentPosition = 0;
          startTime = millis();  //to reset startTime for entering calibrationmode
        }
      }
      lastButtonStateDown = buttonStateDown;


    } else {

      //start and stop going up with manual switch
      buttonStateUp = digitalRead(pinUpInput);
      if (lastButtonStateUp == LOW && buttonStateUp == HIGH) {
        Serial.println("pinUpInput je HIGH from LOW to HIGH");
        if (!goingUp) {
          rolo1.up();
          goingDown = 0;
          goingUp = 1;
          Serial.println("starting goingUp");
        } else {
          rolo1.stop();
          goingDown = 0;
          goingUp = 0;
          Serial.println("stop goingUp");
          delay(10);  //short delay, just to avoid double triggering of if clause
        }
      }
      lastButtonStateUp = buttonStateUp;

      // start and stop going down with manual switch
      buttonStateDown = digitalRead(pinDownInput);
      if (lastButtonStateDown == LOW && buttonStateDown == HIGH) {
        Serial.println("pinDownInput je HIGH from LOW to HIGH");
        if (!goingDown) {
          rolo1.down();
          goingDown = 1;
          goingUp = 0;
          Serial.println("starting goingDown");
        } else {
          rolo1.stop();
          goingDown = 0;
          goingUp = 0;
          Serial.println("stop goingDown");
          delay(10);  //short delay, just to avoid double triggering of if clause
        }
        startTime = millis(); // Record the start time of press
      }

      lastButtonStateDown = buttonStateDown;

      //if we hold button down for more than 10s, we enter calibration mode
      if (buttonStateDown == HIGH) {
        if ((millis() - startTime) >= timeLimit) {
          Serial.println("Entered calibration mode");
          calibrated = 0;
          calibrationMode = 1;
          rolo1.up();
          goingUp = 1;
          goingDown = 0;
          startTimeCalibrationUp = millis();

        }
      }
    }

  }

}
