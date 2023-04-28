void Task1code(void* pvParameters) {
    TelnetStream.print("Task1 running on core ");
    TelnetStream.println(xPortGetCoreID());

    for (;;) {
        delay(28);
        client.loop();  // takes 60 micro seconds to complete, fast...
        unsigned long currentMillis = millis();
        // EVERY_N_MILLISECONDS(1000) {
        //     TelnetStream.print("lightsOn: ");
        //     TelnetStream.print(lightsOn);
        //     TelnetStream.print(" brightness: ");
        //     TelnetStream.print(brightness);
        //     TelnetStream.print(" current state: ");
        //     TelnetStream.println(currentState);
        // }

        EVERY_N_MILLISECONDS(500) {
            if (lightsOn) {
                client.publish("ledboard/OnOffState", "ON");
                client.publish("ledboard/set_speed", currentSpeed(currentState));
            } else {
                client.publish("ledboard/OnOffState", "OFF");
            }
            client.publish("ledboard/brightnessState", String(homeAssitantBrightness));
            client.publish("ledboard/effectState", animationName);
        }
    }
}

void onConnectionEstablished() {
    client.subscribe("ledboard/brightness", [](const String& payload) {
        TelnetStream.print("brightness payload is: ");
        TelnetStream.println(payload);
        homeAssitantBrightness = payload.toInt();
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
                lastState = currentState;
                currentState = 0;
            }
        } else {
            if (!lightsOn) {
                lightsOn = true;
                brightness = lastBrightness;
                currentState = lastState;
            }
        }
    });

    client.subscribe("ledboard/effects", [](const String& payload) {
        TelnetStream.print("Effect payload is: ");
        TelnetStream.println(payload);
        animationName = payload;
        setAnimation(animationName);
    });

    client.subscribe("ledboard/speed_slider", [](const String& payload) {
        TelnetStream.print("Speed payload is: ");
        TelnetStream.println(payload);
        assignSpeed(payload.toInt(), currentState);
    });

    client.subscribe("python/mqtt", [](const String& payload) {
        TelnetStream.print("Controller payload is: ");
        TelnetStream.println(payload);
        snakeDir=payload;
    });
}
