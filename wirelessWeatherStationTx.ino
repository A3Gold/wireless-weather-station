// Purpose: Wireless Weather Station Transmitter.
// Reference: https://bit.ly/3gxXKML
// Author: A. Goldman
// Date: May 30, 2020
// Status: Working

#include <TimerOne.h> // Includes TimerOne library
#include <DHT.h>      // Includes DHT library
#include <SPI.h>      // Includes SPI library
#include <nRF24L01.h> // Includes NRF24L01 libraries...
#include <RF24.h>
#define DHTPIN 2                 // Defines DHT sensor pin
#define DHTTYPE DHT22            // Defines DHT sensor type
#define CE 5                     // Defines RF CE pin
#define CSN 6                    // Defines RF CSN pin
DHT dht(DHTPIN, DHTTYPE);        // Creates DHT object
RF24 radio(CE, CSN);             // Creates RF object
const byte address[6] = "00001"; // RF communication address
struct sensorData
{                        // Struct to contain sensor data
    float temp = 0;      // Float to hold temp.
    float hum = 0;       // Float to hold hum.
    uint8_t negTemp = 0; // Variable; 0 = + temp., 1 = = temp.
};
struct sensorData data; // Assigns data as name for struct

void setup()
{
    radio.begin();                        // Initializes RF module
    radio.openWritingPipe(address);       // Sets RF address
    radio.setPALevel(RF24_PA_MIN);        // Sets RF power level
    radio.stopListening();                // Ends initialization
    dht.begin();                          // Initializes DHT22
    Timer1.initialize(3000000);           // Initializes timer; in microseconds
    Timer1.attachInterrupt(ISR_ReadData); // Function called when timer up
}

void ISR_ReadData()
{                                      // Function to read data from sensor...
    data.temp = dht.readTemperature(); // Assigns temp. data to data.temp
    data.hum = dht.readHumidity();     // Assigns hum. data to data.hum
    if (data.temp < 0)
    {                               // If temp. below 0...
        data.negTemp = 1;           // data.negTemp = 1
        data.temp = data.temp * -1; // Make data.temp positive by * -1
    }
    else
    {                     // Otherwise...
        data.negTemp = 0; // data.negTemp = 0
    }
}

void loop()
{
    radio.write(&data, sizeof(data)); // Transmits data via RF
}
