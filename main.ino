/*  Arduino weather station, taking it to the internet.
 *  Developed By Mohammad Atiqur Rahman Aakash
 *  Email: aakash.unipune@gmail.com
*/
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <EEPROM.h>

/*===============================================*/

// WiFi Credentials
const char* ssid = "home";
const char* password = "password";
const char *temp_event = "temperature";

/*===============================================*/

// Definition pins
#define DHTPIN 2 // GPIO -> D4. Can be changed to any other pins.
#define DHTTYPE DHT11

/*===============================================*/

// NodeMCU pin mapping
/*static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1; */

/*===============================================*/

int led = 5; // D1
int vib_data = 14; // D5 Vibration data input line.
int echoPin = 4; // D2
int trigPin = 0; // D3
float humid;
float temp;
float fahr;
float hif;
float hic;
float dpt;

WiFiServer server(80); // 80 is the port followed by ip address.
DHT dht(DHTPIN, DHTTYPE, 30);

/*===============================================*/


// Setup starts here.
void setup()
{
   Serial.begin(115200);
   delay(1000);
   Serial.println("_____");

   // Defining Pin Modes
   pinMode(led, OUTPUT);


   // Initialize all pins at inactive state.
   digitalWrite(led, 0);

   // Initialize DHT sensor
   dht.begin();

   // Wifi mode selection.
   WiFi.mode(WIFI_STA); // Station mode selected.

   // Debugging through serial monitor.
   // Connecting to WiFi Network.
   WiFi.begin(ssid,password);

   while (WiFi.status() != WL_CONNECTED)
   {
      Serial.print("Connecting to  ");
      Serial.print(ssid);
      Serial.println("  Please Wait");
      delay(10);
    }

   Serial.println("Okay, I'm ON!");
   Serial.println("WiFi Connected");


   // Start the server.
   server.begin();
   Serial.println("Bingo.. Server Started");

   //Show Ip Address on serial monitor.
   Serial.println(WiFi.localIP());
   Serial.print("MAC: ");
   Serial.println(WiFi.macAddress());
}


/*===============================================*/
// Main function Starts.
void loop()
{
  delay(100);
/*===============================================*/

  // Temperature and humidity section.

  float humid = dht.readHumidity();
  float temp = dht.readTemperature();
  delay(10);
  if (temp>30){tem(temp_event);}
  float fahr = dht.readTemperature(true);
  float hif = dht.computeHeatIndex(fahr, humid); // For fahrenheight
  float hic = dht.computeHeatIndex(temp, humid, false); // For Celcius
  float dpt = dewPointFast(temp, humid);

  // DHT availability.
  if (isnan(humid) || isnan(temp) || isnan(fahr))
  {
    Serial.println(" DHT not connected");
    return;
  }

  Serial.print("Temperature :");
  Serial.print(temp);
  Serial.print("°C");
  Serial.print("|| Humidity ");
  Serial.print(humid);
  Serial.println("%");


  // Checking if connected or not.
  WiFiClient client = server.available();
  if (!client)
    return;

  while(!client.available())
    delay(1);
    String req = client.readStringUntil('\r');
    Serial.println(req);

    client.flush();

   // User input condition starts here.
   if (req.indexOf("/led-on") != -1)
        digitalWrite(led, 1);
   else if (req.indexOf("/led-off") != -1)
        digitalWrite(led, 0);


   client.flush();

    /*===============================================*/
    // Page Development.
  String html =  "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
      html +=  "<!DOCTYPE html>";
      html +=  "<head><meta charset=\"UTF-8\">";
      //html +=  "<meta http-equiv=\"refresh\" content=\"60\">";
      html +=  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
      html +=  "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
      html +=  "<link rel=\"icon\" type=\"image/x-icon\" href=\"https://www.ald.softbankrobotics.com/sites/aldebaran/themes/aldebaran/favicon.ico\">";
      html +=  "<title>Aakash-IOT</title>";
      html +=  "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">";
      html +=  "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>";
      html +=  "</head><body>";
      html +=  "<div class='container'>";
      html +=  "<div class='progress'>";
      html +=  "<div class='progress-bar progress-bar-striped active' role='progressbar' aria-valuenow='' aria-valuemin='0' aria-valuemax='100' style='width:100%'>";
      html +=  "<MARQUEE BEHAVIOR=ALTERNATE>Developed by Atiq</MARQUEE><br><br>";
      html +=  "</div></div></div>";
      html +=  "<div class=\"container\">";
      html +=  "<div class=\"panel panel-default\" margin:25px>";
      html +=  "<div class=\"panel-heading\">";
      html +=  "<H2 style=\"font-family:robot; color:led2\"><center>LOAD CONTROL</center></H2>";
      html +=  "  <div class=\"row\">";
      html +=  "</div></div></div></div>";
      html +=  "<div class=\"container\">";
      html +=  "<div class=\"panel panel-default\" margin:5px>";
      html +=  "<div class=\"panel-heading\">";
      html +=  "<H2 style=\"font-family:robot; color:#ed4917\"><center>WEATHER INFO</center></H2>";
      html +=  "<div class=\"panel-body\" style=\"background-color: #518DC1\">";
      html +=  "<pre>";
      html +=  "Humidity       : ";
      html +=  humid;
      html +=  " %\n";
      html +=  "Temperature    : ";
      html +=  temp;
      html +=  " °C\n";
      html +=  "Temperature    : ";
      html +=  fahr;
      html += " °F\n";
      html +=  "Heat Index     : ";
      html +=  hic;
      html +=  " °C || ";
      html +=  hif;
      html +=  " °F\n";
      html +=  "Dew PointFast  : ";
      html +=  dpt;
      html +=  " °C\n<br>";
      html +=  "</pre></div></div></div></div>";
      html +=  "</body></html>";

  client.print(html);       /* Print out the page */
  delay(1);
  Serial.println("Running");
  client.stop();
}


void tem(const char *e_temp)
{
  WiFiClient client;
  int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    Serial.println("Connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/trigger/";
  url += temp_event;
  url += "/with/key/";
  url += temp_key;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Read all the lines of the reply from server and print them to Serial,
  // the connection will close when the server has sent all the data.
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    else
    {
      // No data yet, wait a bit
      delay(50);
    }
  }
}



 // reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity){
  double a = 17.271;
  double b = 237.7;
  double t = (a * celsius) / (b + celsius) + log(humidity*0.01);
  double Td = (b * t) / (a - t);
  return Td;
}

  // Read all the lines of the reply from server and print them to Serial,
  // the connection will close when the server has sent all the data.
while(client.connected())
{
  if(client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  else
  {
    // No data yet, wait a bit
    delay(50);
  }
}
