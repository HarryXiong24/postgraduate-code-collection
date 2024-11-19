// Part1

// #include <Arduino.h>
// #include <inttypes.h>
// #include <stdio.h>
// #include "esp_system.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "nvs.h"
// #include "nvs_flash.h"

// void setup()
// {
//     Serial.begin(9600);
//     delay(4000);
//     Serial.printf("Starting setup...\n");

//     // Initialize NVS
//     esp_err_t err = nvs_flash_init();
//     if (err != ESP_OK)
//     {
//         Serial.printf("NVS init failed: %s\n", esp_err_to_name(err));
//     }

//     if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
//         err == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     {
//         Serial.printf("NVS needs erase, erasing...\n");
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         err = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(err);

//     Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
//     nvs_handle_t my_handle;
//     err = nvs_open("storage", NVS_READWRITE, &my_handle);
//     if (err != ESP_OK)
//     {
//         Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     }
//     else
//     {
//         Serial.printf("Done\n");
//         // char ssid[] = "Harry's iPhone 16 Pro";
//         // char pass[] = "66336439";
//         char ssid[] = "iPhone1000";
//         char pass[] = "h18306215568";
//         err = nvs_set_str(my_handle, "ssid", ssid);
//         err |= nvs_set_str(my_handle, "pass", pass);
//         Serial.printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

//         Serial.printf("Committing updates in NVS ... ");
//         err = nvs_commit(my_handle);
//         Serial.printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
//         nvs_close(my_handle);
//     }
//     Serial.printf("Setup completed.\n");
// }

// void loop()
// {
//     // put your main code here, to run repeatedly:
//     delay(1000);
// }

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

// Part2

// // This example downloads the URL "http://arduino.cc/"
// char ssid[50] = ""; // your network SSID (name)
// char pass[50] = ""; // your network password (use for WPA, or use
// // as key for WEP)
// // Name of the server we want to connect to
// const char kHostname[] = "worldtimeapi.org";
// // Path to download (this is the bit after the hostname in the URL
// // that you want to download
// const char kPath[] = "/api/timezone/Europe/London.txt";
// // Number of milliseconds to wait without receiving any data before we give up
// const int kNetworkTimeout = 30 * 1000;
// // Number of milliseconds to wait if no data is available before trying again
// const int kNetworkDelay = 1000;

// void nvs_access()
// {
//     // Initialize NVS
//     esp_err_t err = nvs_flash_init();
//     if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
//         err == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     {
//         // NVS partition was truncated and needs to be erased
//         // Retry nvs_flash_init
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         err = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(err);
//     // Open
//     Serial.printf("\n");
//     Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
//     nvs_handle_t my_handle;
//     err = nvs_open("storage", NVS_READWRITE, &my_handle);
//     if (err != ESP_OK)
//     {
//         Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     }
//     else
//     {
//         Serial.printf("Done\n");
//         Serial.printf("Retrieving SSID/PASSWD\n");
//         size_t ssid_len;
//         size_t pass_len;
//         err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
//         err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
//         switch (err)
//         {
//         case ESP_OK:
//             Serial.printf("Done\n");
//             Serial.printf("SSID = %s\n", ssid);
//             Serial.printf("PASSWD = %s\n", pass);
//             break;
//         case ESP_ERR_NVS_NOT_FOUND:
//             Serial.printf("The value is not initialized yet!\n");
//             break;
//         default:
//             Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
//         }
//     }
//     // Close
//     nvs_close(my_handle);
// }

// void setup()
// {
//     Serial.begin(9600);
//     delay(4000);
//     // Retrieve SSID/PASSWD from flash before anything else
//     nvs_access();
//     // We start by connecting to a WiFi network
//     delay(1000);
//     Serial.println();
//     Serial.println();
//     Serial.print("Connecting to ");
//     Serial.println(ssid);
//     WiFi.begin(ssid, pass);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("");
//     Serial.println("WiFi connected");
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());
//     Serial.println("MAC address: ");
//     Serial.println(WiFi.macAddress());
// }

// void loop()
// {
//     int err = 0;
//     WiFiClient c;
//     HttpClient http(c);
//     // err = http.get(kHostname, kPath);
//     err = http.get("54.81.29.224", 5000, "/?var=10", NULL);

//     if (err == 0)
//     {
//         Serial.println("startedRequest ok");
//         err = http.responseStatusCode();
//         if (err >= 0)
//         {
//             Serial.print("Got status code: ");
//             Serial.println(err);
//             // Usually you'd check that the response code is 200 or a
//             // similar "success" code (200-299) before carrying on,
//             // but we'll print out whatever response we get
//             err = http.skipResponseHeaders();
//             if (err >= 0)
//             {
//                 int bodyLen = http.contentLength();
//                 Serial.print("Content length is: ");
//                 Serial.println(bodyLen);
//                 Serial.println();
//                 Serial.println("Body returned follows:");
//                 // Now we've got to the body, so we can print it out
//                 unsigned long timeoutStart = millis();
//                 char c;
//                 // Whilst we haven't timed out & haven't reached the end of the body
//                 while ((http.connected() || http.available()) &&
//                        ((millis() - timeoutStart) < kNetworkTimeout))
//                 {
//                     if (http.available())
//                     {
//                         c = http.read();
//                         // Print out this character
//                         Serial.print(c);
//                         bodyLen--;
//                         // We read something, reset the timeout counter
//                         timeoutStart = millis();
//                     }
//                     else
//                     {
//                         // We haven't got any data, so let's pause to allow some to
//                         // arrive
//                         delay(kNetworkDelay);
//                     }
//                 }
//             }
//             else
//             {
//                 Serial.print("Failed to skip response headers: ");
//                 Serial.println(err);
//             }
//         }
//         else
//         {
//             Serial.print("Getting response failed: ");
//             Serial.println(err);
//         }
//     }
//     else
//     {
//         Serial.print("Connect failed: ");
//         Serial.println(err);
//     }
//     http.stop();
//     // And just stop, now that we've tried a download
//     while (1)
//         ;
// }

// Part3

#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_AHTX0.h>
#include <nvs_flash.h>
#include <nvs.h>

// Initialize the DHT20 sensor
Adafruit_AHTX0 aht;

// WiFi credentials
char ssid[50] = ""; // your network SSID (name)
char pass[50] = ""; // your network password (use for WPA, or use as key for WEP)

// HTTP server details
const char kHostname[] = "54.81.29.224"; // Replace with your server IP
const int kPort = 5000;
const char kPath[] = "/data"; // Endpoint to send sensor data

const int kNetworkTimeout = 30 * 1000; // Timeout in milliseconds
const int kNetworkDelay = 1000;        // Delay in milliseconds

void nvs_access()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS storage
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        size_t ssid_len, pass_len;
        err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
        err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
        if (err == ESP_OK)
        {
            Serial.printf("Retrieved SSID: %s\n", ssid);
            Serial.printf("Retrieved Password: %s\n", pass);
        }
        else
        {
            Serial.printf("Error (%s) retrieving WiFi credentials!\n", esp_err_to_name(err));
        }
    }
    nvs_close(my_handle);
}

void setup()
{
    Serial.begin(9600);
    delay(4000);

    // Initialize NVS and WiFi credentials
    nvs_access();

    // Initialize WiFi
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize DHT20 sensor
    if (!aht.begin())
    {
        Serial.println("Failed to initialize DHT20 sensor! Check wiring.");
        while (1)
            ;
    }
    Serial.println("DHT20 sensor initialized successfully.");
}

void loop()
{
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    // Read temperature and humidity
    float temperature = temp.temperature;
    float hum = humidity.relative_humidity;

    if (isnan(temperature) || isnan(hum))
    {
        Serial.println("Failed to read from DHT20 sensor!");
        delay(5000); // Retry after delay
        return;
    }

    // Print temperature and humidity to console
    Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", temperature, hum);

    // Send data to the server
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(String("http://") + kHostname + ":" + kPort + kPath);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = String("{\"temperature\":") + temperature +
                             ",\"humidity\":" + hum + "}";

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0)
        {
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        }
        else
        {
            Serial.printf("HTTP POST failed: %s\n", http.errorToString(httpResponseCode).c_str());
        }

        http.end();
    }
    else
    {
        Serial.println("WiFi disconnected. Reconnecting...");
        WiFi.reconnect();
    }

    delay(10000); // Delay for 10 seconds
}