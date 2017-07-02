// testing to see how to use POST with a web page
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "martel";
const char WiFiPSK[] = "mpahrilleinpe";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, blue LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

const char Page[] = ("\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\r\n\
<!DOCTYPE HTML>\r\n<html><head><title>\r\n\
Thing Dev Board Web Page</title>\r\n\
</head><body>\r\n\
<form method=\"post\" \">\r\n\
  Blue LED <input type=\"submit\" name=\"led\" value=\"ON\"><br>\r\n\
  Blue LED <input type=\"submit\" name=\"led\" value=\"OFF\"><br>\r\n\
  Blue LED <input type=\"submit\" name=\"blink\" value=\"BLINK\">\
  <input type=\"float\" name=\"rate\" value=\"1\" size=\"5\"><br>\n\
</form>\r\n\
</body></html>\r\n\
");

WiFiServer server(80);

void setup() {
   initHardware();
  connectWiFi();
  server.begin();
  setupMDNS();
  Serial.println();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req;
  int n, i=1; 

  // first line of request decides what happens
  req = client.readStringUntil('\n');
  if ( req.startsWith( "GET / HTTP/1.1") ){
    // load web page
    Serial.println( "Loading web page" );
    client.print(Page);
    delay(1);
    req = client.readString(); // drop the rest 
    return;
  } else if (req.startsWith( "GET /favicon.ico HTTP/1.1" ) ) {
    // At some point we may want to send an icon.  Punt for now
    Serial.println( "favicon.ico request" );
    delay(1);
    req = client.readString(); // drop the rest
    return;
  } else if (req.startsWith( "GET /currentsetting.htm HTTP/1.1" ) ) {
    // Seems to be part of the "keep alive". Punt for now
    Serial.println( "currentsetting.htm request" );
    delay(1);
    req = client.readString(); // drop the rest
    return;
  } else if ( 0 == req.length() ) { // skip empty requests
    delay(1);
    Serial.println( "empty request" );
    return;
  } else if (req.startsWith( "POST / HTTP/1.1" ) ) {
    // process the post
    // skip until the data
    while ( req.length() > 1 ) {
      req = client.readStringUntil('\n');
    }
    if ( 1 == req.length() ) {
      req = client.readStringUntil('\n');
      Serial.println( "POSTed data:");
      Serial.println( req );\
      // need to send back some response to avoid error
      client.print("HTTP/1.1 204 No content\r\n");
      if ( req.startsWith("led=ON") ){
        digitalWrite(LED_PIN, 0);        
      } else  if ( req.startsWith("led=OFF") ) {
        digitalWrite(LED_PIN, 1);        
      }
    }
    
    delay(1);
  }

}

void connectWiFi()
{
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

void setupMDNS()
{
  // Call MDNS.begin(<domain>) to set up mDNS to point to
  // "<domain>.local"
  if (!MDNS.begin("thing")) 
  {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

}

