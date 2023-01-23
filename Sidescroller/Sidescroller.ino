#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_LEFT 4
#define PIN_JUMP 8
#define PIN_RIGHT 7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCROLL_POINT 80

#define LOGIC_DEBOUNCE 50

#define H_OFFSET 4
#define SPRITE_DIMEN 10
#define SPIKE_HEIGHT 7
#define PLAYER_RADIUS 5

#define JUMP_VELO 5
#define MIN_VELO -3
#define GRAVITY_VELO -1

#define ENABLE_TIMINGS true

unsigned char mapData[] = { 0x21, 0x30, 0x38, 0x30, 0xa1, 0xa1, 0xa1, 0x30, 0x38, 0x3c, 0x40, 0x40, 0x40, 0x20, 0x20, 0x30, 0x30, 0x60, 0x30, 0x30, 0x21, 0x21, 0x30, 0x38, 0x30 };

unsigned int scroll = 0;
unsigned char offset = 0;

long playerYPos = 32;
unsigned long playerXPos = 10;

unsigned long timer = 0;
unsigned long logicTimer = 0;

char velocityY = 0;
char velocityX = 0;
bool isStandingOnFloor = true;

char upperCollisionBound = 0;
char lowerCollsionBound = 60;
char upperKillBound = -1;
char lowerKillBound = 64;

bool data[8];

int mapLoop = sizeof(mapData);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup() {
  if (ENABLE_TIMINGS) {
    Serial.begin(115200);
  }

  pinMode(PIN_LEFT, INPUT);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_JUMP, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  if (ENABLE_TIMINGS) {
    timer = millis();
  }
}

void loop() {
  logic();
  draw();
  if (ENABLE_TIMINGS) {
    calcTimer();
  }
}

// region LOGIC
void logic() {
  if (logicTimer < millis()) {
    // COLISION
    int column = ((playerXPos + offset) / SPRITE_DIMEN + scroll) % sizeof(mapData);
    fromByte(mapData[column], data);
    for (int j = 0; j < 6; j++) {
      if (data[j]) {} else {
        if(j == 0){
          upperCollisionBound = -100;
        }
        if (data[j - 1]) {
          upperCollisionBound = j*SPRITE_DIMEN+2*PLAYER_RADIUS;
        }
        if (j == 5 || data[j + 1]) {
          lowerCollsionBound = (j+1)*SPRITE_DIMEN;
        }
      }
    }

    if (playerYPos > lowerCollsionBound) {
      playerYPos = lowerCollsionBound;
      isStandingOnFloor = true;
      velocityY = 0;      
    } else {
      isStandingOnFloor = false;
    }

    if(playerYPos < upperCollisionBound){
      playerYPos = upperCollisionBound;
      velocityY = 0;
    }

    // TODO add side collision, add kill zones
    // END COLLISION
    if (!isStandingOnFloor){}
      velocityY = max(velocityY + GRAVITY_VELO, MIN_VELO);
    }

    if (digitalRead(PIN_JUMP) && isStandingOnFloor) {
      velocityY = JUMP_VELO;
      isStandingOnFloor = false;
    }
    if (digitalRead(PIN_LEFT)) {
      velocityX = -1;
    } else if (digitalRead(PIN_RIGHT)) {
      velocityX = 1;
    } else {
      velocityX = 0;
    }

    playerXPos += velocityX;
    playerYPos = min(playerYPos - velocityY, 64);
    logicTimer += LOGIC_DEBOUNCE;
  }

  if (playerXPos > SCROLL_POINT) {
    offset += playerXPos - SCROLL_POINT;
    playerXPos = SCROLL_POINT;
    scroll += offset / SPRITE_DIMEN;
    offset %= SPRITE_DIMEN;
  }
}

// endregion

// region DISPLAY
void draw() {
  display.clearDisplay();
  drawMap();
  drawPlayer();
  display.display();
}

void drawMap() {
  for (int i = 0; i < 14; i++) {
    int addr = (i + scroll) % mapLoop;
    int xPos = i * SPRITE_DIMEN - offset;

    fromByte(mapData[addr], data);

    for (int j = 0; j < 6; j++) {
      if (data[j]) {
        drawBlock(xPos, H_OFFSET + j * SPRITE_DIMEN);
      } else {
        if ((j == 0 || data[j - 1]) && data[7]) {
          drawTriangleDown(xPos, H_OFFSET + j * SPRITE_DIMEN);
        }
        if ((j == 5 || data[j + 1]) && data[6]) {
          drawTriangleUp(xPos, H_OFFSET + j * SPRITE_DIMEN);
        }
      }
    }
  }
}

void drawBlock(int xPos, int yPos) {
  display.drawRect(xPos, yPos, SPRITE_DIMEN, SPRITE_DIMEN, SSD1306_WHITE);
}

void drawTriangleDown(int xPos, int yPos) {
  display.fillTriangle(xPos, yPos, xPos + SPRITE_DIMEN - 1, yPos, xPos + SPRITE_DIMEN / 2, yPos + SPIKE_HEIGHT - 1, SSD1306_WHITE);
}

void drawTriangleUp(int xPos, int yPosU) {
  int yPos = yPosU + SPRITE_DIMEN - 1;
  display.fillTriangle(xPos, yPos, xPos + SPRITE_DIMEN - 1, yPos, xPos + SPRITE_DIMEN / 2, yPos - SPIKE_HEIGHT + 1, SSD1306_WHITE);
}

void drawPlayer() {
  display.drawCircle(playerXPos, playerYPos - PLAYER_RADIUS, PLAYER_RADIUS, SSD1306_WHITE);
}
// endregion

void fromByte(unsigned char c, bool b[8]) {
  for (int i = 0; i < 8; ++i)
    b[i] = (c & (1 << i)) != 0;
}

void calcTimer() {
  unsigned long oldTimer = timer;
  timer = millis();

  unsigned long frameMs = timer - oldTimer;
  float frame = (float)1000 / frameMs;

  Serial.print(frameMs);
  Serial.print(" ms, ");
  Serial.print(frame);
  Serial.println(" fps");
}
