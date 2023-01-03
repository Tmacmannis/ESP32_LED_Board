#define FASTLED_ALLOW_INTERRUPTS 0
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <LEDMatrix.h>
#include <arduinoFFT.h>
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
int snakeSpeed = 100;
int rainSpeed = 100;
int rainbowSpeed = 3;

int rainbowSpeedOrig = 3;
int snakeSpeedOrig = 50;
int rainSpeedOrig = 50;

int cursorPos = 32;

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

//Audio stuff ***************************
#define SAMPLES 512                                // Must be a power of 2
#define SAMPLING_FREQ 40000                         // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE 6000                              // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
#define AUDIO_IN_PIN 35                             // Signal in on this pin
#define LED_PIN 23                                  // LED strip data
#define BTN_PIN 4                                   // Connect a push button to this pin to change patterns
#define LONG_PRESS_MS 200                           // Number of ms to count as a long press
#define COLOR_ORDER GRB                             // If colours look wrong, play with this
#define CHIPSET WS2812B                             // LED strip type
#define MAX_MILLIAMPS 6000                          // Careful with the amount of power here if running off USB port
const int BRIGHTNESS_SETTINGS[3] = {5, 70, 200};    // 3 Integer array for 3 brightness settings (based on pressing+holding BTN_PIN)
#define LED_VOLTS 5                                 // Usually 5 or 12
#define NUM_BANDS 16                                // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE 500                                   // Used as a crude noise filter, values below this are ignored
const uint8_t kMatrixWidth = 32;                    // Matrix width
const uint8_t kMatrixHeight = 16;                   // Matrix height
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)     // Total number of LEDs
#define BAR_WIDTH (kMatrixWidth / (NUM_BANDS - 1))  // If width >= 8 light 1 LED width per bar, >= 16 light 2 LEDs width bar etc
#define TOP (kMatrixHeight - 0)                     // Don't allow the bars to go offscreen
#define SERPENTINE true                             // Set to false if you're LEDS are connected end to end, true if serpentine

// Sampling and FFT stuff
unsigned int sampling_period_us;
byte peak[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // The length of these arrays must be >= NUM_BANDS
int oldBarHeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int bandValues[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

DEFINE_GRADIENT_PALETTE(purple_gp){
    0, 0, 212, 255,     //blue
    255, 179, 0, 255};  //purple
DEFINE_GRADIENT_PALETTE(outrun_gp){
    0, 141, 0, 100,    //purple
    127, 255, 192, 0,  //yellow
    255, 0, 5, 255};   //blue
DEFINE_GRADIENT_PALETTE(greenblue_gp){
    0, 0, 255, 60,     //green
    64, 0, 236, 255,   //cyan
    128, 0, 5, 255,    //blue
    192, 0, 236, 255,  //cyan
    255, 0, 255, 60};  //green
DEFINE_GRADIENT_PALETTE(redyellow_gp){
    0, 200, 200, 200,     //white
    64, 255, 218, 0,      //yellow
    128, 231, 0, 0,       //red
    192, 255, 218, 0,     //yellow
    255, 200, 200, 200};  //white
CRGBPalette16 purplePal = purple_gp;
CRGBPalette16 outrunPal = outrun_gp;
CRGBPalette16 greenbluePal = greenblue_gp;
CRGBPalette16 heatPal = redyellow_gp;
uint8_t colorTimer = 0;

int buttonPushCounter = 0;
// **************************************

void setup() {
    setupOTA("LEDBoard", mySSID, myPASSWORD);
    TelnetStream.begin();

    client.enableDebuggingMessages(false);                                           // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater();                                              // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
    client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true

    FastLED.addLeds<WS2812B, PIN, RGB>(matrixleds, NUMMATRIX);
    matrix->begin();
    matrix->setTextWrap(false);
    matrix->setBrightness(255);
    matrix->setTextColor(colors[0]);

    sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));

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

    ledStateMachine(currentState);

    matrix->setBrightness(255);
    FastLED.setBrightness(255);
}

void ledStateMachine(int showNum) {
    switch (showNum) {
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
            randomBlinks();
            break;
        case 4:
            drawRain();
            break;
        case 5:
            makeSnakes();
            break;
        case 6:
            happyHalloweenText();
            break;
        case 7:
            runSpectrums();
            break;
        case 8:
            randomShow();
            break;
    }
}

int randomShowNum;

void randomShow() {
    EVERY_N_MILLISECONDS_I(timingObj, 10000) {
        randomShowNum = random(1, 8);
        timingObj.setPeriod(random(30000, 90000));
    }

    ledStateMachine(randomShowNum);
}

int pickRandomShow() {}

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
    if (payload == "Rain") {
        currentState = 4;
    }
    if (payload == "Snakes") {
        currentState = 5;
    }
    if (payload == "Halloween") {
        currentState = 6;
    }
    if (payload == "Spectrums") {
        currentState = 7;
    }
    if (payload == "Random") {
        currentState = 8;
    }
}

void assignSpeed(int speed, int stateNum) {
    switch (stateNum) {
        case 1:  //Rainbow
            rainbowSpeedOrig = speed;
            rainbowSpeed = map(speed, 0, 100, 1, 6);
            break;
        case 2:  //Fireworks
            // showFireworks();
            break;
        case 3:
            // randomBlinks();
            break;
        case 4:
            rainSpeedOrig = speed;
            rainSpeed = map(speed, 0, 100, 500, 20);
            break;
        case 5:
            snakeSpeedOrig = speed;
            snakeSpeed = map(speed, 0, 100, 500, 20);
            break;
        case 6:
            assignSpeed(speed, randomShowNum);
            break;
    }
}

String currentSpeed(int currentSpeedNum) {
    switch (currentSpeedNum) {
        case 1:  //Rainbow
            return String(rainbowSpeedOrig);
            break;
        case 2:  //Fireworks
            // showFireworks();
            break;
        case 3:
            // randomBlinks();
            break;
        case 4:
            return String(rainSpeedOrig);
            break;
        case 5:
            return String(snakeSpeedOrig);
            break;
        case 6:
            return currentSpeed(randomShowNum);
            break;
    }
    return "50";
}
