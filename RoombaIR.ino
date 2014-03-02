// Roomba IR remote web server with a Nanode by Jan Wildeboer
// 
// Based on BackSoon exmaple of the Ethercard library
// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Uncomment to see Debug on Serial
//#define DEBUG

// Includes
#include <EtherCard.h> // https://github.com/jcw/ethercard
#include <IRremote.h>  // http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html

// Global variables

IRsend irsend; // Initialise the IRsend library. IR LED must be on Digital Pin 3

int seen = -1;              // Global indicator to avoid repeat commands
static char outCount = -1;
static BufferFiller bfill;  // used as cursor while filling the buffer

// Nanode Network definitions
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 }; // MAC Address
static byte myip[] =  { 192,168,1,28 };                  // IP Address
static byte gwip[] =  { 192,168,1,254 };                 // Default Gateway
static byte dnsip[] = { 192,168,1,254 };                 // DNS Server

byte Ethernet::buffer[700];                              // tcp/ip send and receive buffer

#ifdef DEBUG
static int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif

// Get the network ready
void setup(){
  Serial.begin(9600);
  Serial.println("\n[RoombaRemote]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  // Setup static network
  ether.staticSetup(myip, gwip, dnsip);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
}

// Put the homepage in the send buffer, including headers and stuff
static void homePage(BufferFiller&buf) {
    buf.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<html>"
    "<head>"
    "<title>Roomba</title>"
    "<meta name=\"viewport\" content=\"width=device-width;\">"
    "<link rel=\"stylesheet\" href=\"http://zorg.org/webremote.css\" type=\"text/css\">"
    "</head>"
    "<body>"
    "<h2>Roomba</h2>"
    "<a href=\"/1\">Clean</a><br>"
    "<a href=\"/2\">Service</a><br>"
    "<a href=\"/3\">Dock</a><br>"
    "</body>"
    "</html>"));
#ifdef DEBUG
    buf.emit_p(PSTR(" ($D bytes free)"), freeRam());
#endif
  Serial.println("Sending homepage");
}

// send redirect to homepage
static void redirect(BufferFiller&buf){
  buf.emit_p(PSTR(
    "HTTP/1.0 302 found\r\n"
    "Location: /\r\n"
    "\r\n"));
#ifdef DEBUG
  Serial.println("Sending 302");
#endif
}

// Send the CLEAN command via IR to the Roomba
int roombaClean(BufferFiller&buf) {
  Serial.println("Sending CLEAN");
  roombaSend(136);
  delay (2500);
  roombaSend(136);
  return 1;
}

// Send the CLEAN and STOP command via IR to the Roomba
int roombaService(BufferFiller&buf) {
  Serial.println("Sending SERVICE");
  roombaSend(136);
  delay (2500);
  roombaSend(137);
  delay (2500);
  roombaSend(137);
  return 1;
}

// Send the DOCK command via IR to the Roomba
int roombaDock(BufferFiller&buf) {
  Serial.println("Sending DOCK");
  roombaSend(143);
  delay (2500);
  roombaSend(143);
  return 1;
}

// Sned code to the Roomba
void roombaSend(int code){
#ifdef DEBUG
  Serial.print("Sending Roomba code ");
  Serial.print(code);
#endif
  int length = 8;
  unsigned int raw[length*2];
  unsigned int one_pulse = 3000;
  unsigned int one_break = 1000;
  unsigned int zero_pulse = one_break;
  unsigned int zero_break = one_pulse;

  int arrayposition = 0;
  for (int counter = length-1; counter >= 0; --counter) {
    if(code & (1<<counter)) {
      raw[arrayposition] = one_pulse;
      raw[arrayposition+1] = one_break;
    }
    else {
      raw[arrayposition] = zero_pulse;
      raw[arrayposition+1] = zero_break;
    }
    arrayposition = arrayposition + 2;
  }
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(raw, 15, 38);
    delay(50);
  }
#ifdef DEBUG
  Serial.println(" - Done");
#endif
}

// Main
//
// Checks for incoming GET requests and acts accordingly.
// It aint no Apache or nginx but it does the job ;-)

void loop(){
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  // check if valid tcp data is received
  if (pos) {
    bfill = ether.tcpOffset();
    char* data = (char *) Ethernet::buffer + pos;
#ifdef DEBUG
    Serial.println(data);
#endif    
    // receive buf hasn't been clobbered by reply yet
    if (strncmp("GET / ", data, 6) == 0){
      homePage(bfill);
      seen = -1;
    } else if (strncmp("GET /1", data, 6) == 0) {
      if ( seen == 1 ) {
#ifdef DEBUG
        Serial.println("Already seen CLEAN!");
#endif
        redirect(bfill);
      } else {
        seen = roombaClean(bfill);
        redirect(bfill);
      }  
    } else if (strncmp("GET /2", data, 6) == 0) {
      if ( seen == 1 ) {
#ifdef DEBUG
        Serial.println("Already seen SERVICE!");
#endif
        redirect(bfill);
      } else {
        seen = roombaService(bfill);
        redirect(bfill);
      }  
    } else if (strncmp("GET /3", data, 6) == 0) {
      if ( seen == 1 ) {
#ifdef DEBUG
        Serial.println("Already seen DOCK!");
#endif
        redirect(bfill);
      } else {
        seen = roombaDock(bfill);
        redirect(bfill);
      }  
    } else {
      bfill.emit_p(PSTR(
        "HTTP/1.0 401 Unauthorized\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>401 Unauthorized</h1>"));
      Serial.println("Sending 401");
    }  
    ether.httpServerReply(bfill.position()); // send web page data
    ether.httpServerReplyAck();
  }
}
