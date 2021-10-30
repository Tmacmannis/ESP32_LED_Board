uint8_t hue = 0;
int16_t counter = 0;

void testAnimation() {
    int16_t sx, sy, x, y;
    uint8_t h;

    FastLED.clear();
    handleClientTest();

    h = hue;
    if (counter < 1125) {
        // ** Fill LED's with diagonal stripes
        for (x = 0; x < (MATRIX_WIDTH + MATRIX_HEIGHT); ++x) {
            leds.DrawLine(x - MATRIX_HEIGHT, MATRIX_HEIGHT - 1, x, 0, CHSV(h, 255, brightness));
            h += 16;
        }
    } else {
        // ** Fill LED's with horizontal stripes
        for (y = 0; y < MATRIX_HEIGHT; ++y) {
            leds.DrawLine(0, y, MATRIX_WIDTH - 1, y, CHSV(h, 255, brightness));
            h += 16;
        }
    }
    hue += rainbowSpeed;

    if (counter < 125)
        ;
    else if (counter < 375)
        leds.HorizontalMirror();
    else if (counter < 625)
        leds.VerticalMirror();
    else if (counter < 875)
        leds.QuadrantMirror();
    else if (counter < 1125)
        leds.QuadrantRotateMirror();
    else if (counter < 1250)
        ;
    else if (counter < 1500)
        leds.TriangleTopMirror();
    else if (counter < 1750)
        leds.TriangleBottomMirror();
    else if (counter < 2000)
        leds.QuadrantTopTriangleMirror();
    else if (counter < 2250)
        leds.QuadrantBottomTriangleMirror();

    counter++;
    if (counter >= 2250)
        counter = 0;
    FastLED.show();
}

uint8_t randomBlinkOffset = 0;
void randomBlinks() {
    EVERY_N_MILLISECONDS(30) {
        fadeToBlackBy(matrixleds, NUMMATRIX, map(brightness, 3, 255, 1, 5));
    }
    EVERY_N_MILLISECONDS(50) {
        int pos = random16(NUMMATRIX);
        matrixleds[pos] += CHSV(randomBlinkOffset + random8(64), 200, brightness);
        matrix->show();
        handleClientTest();
    }
    EVERY_N_MILLISECONDS(100) {
        randomBlinkOffset++;
    }
}