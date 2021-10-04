int foodX;
int foodY;



class Snake {
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

        if (xPos[0] == foodX && yPos[0] == foodY) {
            placeFood();
            drawFood();
            snakeSize++;
        }

        if (!getValidMoves()) {
            if (snakeSize > maxSize) {
                maxSize = snakeSize;
                TelnetStream.println("New Record!");
            }
            TelnetStream.print("Snake reached a size of: ");
            TelnetStream.println(snakeSize);
            TelnetStream.print("Max size of: ");
            TelnetStream.println(maxSize);
            snakeSize = 1;
            return;
        }

        int direction = chooseMove(false);

        if (direction == 0) {
            newY--;
        }
        if (direction == 1) {
            newX++;
        }
        if (direction == 2) {
            newY++;
        }
        if (direction == 3) {
            newX--;
        }

        shiftArrays(newX, newY);
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

    boolean getValidMoves() {
        int xPossible[4];
        int yPossible[4];

        //up
        xPossible[0] = xPos[0];
        yPossible[0] = yPos[0] - 1;
        //right
        xPossible[1] = xPos[0] + 1;
        yPossible[1] = yPos[0];
        //down
        xPossible[2] = xPos[0];
        yPossible[2] = yPos[0] + 1;
        //left
        xPossible[3] = xPos[0] - 1;
        yPossible[3] = yPos[0];

        boolean moveFound = false;

        for (int i = 0; i < 4; i++) {
            if (xInRange(xPossible[i]) && yInRange(yPossible[i]) && !tailCollision(i)) {
                validMoves[i] = 1;
                moveFound = true;
            } else {
                validMoves[i] = 0;
            }
        }

        if (moveFound) {
            return true;
        } else {
            TelnetStream.println("******Move not found!");
            return false;
        }
    }

    int chooseMove(boolean randomMove) {
        int direction;
        if (randomMove) {
            while (true) {
                direction = random(4);
                if (validMoves[direction] != 0) {
                    return direction;
                }
            }
        } else {
            // 0 = up, 1 = right, 2 = down, 3 = left
            if ((xPos[0] < foodX) && validMoves[1] == 1) {
                return 1;
            }
            if ((xPos[0] > foodX) && validMoves[3] == 1) {
                return 3;
            }
            if ((yPos[0] < foodY) && validMoves[2] == 1) {
                return 2;
            }
            if ((yPos[0] > foodY) && validMoves[0] == 1) {
                return 0;
            }
            while (true) {
                direction = random(4);
                if (validMoves[direction] != 0) {
                    return direction;
                }
            }
        }
    }

    boolean tailCollision(int newDir) {
        // 0 = up, 1 = right, 2 = down, 3 = left
        int newX;
        int newY;
        switch (newDir) {
            case 0:
                newX = xPos[0];
                newY = yPos[0] - 1;
                break;
            case 1:
                newX = xPos[0] + 1;
                newY = yPos[0];
                break;
            case 2:
                newX = xPos[0];
                newY = yPos[0] + 1;
                break;
            case 3:
                newX = xPos[0] - 1;
                newY = yPos[0];
                break;
        }

        for (int i = 0; i < snakeSize; i++) {
            if (xPos[i] == newX && yPos[i] == newY) {
                return true;
            }
        }

        return false;
    }
};

Snake snake1;
boolean initialSetup = true;
void makeSnakes() {
    EVERY_N_MILLISECONDS_I(timingObj, 100) {
        timingObj.setPeriod(snakeSpeed);
        if (initialSetup) {
            placeFood();
            initialSetup = false;
        }
        snake1.moveSnake();
        snake1.drawSnake();
        drawFood();
        FastLED.show();
    }
}

boolean xInRange(int x) {
    if (x >= 0 && x < 32) {
        return true;
    } else {
        return false;
    }
}

boolean yInRange(int y) {
    if (y >= 0 && y < 16) {
        return true;
    } else {
        return false;
    }
}

void placeFood() {
    while (true) {
        foodX = random(32);
        foodY = random(16);

        boolean foodCollision = false;
        for (int i = 0; i < snake1.snakeSize; i++) {
            if (snake1.xPos[i] == foodX && snake1.yPos[i] == foodY) {
                foodCollision = true;
                break;
            }
        }
        if (!foodCollision) {
            return;
        }
    }
}
void drawFood() {
    leds.DrawPixel(foodX, foodY, CHSV(96, 255, brightness));
}
