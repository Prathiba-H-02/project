#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// provide your WLAN set up password and id
// obtain adafruit credentials from adafruit logged in account
// adafruitcredentials required for connecting - server,port, username, key
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "WLANID"
#define WLAN_PASS       "@PASSWORD"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT   1883                   // use 8883 for SSL
#define AIO_USERNAME    "Your Username"
#define AIO_KEY         "Your Key"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish ultrasonic = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/garbage");// garbage is the feedname to which data values are sent
//Adafruit_MQTT_Publish ultrasonic2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/garbage2");

/*************************** Sketch Code ************************************/

void MQTT_connect();
char u[5];
String ultra;

void setup()
{
  Serial.begin(9600);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
  //Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

void loop()
{
  MQTT_connect();
  ultra = "";

  while (NodeSerial.available() > 0)
  {
    char c = NodeSerial.read();
    //Serial.print(c);
    if (c == '*')
    {
      while (NodeSerial.available() > 0)
      {
        char t = NodeSerial.read();
        if (t == '#')
        {
          break;
        }
        ultra += t;
      }
    }


  if ( ultra.length() > 0 )
  {
    for (int i = 0; i < 6; i++)
    {
      u[i] = ultra[i];
    }
    Serial.println(u);
    ultrasonic.publish(u);
    digitalWrite(D4, HIGH);
    delay(2000);
    digitalWrite(D4, LOW);
    ultra = "";
  }

  ultra = "";
  delay(100);
}

void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
