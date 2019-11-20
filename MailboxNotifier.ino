/*
This is a mailbox notifier sketch for a Lolin D1 Mini or Mini Pro.  It was
compiled using the Arduino IDE v1.8.8 with ESP8266 Core v2.5.2. Two IFTTT
events are used, the first notifying that the mailbox has been opened, and
after one minute a second notifying that the mailbox door has been left open.
*/

#include <ESP8266WiFi.h>

#define ssid "yourSSID"                          //SSID and PW of your router
#define password "yourpassword"

#define webhookskey "yourwebhookskey"            //enter your Webhooks key here
#define eventname1 "MailboxOpened"               //IFTTT event - mailbox opened
#define eventname2 "DoorLeftOpen"                //IFTTT event - door left open

const int API_TIMEOUT = 15000;
const int httpsPort = 443;
const int powerPin = D2;                         //power kept on by this pin
const char* trigger1 = "/trigger/" eventname1 "/with/key/" webhookskey;
const char* trigger2 = "/trigger/" eventname2 "/with/key/" webhookskey;
char const* event = trigger1;
const char* server = "maker.ifttt.com";
bool result = true;

void setup() {
  pinMode(powerPin, OUTPUT);                     //keep power on
  digitalWrite(powerPin, HIGH);

  delay(5000);
  Serial.begin(57600);                           //ignored if no connection
  delay(100);

  if(connectToWifi()) {                          //if connect ok, send GET
    makeIFTTTRequest();
  }
  WiFi.disconnect();

  Serial.println("Turning off power");
  delay(1000);
  digitalWrite(powerPin, LOW);                   //if door closed, this will
                                                 //   turn off power

  delay(60000);                                  //this completes if power still on
  Serial.println();
  Serial.println();
  Serial.println("Door left open, power still ON, sending notification");
  event = trigger2;                              //send door open notice
  if(connectToWifi()) {
    makeIFTTTRequest();
  }
  WiFi.disconnect();
  Serial.println("Going to sleep now");
  ESP.deepSleep(0);                              //power will turn off when
}                                                //   door eventually closed

void loop(){
}

// Establish WiFi connection to the router

bool connectToWifi() {
  WiFi.disconnect();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);                           //connect as client
  WiFi.begin(ssid, password);                    //connect to router
  Serial.print("Attempting to connect: ");

  int i = 16;                                    //try to connect for 15 seconds
  while((WiFi.status() != WL_CONNECTED) && (i-- > 0)) {
    delay(1000);
    Serial.print(i);
    Serial.print(", ");
  }
  Serial.println();

  //print connection result
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected.");
    Serial.print("D1 Mini IP address: ");
    Serial.println(WiFi.localIP());
    result = true;
  }
  else {
    Serial.println("Connection failed - check your credentials or connection");
    result = false;
  }
  return result;
}

// Make an HTTP request to the IFTTT web service

void makeIFTTTRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);

  BearSSL::WiFiClientSecure client;

  for (int tries = 0; tries < 5; tries++) {        //try up to 5 times to connect
    client.setTimeout(API_TIMEOUT);
    client.setInsecure();                          //don't check fingerprint
    if(client.connect(server, httpsPort)) break;   //exit FOR loop if connection
    Serial.print(".");                             //  else wait, try again
    delay(2000);
  }

  Serial.println();
  if(!client.connected()) {
    Serial.println("Failed to connect to server");
    client.stop();
    return;                                      //if no connection, bail out
  }
  Serial.print("Request event: ");               //if good connection, send GET
  Serial.println(event);
  client.print(String("GET ") + event +
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" +
                  "Connection: close\r\n\r\n");

  int timeout = 50;                              //wait 5 seconds for a response
  while(!client.available() && (timeout-- > 0)){
    delay(100);
  }

  if(!client.available()) {
     Serial.println("No response to GET");
     client.stop();
     return;
  }
  while(client.available()){
    Serial.write(client.read());
  }
  Serial.println("\nclosing connection");
  delay(1000);
  client.stop();
}
