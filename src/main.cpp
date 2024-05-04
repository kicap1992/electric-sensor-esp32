#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

HardwareSerial fromUno(2);

char c;
String dataIn;

const char *ssid = "KARAN";
const char *password = "12345679";

void displayData(String data)
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(data);
  display.display();
}

String ifElse(String data, String name, String stat)
{
  if (data != " NAN")
  {
    Serial.println(name + " : " + data + " " + stat);
    return name + " : " + data + " " + stat + "\n";
  }
  else
  {
    Serial.println("Error get " + name);
    return "Error get " + name+ '\n';
  }
}

void seperator(String data)
{
  char charArray[data.length() + 1];              // Create a char array to store the string
  data.toCharArray(charArray, sizeof(charArray)); // Convert String to char array

  char *token = strtok(charArray, ","); // Tokenize the string based on comma delimiter
  int count = 0;

  String dataForDisplay = "";

  while (token != NULL)
  {
    switch (count)
    {
    case 1:
      dataForDisplay += String(token) +"\n";
      break;
    case 2:
      dataForDisplay += ifElse(String(token), "Voltage", "V");
      break;
    case 3:
      dataForDisplay += ifElse(String(token), "Current", "A");
      break;
    case 4:
      dataForDisplay += ifElse(String(token), "Power", "W");
      break;
    case 5:
      dataForDisplay += ifElse(String(token), "Energy", "kWh");
      break;
    case 6:
      dataForDisplay += ifElse(String(token), "Power Factor", "pf");
      break;
    default:
      dataForDisplay += ifElse(String(token), "Wifi", "");
      break;
    }
    token = strtok(NULL, ","); // Get the next token
    count++;
  }

  displayData(dataForDisplay);
}

void setup()
{
  Serial.begin(19200);
  fromUno.begin(9600); // ini dari esp32
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...\n");
    fromUno.print("Connecting to WiFi...\n");
    displayData("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi\n");
  fromUno.print("Connected to WiFi\n");
  displayData("Connected to WiFi");
}

void loop()
{
  String wifiStatus = "";
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Connected\n");
    fromUno.print("WiFi Connected\n");
    wifiStatus += "Connected";
    // delay(2000);
  }
  else
  {
    // toUnoWifiState.print("WiFi Disconnected\n");
    Serial.println("WiFi Disconnected\n");
    fromUno.print("WiFi Disconnected\n");
    wifiStatus += "Disconnected";
    // delay(2000);
  }

  while (fromUno.available())
  {
    c = fromUno.read();
    if (c != '\n')
    {
      dataIn += c;
    }
    else
    {
      break;
    }
  }

  if (c == '\n')
  {
    // Serial.println(dataIn);
    seperator(wifiStatus + "," +dataIn);
    // displayData(wifiStatus);
    c = 0;
    dataIn = "";
  }

  delay(2000);
}