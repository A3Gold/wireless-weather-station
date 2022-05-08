// Purpose: Wireless Weather Station Receiver.
// Reference: https://bit.ly/3gxXKML
// Author: A. Goldman
// Date: May 30, 2020
// Status: Working

#include <SPI.h>      // Includes SPI library
#include <nRF24L01.h> // Includes NRF24L01 libraries...
#include <RF24.h>
#define B1PIN A0                 // Defines button 1 pin
#define B2PIN A1                 // Defines button 2 pin
#define B3PIN A2                 // Defines button 3 pin
#define DATAPIN 2                // Defines data pin
#define CLOCKPIN 3               // Defines clock pin
#define LATCHPIN 4               // Defines latch pin
#define CE 7                     // Defines CE pin
#define CSN 8                    // Defines CSN pin
RF24 radio(CE, CSN);             // Creates RF object
const byte address[6] = "00001"; // RF communication address
struct sensorData
{                        // Struct to contain incoming sensor data
    float temp = 0;      // Float to hold temp.
    float hum = 0;       // Float to hold hum.
    uint8_t negTemp = 0; // Variable; 0 = + temp., 1 = = temp.
};
struct sensorData data; // Assigns data as name for struct
float maxTemp;          // Float to hold maximum temp.
float maxHum;           // Float to hold maximum hum.
float minTemp;          // Float to hold minimum temp.
float minHum;           // Float to hold minimum hum.
float d1Data;           // Float to hold display 1 data
float d2Data;           // Float to hold display 2 data
uint8_t i = 0;          // i variable; used to set max. and min. values
uint8_t segmentMap[] = {
    // Segment map for TPIC6C595N
    0b11111100, // Segment map for 0
    0b01100000, // Segment map for 1
    0b11011010, // Segment map for 2
    0b11110010, // Segment map for 3
    0b01100110, // Segment map for 4
    0b10110110, // Segment map for 5
    0b10111110, // Segment map for 6
    0b11100000, // Segment map for 7
    0b11111110, // Segment map for 8
    0b11100110, // Segment map for 9
    0b00000010  // Segment map for - sign
};
uint8_t segmentMapWD[] = {
    // Segment map with decimal point
    0b11111101, // Segment map for 0
    0b01100001, // Segment map for 1
    0b11011011, // Segment map for 2
    0b11110011, // Segment map for 3
    0b01100111, // Segment map for 4
    0b10110111, // Segment map for 5
    0b10111111, // Segment map for 6
    0b11100001, // Segment map for 7
    0b11111111, // Segment map for 8
    0b11100111  // Segment map for 9
};

void setup()
{
    pinMode(CLOCKPIN, OUTPUT);         // Defines clock pin as output
    pinMode(DATAPIN, OUTPUT);          // Defines data pin as output
    pinMode(LATCHPIN, OUTPUT);         // Defines latch pin as output
    pinMode(B1PIN, INPUT);             // Defines button 1 pin as output
    pinMode(B2PIN, INPUT);             // Defines button 2 pin as output
    pinMode(B3PIN, INPUT);             // Defines button 3 pin as output
    radio.begin();                     // Initializes RF module
    radio.openReadingPipe(0, address); // New data into pipe 0
    radio.setPALevel(RF24_PA_MIN);     // Sets RF power level
    radio.startListening();            // Start looking for RF data
}

void loop()
{
    if (radio.available())
    {                                    // If radio signal available...
        radio.read(&data, sizeof(data)); // Read, put in struct
        if (i == 0)
        {
                                     // Only true first time loop runs
            minTemp = data.temp;     // Sets minimum temp.
            maxTemp = data.temp;     // Sets maxmimum temp.
            minHum = data.hum;       // Sets minimum hum.
            maxHum = data.hum;       // Sets maximum hum.
            i++;                     // Increase i; loop will not run again
        }
    }
    if (data.temp < minTemp)
    {                        // If temp. is < minimum temp....
        minTemp = data.temp; // minimum temp. = temp.
    }
    if (data.temp > maxTemp)
    {                        // If temp. is > maximum temp....
        maxTemp = data.temp; // maximum temp. = temp.
    }
    if (data.hum < minHum)
    {                      // If hum. is < minimum hum....
        minHum = data.hum; // minimum hum. = hum.
    }
    if (data.hum > maxHum)
    {                      // If hum. is > maximum hum....
        maxHum = data.hum; // maximum hum. = hum.
    }
    if (digitalRead(B1PIN) == LOW)
    {                       // If button 1 pressed...
        d1Data = data.temp; // data for display 1 = temp.
        d2Data = data.hum;  // data for display 2 = hum.
    }
    if (digitalRead(B2PIN) == LOW)
    {                     // If button 2 pressed...
        d1Data = maxTemp; // data for display 1 = maximum temp.
        d2Data = minTemp; // data for display 2 = minimum temp.
    }
    if (digitalRead(B3PIN) == LOW)
    {                    // If button 3 pressed...
        d1Data = maxHum; // data for display 1 = maximum hum.
        d2Data = minHum; // data for display 2 = minimum hum.
    }
    int8_t d1Tens = d1Data / 10;             // Display 1, digit 2
    int8_t d1Units = d1Data - (d1Tens * 10); // Display 1, digit 3
    // Display 1, digit 4...
    int8_t d1Decimal = (d1Data - (d1Tens * 10) - d1Units) * 10;
    int8_t d2Tens = d2Data / 10; // Display 2, digit 2
    int8_t d2Units = d2Data - (d2Tens * 10);
    Display 2, digit 3
    // Display 2, digit 4
    int8_t d2Decimal = (d2Data - (d2Tens * 10) - d2Units) * 10;
    if (data.negTemp == 0)
    {                                // If temp. positive...
        digitalWrite(LATCHPIN, LOW); // Latch low
        // make first digit a zero
        shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b10000000);
        shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[0]);
        digitalWrite(LATCHPIN, HIGH); // Latch high
    }
    else
    {                                // If temp. negative...
        digitalWrite(LATCHPIN, LOW); // Latch low
        // make first digit a - sign
        shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b10000000);
        shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[10]);
        digitalWrite(LATCHPIN, HIGH); // Latch high
    }
    digitalWrite(LATCHPIN, LOW); // Latch low
    // Display 1, tens digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b01000000);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[d1Tens]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
    digitalWrite(LATCHPIN, LOW);  // Latch low
    // Display 1, units digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b00100000);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMapWD[d1Units]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
    digitalWrite(LATCHPIN, LOW);  // Latch low
    // Display 1, decimal digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b00010000);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[d1Decimal]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
    digitalWrite(LATCHPIN, LOW);  // Latch low
    // Display 2, zero for first digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b00001000);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[0]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
    digitalWrite(LATCHPIN, LOW);  // Latch low
    // Display 2, tens digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b00000100);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[d2Tens]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
    digitalWrite(LATCHPIN, LOW);  // Latch low
    // Display 2, units digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b00000010);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMapWD[d2Units]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
    digitalWrite(LATCHPIN, LOW);  // latch low
    // Display 2, decimal digit
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, 0b0000001);
    shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, segmentMap[d2Decimal]);
    digitalWrite(LATCHPIN, HIGH); // Latch high
}
