#include <HttpRequest.h>

/*

  Example on common use of HttpRequest library to parse HTTP Request from browser

  created 5 Dec 2012
  by Francisco G. Paletta


  Build over the exemple:

  Web Server

  A simple web server that shows the value of the analog input pins using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13
   Analog inputs attached to pins A0 through A5 (optional)

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
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress serverIP(192, 168, 1, 177);
IPAddress gwIP(192, 168, 0, 254);
IPAddress dnsIP(192, 168, 0, 6);
IPAddress mask(255, 255, 255, 0);

EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Set all digitla pins to output
  for (int i = 0; i < 13; i++)
  {
    pinMode(i, OUTPUT);
  }

  // disable w5100 SPI while setting up SD
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  if (SD.begin(4) == 0)
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
          if (httpReq.uri == "/favicon.ico") {
            // Ignore this request
            continue;
          }

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");

          if (String("GET").equals(&httpReq.method[0])) {
            for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
              int sensorReading = analogRead(analogChannel);
              client.print("Analog input ");
              client.print(analogChannel);
              client.print(" is ");
              client.print(sensorReading);
              client.println("<br />");
            }
            client.println("<br /><br />");
            for (int digitalChannel = 2; digitalChannel <= 13; digitalChannel++) {
              int pinReading = digitalRead(digitalChannel);
              client.print("Digital Output ");
              client.print(digitalChannel);
              client.print(" is ");
              client.print(pinReading);
              client.println("<br />");
            }
            client.println("<br /><br />");
          } else if (String("POST").equals(&httpReq.method[0]))  {
            int pos = httpReq.getParam("pin", value);
            client.print("Found Param pin");
            client.println("<br />");
            if (pos > 0) {
              if (String("2").equals(&value[0])) {
                digitalWrite(2, !digitalRead(2));
              } else if (String("3").equals(&value[0])) {
                digitalWrite(3, !digitalRead(3));
              } else if (String("4").equals(&value[0])) {
                Serial.write("toggled pin 4 is not supported.  The SD Card use this pin \n");
                client.print("Skipped ");
                // digitalWrite(4, !digitalRead(4));
              } else if (String("5").equals(&value[0])) {
                digitalWrite(5, !digitalRead(5));
              } else if (String("6").equals(&value[0])) {
                digitalWrite(6, !digitalRead(6));
              } else if (String("7").equals(&value[0])) {
                digitalWrite(7, !digitalRead(7));
              } else if (String("8").equals(&value[0])) {
                digitalWrite(8, !digitalRead(8));
              } else if (String("9").equals(&value[0])) {
                digitalWrite(9, !digitalRead(9));
              } else if (String("10").equals(&value[0])) {
                digitalWrite(10, !digitalRead(10));
              } else if (String("11").equals(&value[0])) {
                digitalWrite(11, !digitalRead(11));
              } else if (String("12").equals(&value[0])) {
                digitalWrite(12, !digitalRead(12));
              } else if (String("13").equals(&value[0])) {
                digitalWrite(13, !digitalRead(13));
              }

              client.print("Toggled pin ");
              client.print(value);
              client.print("<br \>");
            }
          }

          //access object properties
          client.print("<br /><br />Method: ");
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
          for (int i = 1; i <= httpReq.paramCount; i++) {
            httpReq.getParam(i, name, value);
            client.print(name);
            client.print("-");
            client.print(value);
            client.println("<br>");
          }
          //list received cookies
          client.println("Cookies:<br>");
          for (int i = 1; i <= httpReq.cookieCount; i++) {
            httpReq.getCookie(i, name, value);
            client.print(name);
            client.print(" - ");
            client.print(value);
            client.println("<br>");
          }
          //find a particular parameter name
          int pos = httpReq.getParam("MyParameter", value);
          if (pos > 0) {
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


