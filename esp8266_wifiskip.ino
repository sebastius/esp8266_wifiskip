/*
   Wifiskip
   revspace.nl/Wifiskip
   github.com/sebastius/esp8266_wifiskip

   by Sebastius

   A squeezer-skipbutton for the ESP8266

   On the push of a button constructs an HTTP-GET request and sends it to our
   squeezer-server. Based on a generic HTTP Get example, generic WiFi connecting code etc.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Pin definitions
const int skipbutton = 0;
const int volhighbutton = 12;
const int vollowbutton = 13;
const int stopbutton = 5;
const int trollbutton = 4;

// WiFi information
const char ssid[] = "revspace-pub-2.4ghz";
const char pass[] = "";
WiFiClient client;

// Squeeze information
const char squeeze_site[] = "jukebox.space.revspace.nl";
const int squeeze_port = 80;
// const char player[] = "d8%3Ad3%3A85%3A17%3A30%3A9c"; // bar sparkshack
const char player[] = "be%3Ae0%3Ae6%3A04%3A46%3A38"; // klusbunker

// strings for jukebox
// Maybe encoding is needed: / -> %2F , ? -> %3F , = -> %3D, & -> %26
// see http://www.w3schools.com/tags/ref%5Furlencode.asp

const char playlist[] = "playlist";
const char mixer[] = "mixer";
const char jumpcommand[] = "jump";
const char stopcommand[] = "stop";
const char plusone[] = "%2B1";
const char volume[] = "volume";
const char plusfive[] = "%2B5";
const char minusfive[] = "-5";
const char insertcommand[] = "insert";
const char troll[] = "file%3A%2F%2F%2Fmusic%2FLosse%20Tracks%2Ftroll.mp3";

void setup() {
  pinMode(skipbutton, INPUT);
  pinMode(volhighbutton, INPUT);
  pinMode(vollowbutton, INPUT);
  pinMode(stopbutton, INPUT);
  pinMode(trollbutton, INPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Revspace WiFi SKIP for Squeezer");
  Serial.println();

  // Connect to WiFi
  connectWiFi();

  // Print some diagnostic info
  Serial.println();
  Serial.print("Configured for player: ");
  Serial.println(player);
}

void loop() {
  yield();
  
  if (digitalRead(skipbutton) == LOW) { // Normally Open button, so when tied to ground (LOW) button is pressed)
    Serial.println("Skip");
    getPage(playlist, jumpcommand , plusone);
    delay(250); // delay to prevent way to many skips
    // An ugly check to see if the skipbutton is still pressed after 3 seconds.
    // Then load a new randomized playlist to the player.
    // Did the staircase "if" to be more responsive after a button press, otherwise the skip button would be disabled for 3 seconds!
    if (digitalRead(skipbutton) == LOW) {
      delay(1000);
      if (digitalRead(skipbutton) == LOW) {
        delay(1000);
        Serial.println("Randomize");
        newrandomplaylist();
        delay(2000);
      }
    }
  }

  if (digitalRead(volhighbutton) == HIGH) { // HIGH because i use Normaly Close instead of NOpen buttons)
    Serial.println("Volume HIGH");
    getPage(mixer, volume , plusfive);
    delay(100); // delay to slow down volume-change
  }

  if (digitalRead(vollowbutton) == HIGH) { // HIGH because i use Normaly Close instead of NOpen buttons)
    Serial.println("Volume Low");
    getPage(mixer, volume, minusfive);
    delay(100); // delay to slow down volume-change
  }

  if (digitalRead(stopbutton) == HIGH) { // HIGH because i use Normaly Close instead of NOpen buttons)
    //stop
    Serial.println("Stop");
    getPage(stopcommand, plusone, plusone);
    delay(250); // delay to 'debounce' stop
  }

  if (digitalRead(trollbutton) == HIGH) { // HIGH because i use Normaly Close instead of NOpen buttons)
    Serial.println("Trollolol");
    getPage(playlist, insertcommand, troll);
    getPage(playlist, jumpcommand , plusone);
    delay(450); // delay to prevent way to many trolls
  }
}

void connectWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Perform an HTTP GET request to a remote page
bool getPage(const char *p0, const char *p1, const char *p2) {
  if ( !client.connect(squeeze_site, squeeze_port) ) {
    return false;
  }
  // Make an HTTP GET request
   // Maybe encoding is needed: / -> %2F , ? -> %3F , = -> %3D, & -> %26
   // see http://www.w3schools.com/tags/ref%5Furlencode.asp
 
  yield();
  client.print("GET /Classic/status_header.html?p0="); 
  client.print(p0);
  client.print("&p1="); 
  client.print(p1);
  client.print("&p2=");
  client.print(p2);
  client.print("&player=");
  client.print(player);
  client.println (" HTTP/1.1");
  yield();
  client.print("Host: ");
  client.println(squeeze_site);
  client.println("Connection: close");
  client.println();
  yield();
  Serial.println("Message sent.");

  return true;
}

bool newrandomplaylist() {
  if ( !client.connect(squeeze_site, squeeze_port) ) {
    return false;
  }
  // Make an HTTP GET request
  yield();
  client.print("GET /Classic/plugins/RandomPlay/mix.html?type=track&player=");
  client.print(player);
  client.print("&addOnly=0");
  client.println (" HTTP/1.1");
  yield();
  client.print("Host: ");
  client.println(squeeze_site);
  client.println("Connection: close");
  client.println();
  yield();

  Serial.println("Message sent.");
  return true;
}
