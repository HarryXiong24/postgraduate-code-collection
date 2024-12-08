#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

int ledPin = 32; // 可以更改为任何你选择的GPIO引脚

// This example downloads the URL "http://arduino.cc/"
char ssid[50] = ""; // your network SSID (name)
char pass[50] = ""; // your network password (use for WPA, or use
// as key for WEP)
// Name of the server we want to connect to
const char kHostname[] = "worldtimeapi.org";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/api/timezone/Europe/London.txt";
// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void nvs_access()
{
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err)
    {
    case ESP_OK:
      Serial.printf("Done\n");
      Serial.printf("SSID = %s\n", ssid);
      Serial.printf("PASSWD = %s\n", pass);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      Serial.printf("The value is not initialized yet!\n");
      break;
    default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}
void setup()
{
  // 初始化串口用于调试
  Serial.begin(9600);
  Serial.begin(9600);
  delay(4000);
  // Retrieve SSID/PASSWD from flash before anything else
  nvs_access();
  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  // 设置GPIO引脚为输出模式
  pinMode(ledPin, OUTPUT);

  // 打印调试信息
  Serial.println("LED start...");
}

void loop()
{
  // 打开LED（高电平）
  // digitalWrite(ledPin, HIGH);
  // Serial.println("LED open");

  // 延时1秒
  delay(1000);

  // 关闭LED（低电平）
  // digitalWrite(ledPin, LOW);
  // Serial.println("LED close");

  int err = 0;
  WiFiClient c;
  HttpClient http(c);
  err = http.get("18.212.189.40", 8080, "/control-light", NULL);
  Serial.print("err: ");
  Serial.println(err);
  if (err == 0)
  {
    Serial.println("startedRequest ok");
    int statusCode = http.responseStatusCode();
    if (statusCode >= 200 && statusCode < 300)
    {
      Serial.print("Got status code: ");
      Serial.println(statusCode);

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println("Body returned follows:");

        String response = "";
        unsigned long timeoutStart = millis();

        // 读取整个响应体
        while ((http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout))
        {
          if (http.available())
          {
            response += (char)http.read();
            timeoutStart = millis();
          }
          else
          {
            delay(kNetworkDelay);
          }
        }

        // 解析响应中的 isOpen 字段
        Serial.print("response: ");
        Serial.println(response);

        int isOpenIndex = response.indexOf("\"isOpen\":");

        Serial.print("isOpenIndex: ");
        Serial.println(response.substring(isOpenIndex + 9, isOpenIndex + 10));

        if (isOpenIndex != -1)
        {
          int isOpenValue = response.substring(isOpenIndex + 9, isOpenIndex + 10).toInt();

          Serial.print("isOpenValue: ");
          Serial.println(isOpenValue);

          if (isOpenValue == 1)
          {
            digitalWrite(ledPin, HIGH);
            Serial.println("LED open");
          }
          else if (isOpenValue == 0)
          {
            digitalWrite(ledPin, LOW);
            Serial.println("LED close");
          }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {
      Serial.print("Getting response failed with status code: ");
      Serial.println(statusCode);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }

  http.stop();

  // 延时1秒
  delay(1000);
}
