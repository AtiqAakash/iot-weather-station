/*  Arduino weather station, taking it to the internet.
 *  Developed By Mohammad Atiqur Rahman Aakash
 *  Email: aakash.unipune@gmail.com
 */
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <EEPROM.h>

/*===============================================*/

// WiFi Credentials
const char* ssid = "home";       // WiFi network SSID
const char* password = "password";  // WiFi network password
const char *temp_event = "temperature"; // Event name for temperature notification

/*===============================================*/

// Definition of sensor pins
#define DHTPIN 2  // GPIO -> D4 for DHT sensor
#define DHTTYPE DHT11  // DHT sensor type

/*===============================================*/

// NodeMCU pin mapping
int led = 5;         // D1 pin for LED
int vib_data = 14;   // D5 pin for Vibration sensor data input
int echoPin = 4;     // D2 pin for Echo (ultrasonic sensor)
int trigPin = 0;     // D3 pin for Trigger (ultrasonic sensor)

float humid;         // Humidity variable
float temp;          // Temperature variable
float fahr;          // Temperature in Fahrenheit
float hif;           // Heat index in Fahrenheit
float hic;           // Heat index in Celsius
float dpt;           // Dew point temperature

WiFiServer server(80); // Web server on port 80
DHT dht(DHTPIN, DHTTYPE, 30); // Initialize DHT sensor

/*===============================================*/

// Setup function
void setup()
{
   Serial.begin(115200); // Start serial communication
   delay(1000);
   Serial.println("_____");

   pinMode(led, OUTPUT); // Set LED pin as output

   digitalWrite(led, LOW); // Initially turn off the LED

   dht.begin(); // Initialize DHT sensor

   WiFi.mode(WIFI_STA); // Station mode for connecting to WiFi

   WiFi.begin(ssid, password); // Connect to WiFi network

   while (WiFi.status() != WL_CONNECTED)
   {
      Serial.print("Connecting to ");
      Serial.print(ssid);
      Serial.println(". Please wait...");
      delay(100);
   }

   Serial.println("WiFi connected."); // WiFi connected successfully

   server.begin(); // Start the server
   Serial.println("Server started."); // Server started successfully

   Serial.print("IP address: ");
   Serial.println(WiFi.localIP()); // Print local IP address
   Serial.print("MAC address: ");
   Serial.println(WiFi.macAddress()); // Print MAC address
}

/*===============================================*/

// Main loop function
void loop()
{
  delay(100);

  // Read temperature and humidity
  float humid = dht.readHumidity();
  float temp = dht.readTemperature();
  delay(10);

  // Calculate temperature in Fahrenheit
  float fahr = dht.readTemperature(true);

  // Calculate heat index
  float hif = dht.computeHeatIndex(fahr, humid); // For Fahrenheit
  float hic = dht.computeHeatIndex(temp, humid, false); // For Celsius

  // Calculate dew point temperature
  float dpt = dewPointFast(temp, humid);

  // Check if DHT sensor reading is valid
  if (isnan(humid) || isnan(temp) || isnan(fahr))
  {
    Serial.println("Error: DHT sensor not connected or reading invalid.");
    return;
  }

  // Print temperature and humidity readings to Serial monitor
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C, Humidity: ");
  Serial.print(humid);
  Serial.println(" %");

  // Check for client connection
  WiFiClient client = server.available();
  if (!client)
    return;

  // Wait until the client sends some data
  while(!client.available())
    delay(1);

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);

  // Clear the client request buffer
  client.flush();

  // Process user input
  if (req.indexOf("/led-on") != -1)
    digitalWrite(led, HIGH); // Turn on LED
  else if (req.indexOf("/led-off") != -1)
    digitalWrite(led, LOW); // Turn off LED

  // Clear the client response buffer
  client.flush();

  // Build HTML page
  String html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  html += "<!DOCTYPE html>";
  html += "<html><head><meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>Weather Station</title>";
  html += "<style>body {font-family: Arial, Helvetica, sans-serif;}</style>";
  html += "</head><body>";
  html += "<h1>Weather Information</h1>";
  html += "<p>Humidity: ";
  html += humid;
  html += " %<br>";
  html += "Temperature: ";
  html += temp;
  html += " °C<br>";
  html += "Temperature (Fahrenheit): ";
  html += fahr;
  html += " °F<br>";
  html += "Heat Index (Celsius): ";
  html += hic;
  html += " °C<br>";
  html += "Heat Index (Fahrenheit): ";
  html += hif;
  html += " °F<br>";
  html += "Dew Point: ";
  html += dpt;
  html += " °C</p>";
  html += "</body></html>";

  // Send HTML response to client
  client.print(html);

  // Delay to ensure response is sent
  delay(1);

  Serial.println("Request processed.");
  client.stop(); // Close the connection
}

// Function to send temperature event notification
void tem(const char *e_temp)
{
  WiFiClient client;
  const char *host = "maker.ifttt.com"; // IFTTT host address
  int httpPort = 80; // HTTP port

  if (!client.connect(host, httpPort))
  {
    Serial.println("Error: Connection to server failed.");
    return;
  }

  // Construct the URL for IFTTT webhook
  String url = "/trigger/";
  url += temp_event;
  url += "/with/key/";
  url += temp_key;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Send GET request to IFTTT server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Read and print server response
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

// Function to calculate dew point temperature
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double t = (a * celsius) / (b + celsius) + log(humidity*0.01);
  double Td = (b * t) / (a - t);
  return Td;
}
