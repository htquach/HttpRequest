/*

  Example on common use of HttpRequest library to parse HTTP Request from browser
  
  created 5 Dec 2012
  by Francisco G. Paletta
  
  
  Build over the exemple:

  Web Server
 
	A simple web server that shows the value of the analog input pins using an Arduino Wiznet Ethernet shield. 
 
	Circuit:
	* Ethernet shield attached to pins 10, 11, 12, 13
	* Analog inputs attached to pins A0 through A5 (optional)
 
	created 18 Dec 2009
	by David A. Mellis
	modified 9 Apr 2012
	by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

//include HttpRequest object library
#include <HttpRequest.h>

//Create an object to handle the HTTP request
HttpRequest httpReq;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress serverIP(192,168,0,18);
IPAddress gwIP(192,168,0,254);
IPAddress dnsIP(192,168,0,6);
IPAddress mask(255,255,255,0);

EthernetServer server(8888);

void setup() {
  Serial.begin(9600);
   while (!Serial) {
    ;
  }
  
  // disable w5100 SPI while setting up SD
   pinMode(10,OUTPUT);
   digitalWrite(10,HIGH);
   
   if(SD.begin(4) == 0) 
      Serial.println("SD falhou");
   else
      Serial.println("SD ok");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, serverIP, dnsIP, gwIP, mask);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  
  //declare name and value to use the request parameters and cookies
  char name[HTTP_REQ_PARAM_NAME_LENGTH], value[HTTP_REQ_PARAM_VALUE_LENGTH];
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        //parse the received caracter
        httpReq.parseRequest(c);
        
        Serial.write(c);
        
        //IF request has ended -> handle response
        if (httpReq.endOfRequest()) {

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
		  client.println("<body>");

          //access object properties
          client.print("Method: ");
          client.print(httpReq.method);
          client.println("<br>");
          client.print("Uri: ");
          client.print(httpReq.uri);
          client.println("<br>");
          client.print("Version: ");
          client.print(httpReq.version);
          client.println("<br>");
          client.print("paramCount: ");
          client.print(httpReq.paramCount);
          client.println("<br>");
          //list received parameters GET and POST
		  client.println("Parameters:<br>");
          for(int i=1;i<=httpReq.paramCount;i++){
            httpReq.getParam(i,name,value);
            client.print(name);
            client.print("-");
            client.print(value);
            client.println("<br>");
          }
          //list received cookies
          client.println("Cookies:<br>");
          for(int i=1;i<=httpReq.cookieCount;i++){
            httpReq.getCookie(i,name,value);
            client.print(name);
            client.print(" - ");
            client.print(value);
            client.println("<br>");
          }
          //find a particular parameter name
          int pos=httpReq.getParam("MyParameter",value);
          if(pos>0){
            client.print("<br>");
            client.print("Found 'MyParameter'. Value: ");
            client.print(value);
            client.println("<br>");  
          }
		  client.println("</body>");
          client.print("</html>");
          
          //Reset object and free dynamic allocated memory
          httpReq.resetRequest();
          
          break;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


