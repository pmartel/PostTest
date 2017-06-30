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
  Blue Led <input type=\"submit\" value=\"on\" name=\"Led\"<br>\r\n\
  Blue Led <input type=\"submit\"  value=\"off\" name=\"Led\"<br>\r\n\
</form><br />\r\n\
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

  String req, currLine, rest;
  int n, i=1, idx;  
  // read (and print) entire request
  // we seem to have trouble handling the 0-length string in
  // a POST request reading a line at a time.

  req = client.readString(); 
  // punt an empty request
  if ( req.length() == 0 ) return;
  // punt an icon request ffor now
  if (req.indexOf( "GET /favicon.ico") >=0) return;

  Serial.print("Thing got request:<<");
  Serial.print(req);
  Serial.println(">>Processing request:");
  while( 1 ) {
    currLine = getLine( &req, "\r\n");
    if ( req == "\0") break;
    Serial.print( i++ );
    Serial.print( " | " );
    Serial.print( currLine.length());
    Serial.print( " <<" );
    Serial.print( currLine);
    Serial.println( ">>" );
  }
  
/*
  idx = req.indexOf("\r\n");
  currLine = req.substring(0,idx);
  
  Serial.print("currLine: ");
  Serial.println(currLine);
  Serial.println(req);
*/

  Serial.println("");
  // Prepare the response. Start with the common header:
  //String s = "HTTP/1.1 200 OK\r\n";
  String s = "";
  s += Page;
#ifdef DEBUG  
  Serial.println("\r\nSending <<<");
  Serial.println(s);
  Serial.println(">>> to web client\r\n");
#endif
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}

// Takes a String, s, containing multiple lines separated by sep
// returns the first line and removes the line and sep from s
String getLine( String *s, const String sep ){
  String retLine;
  int i;
  
  i = s->indexOf( sep );
  if ( i == -1 ){
    // last line may not have sep
    if ( s->length() > 0 ) {
      retLine = *s;
      *s = String( '\0');
    }
    else {
      retLine = String('\0');
      *s = retLine;
    }
    Serial.print("retLine=<<");
    Serial.print(retLine);
    Serial.println(">>");
    Serial.println("Nothing left");
    return retLine;
  }
  
  retLine = s->substring(0,i);
  i += sep.length();
  Serial.print( "s =<<" );
  Serial.print( *s );
  Serial.println(">>");
  Serial.print( "i=");
  Serial.println( i);
  
  *s = s->substring(i);

  Serial.print("retLine=<<");
  Serial.print(retLine);
  Serial.println(">>");
  Serial.print("s left=<<");
  Serial.print( *s );
  Serial.println(">>");
  return retLine;
}

// WiFi functions
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

