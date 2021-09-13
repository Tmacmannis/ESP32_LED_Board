void Task1code(void* pvParameters) {
    TelnetStream.print("Task1 running on core ");
    TelnetStream.println(xPortGetCoreID());

    for (;;) {
        delay(28);
        client.loop();  // takes 60 micro seconds to complete, fast...
        unsigned long currentMillis = millis();
    }
}

void onConnectionEstablished() {
    client.subscribe("ledboard/brightness", [](const String& payload) {
        TelnetStream.print("brightness payload is: ");
        TelnetStream.println(payload);
        brightness = map(payload.toInt(), 3, 255, 0, 255);
    });

    client.subscribe("ledboard/OnOff", [](const String& payload) {
        TelnetStream.print("OnOff payload is: ");
        TelnetStream.println(payload);
        if (payload == "OFF") {
            if (lightsOn) {
                lastBrightness = brightness;
                brightness = 0;
                lightsOn = false;
            }
        } else {
            if (!lightsOn) {
                lightsOn = true;
                brightness = lastBrightness;
            }
        }
    });

    client.subscribe("ledboard/effects", [](const String& payload) {
        TelnetStream.print("Effect payload is: ");
        TelnetStream.println(payload);
        setAnimation(payload);
    });
}
