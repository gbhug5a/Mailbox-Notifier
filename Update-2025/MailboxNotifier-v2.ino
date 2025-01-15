/*
This is a mailbox notifier sketch for a Lolin D1 Mini or Mini Pro.  It was
compiled using the Arduino IDE v1.8.8 with ESP8266 Core v2.5.2. The Pushover
app is used to send a notification to a smartphone.
*/

#include <ESP8266WiFi.h>                         //automatically includes BearSSL

#define ssid "mySSID"                            //SSID and PW of your router
#define password "myPassword"

const String userkey = "myUserKeyxxxxxxxxxxxxxxxxxxxxx";  //Pushover keys
const String appkey = "myAppKeyxxxxxxxxxxxxxxxxxxxxxx";

String message = "Mailbox Opened";

const int API_TIMEOUT = 15000;
const int httpsPort = 443;
const int powerPin = D2;                         //power kept on by this pin
const char* server = "api.pushover.net";         //Pushover server
String pushParameters;                           //keys and message
bool result;

void setup() {
  pinMode(powerPin, OUTPUT);                     //keep power on
  digitalWrite(powerPin, HIGH);

  delay(3000);
  Serial.begin(57600);                           //all Serial ignored if no USB connection
  delay(100);

  if(connectToWifi()) {                          //if connect ok, send POST to Pushover
    makePushoverRequest();
  }
  WiFi.disconnect();

  Serial.println("Turning off power");
  delay(2000);
  digitalWrite(powerPin, LOW);                   //this will turn off power
  delay(5000);                                   //power should drop before this completes

  // Door Left Open section - *ONLY* if using original circuit, the following section will
  // send a second notification if the door is left open.

  delay(60000);                                  //this completes if power still on
  Serial.println();
  Serial.println();
  Serial.println("Door left open, power still ON, sending notification");
  message = "Door Left Open";                    //send door open notice
  if(connectToWifi()) {                          //if connect ok, send POST to Pushover
    makePushoverRequest();
  }
  WiFi.disconnect();
  Serial.println("Going to sleep now");
  delay(1000);
  ESP.deepSleep(0);                              //power will turn off when door is closed
}

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

// Make an HTTPS request to the Pushover web service

void makePushoverRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);
  pushParameters = "token=" + appkey + "&user=" + userkey + "&message=" + message;

  BearSSL::WiFiClientSecure client;

  for (int tries = 0; tries < 5; tries++) {      //try up to 5 times to connect
    client.setTimeout(API_TIMEOUT);
    client.setInsecure();                        //don't check certificate/fingerprint
    if(client.connect(server, httpsPort)) break; //exit FOR loop if connection
    Serial.print(".");                           //  else wait, try again
    delay(2000);
  }

  Serial.println();
  if(!client.connected()) {
    Serial.println("Failed to connect to server");
    client.stop();
    return;                                      //if no connection, bail out
  }
  Serial.println("Connected");                   //if good connection, send POST

    int length = pushParameters.length();
    Serial.println("Sending push notification");
//    Serial.println(pushParameters);
    client.println("POST /1/messages.json HTTP/1.1");
    client.println("Host: api.pushover.net");
    client.println("Connection: close\r\nContent-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.print(length);
    client.println("\r\n");
    client.print(pushParameters);

  int timeout = 50;                              //wait 5 seconds for a response
  while(!client.available() && (timeout-- > 0)){
    delay(100);
  }

  if(!client.available()) {
     Serial.println("No response to POST");
     client.stop();
     return;
  }
  String response;
  response = client.readStringUntil('\n');
  Serial.println(response);                      // should be 200 if all ok

  while(client.available()){
                                                 // comment one out:
    client.read();                               // skip rest of response, or
//    Serial.print(client.read());                 // print rest of response
  }
  Serial.println("\nClosing connection");
  delay(1000);
  client.stop();
}
