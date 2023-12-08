#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>

const byte ROWS = 4;
const byte COLS = 4;

const char *ssid = "JEMBATAN ZAYA"; // Change this to your WiFi SSID
const char *password = "Heil1234";  // Change this to your WiFi password
String serverName = "http://kuritzu.com:8000";

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte colPins[COLS] = {13, 12, 14, 27};
byte rowPins[ROWS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int servo_pin = 15;
Servo servo;

char codes[4] = {'_', '_', '_', '_'};
int current_code_index = 0;
bool repaint_lcd = false;
char correct_code[4] = {'1', '2', '3', '4'};
char char_close = '*';

// function declaration
void lcd_standby();
bool lcd_check_code(char code[4]);
void lcd_correct_code();
void lcd_wrong_code();
void open_servo();
void close_servo();
void wifi_connect();
void send_data();
void set_server_mode();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // We start by connecting to a WiFi network

  Serial.println("Initiate Servo");
  servo.attach(servo_pin);
  Serial.println("Initiate LCD");
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  wifi_connect();
  lcd_standby();
  close_servo();
}

void loop()
{
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    Serial.print("Key pressed: ");
    Serial.println(key);

    codes[current_code_index] = key;
    current_code_index++;

    if (current_code_index == 4)
    {
      // if (lcd_check_code(codes))
      // {
      //   lcd_correct_code();
      // }
      // else
      // {
      //   lcd_wrong_code();
      // }
      if (codes[0] == 'A' && codes[1] == 'A' && codes[2] == 'A' && codes[3] == 'A')
      {
        set_server_mode();
      }

      send_data();
      current_code_index = 0;
    }
    repaint_lcd = true;
  }

  if (repaint_lcd)
  {
    lcd_standby();
    repaint_lcd = false;
  }
}

void open_servo()
{
  servo.write(179);
}

void close_servo()
{
  servo.write(0);
}

void set_server_mode()
{
  String ip = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NEW IP FORMAT:");
  lcd.setCursor(0, 1);
  lcd.print("IP*IP*IP*IP#PORT");
  lcd.setCursor(0, 2);
  lcd.print("PRESS A TO CONFIRM");
  lcd.setCursor(0, 3);

  char key;

  while (key != 'A')
  {
    key = keypad.getKey();
    if (key != NO_KEY)
    {
      Serial.print("Key pressed: ");
      Serial.println(key);
      if (key == '*')
      {
        ip += ".";
      }
      else if (key == '#')
      {
        ip += ":";
      }
      else if (key != 'A')
      {
        ip += key;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NEW IP FORMAT:");
      lcd.setCursor(0, 1);
      lcd.print("IP*IP*IP*IP#PORT");
      lcd.setCursor(0, 2);
      lcd.print("PRESS A TO CONFIRM");
      lcd.setCursor(0, 3);
      lcd.print(ip);
    }
  }

  serverName = "http://" + ip;
}

void send_data()
{
  HTTPClient http;
  // send post request
  http.begin(serverName + "/api/box/" + codes[0] + codes[1] + codes[2] + codes[3] + "/123456");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("");

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println(response);
    if (response == "true")
    {
      lcd_correct_code();
    }
    else
    {
      lcd_wrong_code();
    }
  }
  else
  {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
}

void wifi_connect()
{
  // = = = = = = = = = = = = = = = = = = = =
  // = = = = = C O N N E C T I N G = = = = =
  // = = = = = . . . . . . . . . . = = = = =
  // = = = = = = = = = = = = = = = = = = = =
  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("CONNECTING");
    lcd.setCursor(4, 2);
    for (int j = 0; j < i + 1; j++)
    {
      lcd.print(".");
    }
    i = (i + 1) % 10;
    delay(500);
  }

  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("CONNECTED");

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void lcd_standby()
{
  // = = = = = = = = = = = = = = = = = = = =
  // = = = = = R E L O W = B O X = = = = = =
  // = = = = = = = _ _ _ _ = = = = = = = = =
  // = = = = = = = = = = = = = = = = = = = =
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("RELOW BOX");
  lcd.setCursor(7, 2);
  lcd.print(codes[0]);
  lcd.print(codes[1]);
  lcd.print(codes[2]);
  lcd.print(codes[3]);
}

void lcd_correct_code()
{
  // = = = = = = = = = = = = = = = = = = = =
  // = = = = = S U C C E S S F U L = = = = =
  // = = = = = = = = = = = = = = = = = = = =
  // = = = = = = = = = = = = = = = = = = = =

  // = = = = = = = = = = = = = = = = = = = =
  // P R E S S = * = T O = C L O S E = N O W
  // = A U T O = C L O S E = I N = 3 0 S = =
  // = = = = = = = = = = = = = = = = = = = =

  open_servo();
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("SUCCESSFUL");
  delay(5000);

  int sec_left = 30;
  char key = 'z';
  while (sec_left > 0)
  {
    key = keypad.getKey();
    if (key == char_close)
    {
      break;
    }
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("HOLD ");
    lcd.print(char_close);
    lcd.print(" TO CLOSE NOW");
    lcd.setCursor(1, 2);
    lcd.print("AUTO CLOSE IN ");
    lcd.print(sec_left);
    lcd.print("S");

    key = keypad.getKey();
    if (key == char_close)
    {
      break;
    }
    delay(1000);
    sec_left--;
    key = keypad.getKey();
    if (key == char_close)
    {
      break;
    }
  }

  codes[0] = '_';
  codes[1] = '_';
  codes[2] = '_';
  codes[3] = '_';
  repaint_lcd = true;
  close_servo();
}

void lcd_wrong_code()
{
  // = = = = = = = = = = = = = = = = = = = =
  // = = = = = W R O N G = C O D E = = = = =
  // = = T R Y = A G A I N = I N = 5 S = = =
  // = = = = = = = = = = = = = = = = = = = =
  int sec_left = 5;
  while (sec_left > 0)
  {
    lcd.clear();
    lcd.setCursor(5, 1);
    lcd.print("WRONG CODE");
    lcd.setCursor(2, 2);
    lcd.print("TRY AGAIN IN ");
    lcd.print(sec_left);
    lcd.print("S");

    delay(1000);
    sec_left--;
  }

  codes[0] = '_';
  codes[1] = '_';
  codes[2] = '_';
  codes[3] = '_';
  repaint_lcd = true;
}

bool lcd_check_code(char code[4])
{
  return code[0] == correct_code[0] &&
         code[1] == correct_code[1] &&
         code[2] == correct_code[2] &&
         code[3] == correct_code[3];
}