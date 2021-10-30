void happyHalloweenText() {
    EVERY_N_MILLISECONDS(125) {
		matrix->setTextColor(matrix->Color(255, 165, 0));
        matrix->setBrightness(brightness);
        matrix->fillScreen(0);
        matrix->setCursor(cursorPos, 0);
        matrix->print("Happy");
        matrix->setCursor(cursorPos, 9);
        matrix->print("Halloween");
        matrix->setBrightness(brightness);
		FastLED.show();
		cursorPos--;
    }

	if(cursorPos < -55 ){
		cursorPos = 32;
	}

    // while (cursorPos > -5) {
    //     matrix->fillScreen(0);
    //     matrix->setCursor(cursorPos, 5);
    //     matrix->print("Jordan");
    //     FastLED.show();
    //     delay(100);
    //     cursorPos--;
    // }
    // matrix->setBrightness(brightness);
    // delay(500);

    // matrix->fillScreen(0);
    // matrix->setCursor(0, 0);
    // matrix->print("Pull");
    // matrix->setCursor(10, 9);
    // matrix->print("Me");
    // FastLED.show();
    // matrix->setBrightness(brightness);
    // delay(500);

    // cursorPos = 2;
    // matrix->fillScreen(0);
    // matrix->setCursor(5, 0);
    // matrix->print("A");
    // matrix->setCursor(cursorPos, 9);
    // matrix->print("Mother");
    // FastLED.show();
    // delay(200);

    // while (cursorPos > -5) {
    //     matrix->setBrightness(brightness);
    //     matrix->fillScreen(0);
    //     matrix->setCursor(5, 0);
    //     matrix->print("A");
    //     matrix->setCursor(cursorPos, 9);
    //     matrix->print("Mother");
    //     FastLED.show();
    //     delay(50);
    //     cursorPos--;
    // }

    // delay(200);

    // matrix->fillScreen(0);
    // matrix->setCursor(0, 0);
    // matrix->print("Fucking");
    // matrix->setBrightness(brightness);
    // // matrix->setCursor(6, 9);
    // // matrix->print("Grav");
    // FastLED.show();
    // delay(200);

    // cursorPos = 0;
    // while (cursorPos > -10) {
    //     matrix->setBrightness(brightness);
    //     cursorPos--;
    //     matrix->fillScreen(0);
    //     matrix->setCursor(cursorPos, 0);
    //     matrix->print("Fucking");
    //     // matrix->setCursor(6, 9);
    //     // matrix->print("Grav");
    //     FastLED.show();
    //     delay(50);
    // }

    // delay(200);

    // matrix->fillScreen(0);
    // matrix->setCursor(cursorPos, 0);
    // matrix->print("Fucking");
    // matrix->setTextColor(matrix->Color(255, 128, 0));
    // matrix->setBrightness(brightness);
    // matrix->setCursor(6, 9);
    // matrix->print("Grav");
    // FastLED.show();
    // delay(750);

    // matrix->setTextColor(matrix->Color(255, 128, 0));
    // matrix->setBrightness(brightness);
    // matrix->fillScreen(0);
    // matrix->setCursor(0, 6);
    // matrix->print("Please");
    // FastLED.show();
    // delay(200);

    // cursorPos = 0;
    // while (cursorPos > -5) {
    //     matrix->fillScreen(0);
    //     matrix->setCursor(cursorPos, 6);
    //     matrix->print("Please");
    //     FastLED.show();
    //     delay(50);
    //     cursorPos--;
    // }

    // delay(2000);
}