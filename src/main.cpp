#include <Arduino.h>
#define CMDBUFFER_SIZE 32
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <SPI.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define PINK 0xEB34
#define LIGHT_PINK 0x92f53b
#define LIGHT_PURPLE 0x865b94
#define LIGHT_BLUE 0x9933ff
#define CYANN 0xE536F3

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);

const char message[] = "card detail";
int charge_length = 50, is_charging;

void inputs(char message[], int number, int strength = 0);
void wifi(int x, int y, int number);
void batteryDraw(int percentage, bool charge, int charging_percent);
void modee();
void charge();

char processCharInput(char *cmdBuffer, char c);
void setup()
{
  Serial.begin(9600);

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  Serial.println(F("OK!"));
  tft.reset();
  // uint16_t identifier = tft.readID();
  tft.begin(identifier);
  // tft.setRotation();
  tft.fillScreen(WHITE);
  tft.setCursor(30, 120);
  tft.setTextSize(10);
  tft.setTextColor(LIGHT_BLUE);
  tft.print("SGL");
  delay(3000);
  tft.fillScreen(BLACK);
  modee();
}

char cmdBuffer[CMDBUFFER_SIZE] = "";
char c;
int receivedData1;
int receivedData2[10];
byte receivedData3[10][12];
int rssi_int[10];

void loop()
{
  // Read the data description string from UART
  // Read the data description string from UART

  while (Serial.available() > 0)
  {
    c = processCharInput(cmdBuffer, Serial.read());
    Serial.print(c);
    if (c == '\n')
    {
      Serial.println();
      // Full command received. Do your stuff here!
      if (strcmp("CARDS_NUM", cmdBuffer) == 0)
      {
        while (!Serial.available())
          ;

        Serial.println("Cards number will be written");
        receivedData1 = Serial.read();
        Serial.println(receivedData1);
        if (receivedData1 != 34 && receivedData1 != 20 && receivedData1 != 48)
        {
          break;
        }
        c = 'A';
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
        break;
      }

      else if (strcmp("RSSI", cmdBuffer) == 0)
      {
        // int temp = receivedData1;
        while (!Serial.available())
          ;
        while (Serial.available() > 0)
        {

          int receivedData2[receivedData1];

          Serial.println(receivedData1);
          Serial.println("RSSI will be written");
          for (int i = 0; i < receivedData1; i++)
          {
            if (i >= 0 && i < receivedData1)
            {
              receivedData2[i] = Serial.read();
              Serial.println(receivedData2[i]);
              rssi_int[i] = map(receivedData2[i], 35, 45, 3, 0);
            }
          }
          c = 'A';
          memset(receivedData2, 0, sizeof(receivedData2));
          memset(cmdBuffer, 0, sizeof(cmdBuffer));
          break;
        }
      }

      else if (strcmp("CARD_DATA", cmdBuffer) == 0)
      {
        while (!Serial.available())
          ;
        while (Serial.available() > 0)
        {
          byte receivedData3[receivedData1][12];
          Serial.println("Cards data will be written");
          for (int i = 0; i < receivedData1; i++)
          {
            for (int j = 0; j < 12; j++)
            {
              if (i >= 0 && i < receivedData1)
              {
                receivedData3[i][j] = Serial.read();
                Serial.print(receivedData3[i][j], HEX);
                Serial.print(" ");
              }
              else
              {
                break;
              }
            }
            if (i >= 0 && i < receivedData1)
            {
              inputs((char *)receivedData3[i], i, (int)rssi_int[i]);
              memset(receivedData3[i], 0, sizeof(receivedData3));
            }
          }
          c = 'A';

          memset(rssi_int, 0, sizeof(rssi_int));
          memset(cmdBuffer, 0, sizeof(cmdBuffer));
          break;
        }
      }

      else if (strcmp("BATTERY", cmdBuffer) == 0)
      {
        while (!Serial.available())
          ;
        Serial.println("Battery data will be written");
        is_charging = Serial.read();
        batteryDraw(0, is_charging, 50);
        c = 'A';
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
      }

      c = 'A';
      memset(cmdBuffer, 0, sizeof(cmdBuffer));
    }
  }
}

char processCharInput(char *cmdBuffer, char c)
{
  // Store the character in the input buffer
  if (c >= 32 && c <= 126) // Ignore control characters and special ascii characters
  {
    if (strlen(cmdBuffer) < CMDBUFFER_SIZE)
    {
      strncat(cmdBuffer, &c, 1); // Add it to the buffer
    }
    else
    {
      return '\n';
    }
  }
  else if ((c == 8 || c == 127) && cmdBuffer[0] != 0) // Backspace
  {

    cmdBuffer[strlen(cmdBuffer) - 1] = 0;
  }

  return c;
}

void inputs(char message[], int number, int strength = 0) // number = number of box we want to print at screen , strength = signal strength of wifi
{
  tft.fillRect(5, 90 + number * 30, 220, 25, BLACK);
  tft.drawRect(5, 90 + number * 30, 220, 25, LIGHT_PINK); // to print UID string rect 1
  tft.setCursor(10, 95 + number * 30);
  tft.setTextColor(CYAN);
  tft.setTextSize(2.5);
  tft.print(message);

  Serial.println("Strrngth");
  Serial.println(strength);
  switch (strength)
  {
  case 1: // to create 1 bar of wifi signal
    wifi(0, 0, number);
    break;
  case 2: // to create 2 bars
    wifi(0, 0, number);
    wifi(1, 1, number);
    break;
  case 3: // to create 3 bars
    wifi(0, 0, number);
    wifi(1, 1, number);
    wifi(2, 2, number);
    break;
  case 4: // to create 4 bars
    wifi(0, 0, number);
    wifi(1, 1, number);
    wifi(2, 2, number);
    wifi(3, 3, number);
    break;
  default:
    wifi(0, 0, number);
    break;
  }
}

void wifi(int x, int y, int number)
{

  tft.drawRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, BLACK);
  tft.fillRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, BLACK);

  tft.drawRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, WHITE);
  tft.fillRect(180 + 10 * x, 104 - 4 * y + number * 30, 8, 8 + 4 * x, WHITE);
}

void batteryDraw(int percentage, bool charge, int charging_percent)
{
  if (charge_length < 0)
  {
    charge_length = 50;
  }
  if (charge)
  {
    tft.drawRect(190, 0, 50, 30, WHITE);
    tft.fillRect(190, 2, 50, 25, BLACK);
    tft.fillRect(190 + charge_length, 2, 50, 25, GREEN);
    tft.drawRect(180, 9, 10, 10, WHITE);
    tft.fillRect(180, 9, 10, 10, WHITE);
    charge_length -= 10;

    Serial.println(charge_length);
  }
  else
  {
    Serial.println("charging process is not going on ");
    int drawValue = map(percentage, 0, 100, 0, 50);
    int battery_x = 190 + (50 - (percentage * 50 / 100));

    if (percentage <= 25)
    {
      tft.drawRect(190, 0, 50, 30, WHITE);
      tft.fillRect(battery_x, 2, drawValue, 25, RED);
      tft.drawRect(180, 9, 10, 10, WHITE);
      tft.fillRect(180, 9, 10, 10, WHITE);
    }
    else if (percentage == 100)
    {
      tft.drawRect(190, 0, 50, 30, WHITE);
      tft.fillRect(battery_x, 2, drawValue, 25, PINK);
      tft.drawRect(180, 9, 10, 10, WHITE);
      tft.fillRect(180, 9, 10, 10, WHITE);
    }
    else
    {
      tft.drawRect(190, 0, 50, 30, WHITE);
      tft.fillRect(battery_x, 2, drawValue, 25, GREEN);
      tft.drawRect(180, 9, 10, 10, WHITE);
      tft.fillRect(180, 9, 10, 10, WHITE);
    }
  }
}

void modee()
{
  tft.setCursor(0, 10); // to write title "mode"
  tft.setTextColor(LIGHT_PINK);
  tft.setTextSize(2.8);
  tft.print("MODE");
  tft.drawRect(4, 34, 225, 40, MAGENTA);
}