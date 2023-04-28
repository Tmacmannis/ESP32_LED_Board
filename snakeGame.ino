int foodXGame;
int foodYGame;



class SnakeGame {
   public:
    int xPos[NUM_LEDS];
    int yPos[NUM_LEDS];
    int snakeSize = 1;
    int maxSize = 0;
    int direction = 0;  // 0 = up, 1 = right, 2 = down, 3 = left
    int validMoves[4];

    void drawSnake() {
        for (int i = 0; i < snakeSize; i++) {
            if (i == 0) {
                leds.DrawPixel(xPos[i], yPos[i], CHSV(150, 255, brightness));
            } else {
                leds.DrawPixel(xPos[i], yPos[i], CHSV(0, 255, brightness));
            }
        }
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            for (int y = 0; y < MATRIX_HEIGHT; y++) {
                boolean inArray = false;
                for (int i = 0; i < snakeSize; i++) {
                    if (xPos[i] == x && yPos[i] == y) {
                        inArray = true;
                        break;
                    }
                }
                if (!inArray) {
                    leds.DrawPixel(x, y, CHSV(0, 0, 0));
                }
            }
        }
    }

    void moveSnake() {
        int newX = xPos[0];
        int newY = yPos[0];

        if (xPos[0] == foodXGame && yPos[0] == foodYGame) {
            placeFoodGame();
            drawFoodGame();
            snakeSize++;
        }

        

        // int direction = chooseMove(false);

        if (snakeDir == "Up") {
            TelnetStream.println("Going Up");
            newY--;
        }
        if (snakeDir == "Right") {
            TelnetStream.println("Going Right");
            newX++;
        }
        if (snakeDir == "Down") {
            TelnetStream.println("Going Down");
            newY++;
        }
        if (snakeDir == "Left") {
            TelnetStream.println("Going Left");
            newX--;
        }

        shiftArrays(newX, newY);

        if (tailCollision() || outOfBounds()) {
            if (snakeSize > maxSize) {
                maxSize = snakeSize;
                TelnetStream.println("New Record!");
            }
            TelnetStream.print("Snake reached a size of: ");
            TelnetStream.println(snakeSize);
            TelnetStream.print("Max size of: ");
            TelnetStream.println(maxSize);
            
            matrix->setTextColor(matrix->Color(255, 165, 0));
            matrix->setBrightness(brightness);
            matrix->fillScreen(0);
            matrix->setCursor(3, 0);
            matrix->print("Score: ");
            matrix->setCursor(3, 8);
            matrix->print(snakeSize);

            FastLED.show();
            delay(3000);
            FastLED.clear(true);

            matrix->setCursor(3, 0);
            matrix->print("Best: ");
            matrix->setCursor(3, 8);
            matrix->print(maxSize);
            FastLED.show();
            delay(3000);

            snakeSize = 1;
            xPos[0] = 10;
            yPos[0] = 5;


            return;
        }
    }

    void shiftArrays(int x, int y) {
        // TelnetStream.println("In shift arrays");
        for (int i = snakeSize - 1; i > 0; i--) {
            xPos[i] = xPos[i - 1];
            yPos[i] = yPos[i - 1];
        }
        xPos[0] = x;
        yPos[0] = y;
        // TelnetStream.println("Shifting complete");
    }

    boolean outOfBounds() {
      if (xPos[0] > (MATRIX_WIDTH - 1) || xPos[0] < 0 || yPos[0] > (MATRIX_HEIGHT - 1) || yPos[0] < 0){
        return true;
      }
      return false;
    }

    boolean tailCollision() {
        for (int i = 1; i < snakeSize; i++) {
            if (xPos[i] == xPos[0] && yPos[i] == yPos[0]) {
                return true;
            }
        }
        return false;
    }
};

SnakeGame snake2;
boolean initialGameSetup = true;
void makeSnakesGame() {
    EVERY_N_MILLISECONDS(68) {
        if (initialGameSetup) {
            placeFoodGame();
            initialGameSetup = false;
        }
        snake2.moveSnake();
        snake2.drawSnake();
        drawFoodGame();
        FastLED.show();
    }
}

boolean xInRangeGame(int x) {
    if (x >= 0 && x < 32) {
        return true;
    } else {
        return false;
    }
}

boolean yInRangeGame(int y) {
    if (y >= 0 && y < 16) {
        return true;
    } else {
        return false;
    }
}

void placeFoodGame() {
    while (true) {
        foodXGame = random(2,30);
        foodYGame = random(2,14);

        boolean foodCollision = false;
        for (int i = 0; i < snake2.snakeSize; i++) {
            if (snake2.xPos[i] == foodXGame && snake2.yPos[i] == foodYGame) {
                foodCollision = true;
                break;
            }
        }
        if (!foodCollision) {
            return;
        }
    }
}
void drawFoodGame() {
    leds.DrawPixel(foodXGame, foodYGame, CHSV(96, 255, brightness));
}
