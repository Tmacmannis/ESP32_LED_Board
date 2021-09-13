#define FASTLED_ALLOW_INTERRUPTS 0
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <LEDMatrix.h>
#include <credentials.h>

#include "EspMQTTClient.h"
#include "OTA.h"

#define PIN 23
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 16
#define NUMMATRIX (MATRIX_WIDTH * MATRIX_HEIGHT)
// #define MATRIX_TYPE (MTX_MATRIX_TOP + MTX_MATRIX_LEFT + MTX_MATRIX_ROWS + MTX_MATRIX_PROGRESSIVE)
#define MATRIX_TYPE HORIZONTAL_MATRIX
#define NUM_LEDS MATRIX_WIDTH *MATRIX_HEIGHT

int brightness = 0;
int lastBrightness = 0;
int homeAssitantBrightness;
int testCount = 0;
boolean lightsOn = false;
int currentState;
int lastState;
String animationName;

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

CRGB *matrixleds = leds[0];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_WIDTH, MATRIX_HEIGHT, 1, 1,
                                                  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                                      NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);

const uint16_t colors[] = {
    matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255)};

EspMQTTClient client(
    mySSID,
    myPASSWORD,
    mqttIP,          // MQTT Broker server ip
    "tim",           // Can be omitted if not needed
    "14Q4YsC6YrXl",  // Can be omitted if not needed
    "LEDBoard",      // Client name that uniquely identify your device
    haPORT           // The MQTT port, default to 1883. this line can be omitted
);

TaskHandle_t Task1;

void setup() {
    setupOTA("LEDBoard", mySSID, myPASSWORD);
    TelnetStream.begin();

    client.enableDebuggingMessages();                                           // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater();                                              // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
    client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
    client.enableDebuggingMessages(true);

    FastLED.addLeds<WS2812B, PIN, RGB>(matrixleds, NUMMATRIX);
    matrix->begin();
    matrix->setTextWrap(false);
    matrix->setBrightness(255);
    matrix->setTextColor(colors[0]);

    xTaskCreatePinnedToCore(
        Task1code, /* Task function. */
        "Task1",   /* name of task. */
        10000,     /* Stack size of task */
        NULL,      /* parameter of the task */
        1,         /* priority of the task */
        &Task1,    /* Task handle to keep track of created task */
        0);        /* pin task to core 0 */
    delay(500);
}

void loop() {
    handleClientTest();

    ledStateMachine();
}

void ledStateMachine() {
    switch (currentState) {
        case 0:  //off
            EVERY_N_MILLISECONDS(20) {
               FastLED.clear(true);
            }
            break;
        case 1:  //Rainbow
            testAnimation();
            break;
        case 2:  //Fireworks
            showFireworks();
            break;
        case 3:
            break;
    }
}

void setAnimation(String payload) {
    if (payload == "Rainbow Effects") {
        currentState = 1;
    }
    if (payload == "Fireworks") {
        currentState = 2;
    }
    if (payload == "Random Blinks") {
        currentState = 3;
    }
    if (payload == "Spectrum") {
        currentState = 4;
    }
    if (payload == "Scrolling Spectrum") {
        currentState = 5;
    }
    if (payload == "Custom Text") {
        currentState = 6;
    }
    if (payload == "Random") {
        currentState = 7;
    }
}
