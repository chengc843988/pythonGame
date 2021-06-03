#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "time.h"
#include "AsyncTCP.h"
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFiMulti.h>
#include <LiquidCrystal_I2C.h>

// ==================================================================================
#define LED_BUILTIN 2
#define BUTTON_BUILTIN 0
// defined in include file: arduino/libraries/TFT_eSPI//User_Setup.h
// #define TFT_MISO 19
// #define TFT_MOSI 23
// #define TFT_SCLK 18
// #define TFT_CS   0// 15  // Chip select control pin
// #define TFT_DC    17// 2  // Data Command control pin
// #define TFT_RST   16  // Reset pin (could connect to RST pin)
// //#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
// #define TFT_BL   5 //32
// #define TOUCH_CS 2// 21     // Chip select pin (T_CS) of touch screen

// #define TFT_WR 0 //22    // Write strobe for modified Raspberry Pi TFT only


// ==================================================================================
LiquidCrystal_I2C lcd(0x3F, 20, 4);

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
AsyncWebServer server(80);
char timeStringBuff[50]; //50 chars should be enough

//------------------------------------------------------------------------------------
char cmd[20];
char inverse;
char cube[7][9];

// ==================================================================================
const char *ap_ssid[] = {"tv2730-OptiPlex-3050", "TT49", "71216457", "BUGABDRIOD", NULL};
const char *ap_pwd[] = {"22611182", "22927399", "22611182", "12345678", NULL};
const uint8_t ap_idx = 0;

tm timeInfo;
const char *NTP_SERVER[]={ "pool.ntp.org","asia.pool.ntp.org","tim.google.com","216.239.35.4"};
const char* ntpServer=NTP_SERVER[3];
const long gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;
unsigned long lstNTPtime;
unsigned long lastEntryTime;

// ===================================================================================

WiFiMulti wifiMulti;
void initWifi() {
  Serial.println("Connecting to WiFi..");
  if (0 == 1) {
    WiFi.begin(ap_ssid[ap_idx], ap_pwd[ap_idx]);
    while (WiFi.status() != WL_CONNECTED) {
      delay(3000);
    }

  } else {
    for (int i = 0; ap_ssid[i]; i++) {
      wifiMulti.addAP(ap_ssid[i], ap_pwd[i]);
    }
    while (wifiMulti.run() != WL_CONNECTED) {
      delay(3000);
    }
  }
  Serial.println("");
  Serial.println("WiFi connected, ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);

}

// ------------------------------------------------------------------------------

void printLocalTime()
{
  static int errorTry = 0;
  struct tm timeinfo;
  timeStringBuff[0] = 0;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    if (errorTry++ > 3) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      errorTry = 0;
    }
    return;
  }
  errorTry = 0;

  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  Serial.println(timeStringBuff);
}

// =============================================================================

// DDDUUUUUURRLRRLRRLFFFFFFFFFDDDDDDUUURLLRLLRLLBBBBBBBBB
// UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB
const char CUBE_LAYOUT[] = "ULFRBD";
const char CUBE_COLOR[] =  "WOGRBY?";
// const uint16_t COLOR_VALUE[] = {TFT_WHITE, TFT_ORANGE, TFT_GREEN , TFT_RED , TFT_BLUE, TFT_YELLOW , TFT_MAROON};
const uint16_t COLOR_VALUE[] = {TFT_WHITE, tft.color565(255,127,0), TFT_GREEN , TFT_RED , TFT_BLUE, TFT_YELLOW , TFT_MAROON};


void initCube() {
  for (int j = 0; j < 6; j++)
    for (int i = 0; i < 9; i++) cube[j][i] = CUBE_COLOR[j];
  cube[6][0] = 0;
}

uint16_t colorToNeon(char color) {
  int i;
  for (i = 0; i < 6; i++) {
    if (CUBE_COLOR[i] == color) return COLOR_VALUE[i];
  }
  for (i = 0; i < 6; i++) {
    if (CUBE_LAYOUT[i] == color) return COLOR_VALUE[i];
  }
  return COLOR_VALUE[6];
}
char layoutCodeToColor(char layoutCode) {
  int i;
  // return layoutCode;
  for (i = 0; i < 6; i++) {
    if (CUBE_LAYOUT[i] == layoutCode) break;
  }
  //Serial.printf("(%c,%d,%c)",colorToNeon,i,CUBE_COLOR[i]);

  return CUBE_COLOR[i];
}


void mapToCube(const char *p) {
  //"UUUUUUUUUR...F...D...L...B..."

  //RRRUUUUUURRDRRDRRDFFFFFFFFFDDDDDDLLLULLULLULLBBBBBBBBB
  // UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB
  //  for(int i=0;i<5;i++) {
  //    cube[i][2]=layoutCodeToColor(*p++);
  //    cube[i][5]=layoutCodeToColor(*p++);
  //    cube[i][8]=layoutCodeToColor(*p++);
  //    cube[i][1]=layoutCodeToColor(*p++);
  //    cube[i][4]=layoutCodeToColor(*p++);
  //    cube[i][7]=layoutCodeToColor(*p++);
  //    cube[i][0]=layoutCodeToColor(*p++);
  //    cube[i][3]=layoutCodeToColor(*p++);
  //    cube[i][6]=layoutCodeToColor(*p++);
  //  }

  int idx[] = {0, 3, 2, 5, 1, 4};
  for (int j = 0; j < 6; j++) {
    cube[idx[j]][0] = layoutCodeToColor(*p++);
    cube[idx[j]][1] = layoutCodeToColor(*p++);
    cube[idx[j]][2] = layoutCodeToColor(*p++);
    cube[idx[j]][3] = layoutCodeToColor(*p++);
    cube[idx[j]][4] = layoutCodeToColor(*p++);
    cube[idx[j]][5] = layoutCodeToColor(*p++);
    cube[idx[j]][6] = layoutCodeToColor(*p++);
    cube[idx[j]][7] = layoutCodeToColor(*p++);
    cube[idx[j]][8] = layoutCodeToColor(*p++);
  }

}

void mapToCube_xxx(const char *p) {
  //"UUUUUUUUUR...F...D...L...B..."

  for (int i = 0; i < 9; i++) cube[0][i] = layoutCodeToColor(p[0 + i]);
  for (int i = 0; i < 9; i++) cube[3][i] = layoutCodeToColor(p[9 + i]);
  for (int i = 0; i < 9; i++) cube[2][i] = layoutCodeToColor(p[18 + i]);
  for (int i = 0; i < 9; i++) cube[5][i] = layoutCodeToColor(p[27 + i]);
  for (int i = 0; i < 9; i++) cube[1][i] = layoutCodeToColor(p[36 + i]);
  for (int i = 0; i < 9; i++) cube[4][i] = layoutCodeToColor(p[45 + i]);

  //  for (int i = 0; i < 9; i++) cube[0][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[1][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[2][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[3][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[4][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[5][i] = *p++;

}
// ------------------------------------------------------------------------------

void printCubeEsp32() {
  Serial.println("    -----");
  Serial.print("    |"); for (int i = 0; i < 3; i++) Serial.print(cube[0][i]); Serial.println('|');
  Serial.print("    |"); for (int i = 3; i < 6; i++) Serial.print(cube[0][i]); Serial.println('|');
  Serial.print("    |"); for (int i = 6; i < 9; i++) Serial.print(cube[0][i]); Serial.println('|');
  Serial.println("-----------------");
  for (int j = 1; j < 5; j++) {
    Serial.print('|');
    for (int i = 0; i < 3; i++) Serial.print(cube[j][i]);
  } Serial.println('|');
  for (int j = 1; j < 5; j++) {
    Serial.print('|');
    for (int i = 3; i < 6; i++) Serial.print(cube[j][i]);
  } Serial.println('|');
  for (int j = 1; j < 5; j++) {
    Serial.print('|');
    for (int i = 6; i < 9; i++) Serial.print(cube[j][i]);
  } Serial.println('|');
  Serial.println("-----------------");
  Serial.print("    |"); for (int i = 0; i < 3; i++) Serial.print(cube[5][i]); Serial.println('|');
  Serial.print("    |"); for (int i = 3; i < 6; i++) Serial.print(cube[5][i]); Serial.println('|');
  Serial.print("    |"); for (int i = 6; i < 9; i++) Serial.print(cube[5][i]); Serial.println('|');
  Serial.println("    -----");
}



// ==============================================================================

#define TFT_GREY 0x0004

void initTFT() {

  Serial.println("Start TFT ....");
  Serial.print("TFT_SCLK="); Serial.println(TFT_SCLK);
  Serial.print("MOSI="); Serial.println(TFT_MOSI);
  Serial.print("TFT_RST="); Serial.println(TFT_RST);
  Serial.print("TFT_DC="); Serial.println(TFT_DC);
  Serial.print("TFT_BL="); Serial.println(TFT_BL);
  Serial.print("MISO="); Serial.println(TFT_MISO);
  Serial.print("TFT_CS="); Serial.println(TFT_CS);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init();
  tft.initDMA(true);
  tft.setRotation(2);

  delay(100);
  tft.fillScreen(TFT_GREY);

}

// ==========================================================================================
// web service....
void cubeCommand(AsyncWebServerRequest *request) {
  Serial.print("cubeCommand:"); Serial.println(request->url().c_str());
  cmd[0]=0;inverse='N';
  
  if (request->hasParam("inverse")) {
    AsyncWebParameter* p = request->getParam("inverse");
    inverse = p->value().c_str()[0];
    inverse = (inverse == 'T' || inverse == 't' || inverse == '1' || inverse == 'Y' || inverse == 'y') ? 'Y' : 'N';
  } else {
    inverse = 'N';
  }


  if (request->hasParam("op")) {
    AsyncWebParameter* p = request->getParam("op");
    Serial.print("Comamnd : "); Serial.println(p->value().c_str());
    const char *x = p->value().c_str();
    int i = 0;
    for (i = 0; i < sizeof(cmd) - 1 && *x; i++) {
      cmd[i] = *x++;
      if (inverse == 'Y' && cmd[i] >= 'A' && cmd[i] <= 'Z') cmd[i] += 0x20;
      else if (inverse == 'N' && cmd[i] >= 'a' && cmd[i] <= 'z') cmd[i] -= 0x20;
    }
    cmd[i] = 0;
    cmd[19] = 0;
  }


  if (request->hasParam("cube")) {
    AsyncWebParameter* p = request->getParam("cube");
    Serial.printf("cube: %s\n", p->value().c_str());
    mapToCube(p->value().c_str());
  } else if(cmd[0]!=0) {
    // command incoming...    
  }

  // Serial.printf("cmd: %s, %c, %s\n", cmd, inverse, (char *) cube);
  printCubeEsp32();
  drawCube();
  request->send(200, "text/plain", "OK");

}
char replyBuffer[2048];

void cubeStatus(AsyncWebServerRequest *request) {
  printLocalTime();
  sprintf(replyBuffer, "Hello, GET: %s\n", timeStringBuff);
  request->send(200, "text/plain", replyBuffer);


  if (request->hasParam("cube")) {
    AsyncWebParameter* p = request->getParam("cube");
    mapToCube(p->value().c_str());
  }
  printCubeEsp32();
}


void notFound(AsyncWebServerRequest *request) {
  Serial.print("request not found:"); Serial.println(request->url());
  request->send(404, "text/plain", "Not found");
}

void filesystem(AsyncWebServerRequest *request) {

  Serial.print("Sendfile:"); Serial.println(request->url().c_str());
  //  if (request->url().endsWith(F(".txt"))) {
  int fnsstart = request->url().lastIndexOf('/');
  String fn = request->url().substring(fnsstart);
  Serial.print("Sendfile:"); Serial.println(fn);

  AsyncWebServerResponse *response = request->beginResponse(SPIFFS, fn);
  response->addHeader("Server", "ESP Async Web Server");
  request->send(response);
  // here comes some mambo-jambo to extract the filename from request->url()

  // ... and finally
  // request->send(SPIFFS, fn, String(), true);
  //  } else {
  //    request->send_P(404, PSTR("text/plain"), PSTR("Not found"));
  //  }
}

// ========================================================================================


#define X_OFFSET 10
#define Y_OFFSET 10
#define X_SIZE 18
#define Y_SIZE 18
#define X_FILLER 2
#define Y_FILLER 2
#define X_SCALE (X_SIZE+X_FILLER)
#define Y_SCALE (Y_SIZE+Y_FILLER)

void drawCube() {
  uint16_t c;
  int x, y;
  // tft.fillScreen(TFT_GREY);
  tft.fillRect(x, y, X_SIZE, Y_SIZE, c);
  if (WiFi.status() == WL_CONNECTED) {
    //tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.drawString(WiFi.localIP().toString().c_str(), 20, 30);
    tft.drawString(timeStringBuff, 20, 40);
  } else {
    tft.drawString("WIFI not ready", 20, 40);    
  }
  for (int i = 0; i < 9; i++) {
    c = colorToNeon(cube[0][i]);
    y = tft.height() - (X_OFFSET + (i % 3 + 3) * X_SCALE) - X_SIZE;
    x = Y_OFFSET + (i / 3) * Y_SCALE;
    tft.fillRect(x, y, X_SIZE, Y_SIZE, c);
  }

  for (int j = 1; j < 5; j++) {
    for (int i = 0; i < 9; i++) {
      c = colorToNeon(cube[j][i]);
      y = tft.height() - (X_OFFSET + (i % 3 + (j - 1) * 3) * X_SCALE) - X_SIZE;
      x = Y_OFFSET + (i / 3 + 3) * Y_SCALE;
      tft.fillRect(x, y, X_SIZE, Y_SIZE, c);
    }

  }

  for (int i = 0; i < 9; i++) {
    c = colorToNeon(cube[5][i]);
    y = tft.height() - (X_OFFSET + (i % 3 + 3) * X_SCALE) - X_SIZE;
    x = Y_OFFSET + (i / 3 + 6) * Y_SCALE;
    tft.fillRect(x, y, X_SIZE,  Y_SIZE, c);
  }


  //  for (int i = 0; i < 6; i++) {
  //    for (int j = 0; j < 9; j++) {
  //      c = colorToNeon(cube[i][j]);
  //      x = X_OFFSET + i * X_SCALE;
  //      y = Y_OFFSET + j * Y_SCALE;
  //      tft.fillRect(x, y, x + X_SIZE, y + Y_SIZE, c);
  //      Serial.printf("[%d,%d] color=%c, value=%04x, %d, %d\n",i,j,cube[i][j],c,x,y);
  //    }
  //  }
}



// ==========================================================================================================
char allSteps[256];
int steps = 0;
#define TRUE 1
#define FALSE 0

char *TERM_COLOR_RESET = "\x1B[0m";

char *TERM_COLOR[] = {
    "\x1B[30m",
    "\x1B[31m",
    "\x1B[32m",
    "\x1B[33m",
    "\x1B[34m",
    "\x1B[35m",
    "\x1B[36m",
    "\x1B[37m",
    "\x1B[90m",
    "\x1B[91m",
    "\x1B[92m",
    "\x1B[93m",
    "\x1B[94m",
    "\x1B[95m",
    "\x1B[96m",
    "\x1B[97m"};

char *TERM_COLOR_BG[] = {
    "\033[3;42;30m",
    "\033[3;43;30m",
    "\033[3;44;30m",
    "\033[3;104;30m",
    "\033[3;100;30m",
    "\033[3;47;35m",
    "\033[2;47;35m",
    "\033[1;47;35m"};



#ifdef WIN32 || LINUX || WIN_A64 
#define MODE_VALUE 16
int cubeColor(int pannel, int pos)
{
  return (cube[pannel][pos] / MODE_VALUE);
}
int cubeValue(int pannel, int pos)
{
  return (cube[pannel][pos] % MODE_VALUE);
}

void initCube_windows()
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 9; j++)
      cube[i][j] = i * MODE_VALUE + j;
    cube[i][4] = i * MODE_VALUE + i;
  }
  steps = 0;
  memset(allSteps, 0, sizeof(allSteps));
}
# else 
inline int cubeColor(int pannel, int pos)
{
 return cube[pannel][pos];
}
inline int cubeValue(int pannel, int pos)
{
  return ('?');
}

#endif
// ------------------------------------------------------------------------

void printColor(int pannel, int pos)
{
  int value = cubeValue(pannel, pos);
  char *color = TERM_COLOR_BG[cubeColor(pannel, pos)];
  Serial.printf("%s%d%s", color, value, TERM_COLOR_RESET);
}



int termPrintCube()
{
  puts(allSteps);

  int i = 0;
  printf("      -------\n");

  printf("      ");
  putchar('|'), printColor(0, 0), putchar('.'), printColor(0, 1), putchar('.'), printColor(0, 2), putchar('|');
  putchar('\n');
  printf("      ");
  putchar('|'), printColor(0, 3), putchar('.'), printColor(0, 4), putchar('.'), printColor(0, 5), putchar('|');
  putchar('\n');
  printf("      ");
  putchar('|'), printColor(0, 6), putchar('.'), printColor(0, 7), putchar('.'), printColor(0, 8), putchar('|');
  putchar('\n');

  printf("-------------------------\n");

  putchar('|'), printColor(1, 0), putchar('.'), printColor(1, 1), putchar('.'), printColor(1, 2);
  putchar('|'), printColor(2, 0), putchar('.'), printColor(2, 1), putchar('.'), printColor(2, 2);
  putchar('|'), printColor(3, 0), putchar('.'), printColor(3, 1), putchar('.'), printColor(3, 2);
  putchar('|'), printColor(4, 0), putchar('.'), printColor(4, 1), putchar('.'), printColor(4, 2);
  putchar('|'), putchar('\n');

  putchar('|'), printColor(1, 3), putchar('.'), printColor(1, 4), putchar('.'), printColor(1, 5);
  putchar('|'), printColor(2, 3), putchar('.'), printColor(2, 4), putchar('.'), printColor(2, 5);
  putchar('|'), printColor(3, 3), putchar('.'), printColor(3, 4), putchar('.'), printColor(3, 5);
  putchar('|'), printColor(4, 3), putchar('.'), printColor(4, 4), putchar('.'), printColor(4, 5);
  putchar('|'), putchar('\n');

  putchar('|'), printColor(1, 6), putchar('.'), printColor(1, 7), putchar('.'), printColor(1, 8);
  putchar('|'), printColor(2, 6), putchar('.'), printColor(2, 7), putchar('.'), printColor(2, 8);
  putchar('|'), printColor(3, 6), putchar('.'), printColor(3, 7), putchar('.'), printColor(3, 8);
  putchar('|'), printColor(4, 6), putchar('.'), printColor(4, 7), putchar('.'), printColor(4, 8);
  putchar('|'), putchar('\n');

  printf("-------------------------\n");
  printf("      ");
  putchar('|'), printColor(5, 0), putchar('.'), printColor(5, 1), putchar('.'), printColor(5, 2), putchar('|');
  putchar('\n');
  printf("      ");
  putchar('|'), printColor(5, 3), putchar('.'), printColor(5, 4), putchar('.'), printColor(5, 5), putchar('|');
  putchar('\n');
  printf("      ");
  putchar('|'), printColor(5, 6), putchar('.'), printColor(5, 7), putchar('.'), printColor(5, 8), putchar('|');
  putchar('\n');
  printf("      -------\n");
}


int simplePrintCube_X()
{
  printf("      |%02x.%02x.%02x|\n", cube[0][0] & 0x0f, cube[0][1], cube[0][2]);
  printf("      |%02x.%02x.%02x|\n", cube[0][3], cube[0][4], cube[0][5]);
  printf("      |%02x.%02x.%02x|\n", cube[0][6], cube[0][7], cube[0][8]);
  printf("-------------------------\n");
  printf("|%02x.%02x.%02x|%02x.%02x.%02x|%02x.%02x.%02x|%02x.%02x.%02x|\n", cube[1][0], cube[1][1], cube[1][2], cube[2][0], cube[2][1], cube[2][2], cube[3][0], cube[3][1], cube[3][2], cube[4][0], cube[4][1], cube[4][2]);
  printf("|%02x.%02x.%02x|%02x.%02x.%02x|%02x.%02x.%02x|%02x.%02x.%02x|\n", cube[1][3], cube[1][4], cube[1][5], cube[2][3], cube[2][4], cube[2][5], cube[3][3], cube[3][4], cube[3][5], cube[4][3], cube[4][4], cube[4][5]);
  printf("|%02x.%02x.%02x|%02x.%02x.%02x|%02x.%02x.%02x|%02x.%02x.%02x|\n", cube[1][6], cube[1][7], cube[1][8], cube[2][6], cube[2][7], cube[2][8], cube[3][6], cube[3][7], cube[3][8], cube[4][6], cube[4][7], cube[4][8]);
  printf("-------------------------\n");
  printf("      |%02x.%02x.%02x|\n", cube[5][0], cube[5][1], cube[5][2]);
  printf("      |%02x.%02x.%02x|\n", cube[5][3], cube[5][4], cube[5][5]);
  printf("      |%02x.%02x.%02x|\n", cube[5][6], cube[5][7], cube[5][8]);
  printf("      -------\n\n");
}

// -------------------------------------------------------------------------------------------

void pannelClockWize(int p)
{
  unsigned c;
  c = cube[p][0];
  cube[p][0] = cube[p][6];
  cube[p][6] = cube[p][8];
  cube[p][8] = cube[p][2];
  cube[p][2] = c;

  c = cube[p][1];
  cube[p][1] = cube[p][3];
  cube[p][3] = cube[p][7];
  cube[p][7] = cube[p][5];
  cube[p][5] = c;
}

// -------------------------------------------------------------------------------------------

void pannelUnClockWize(int p)
{
  unsigned c;
  c = cube[p][0];
  cube[p][0] = cube[p][2];
  cube[p][2] = cube[p][8];
  cube[p][8] = cube[p][6];
  cube[p][6] = c;

  c = cube[p][1];
  cube[p][1] = cube[p][5];
  cube[p][5] = cube[p][7];
  cube[p][7] = cube[p][3];
  cube[p][3] = c;
}

// -------------------------------------------------------------------------------------------

void rotateFront()
{
  unsigned c;
  c = cube[0][6];
  cube[0][6] = cube[1][8];
  cube[1][8] = cube[5][2];
  cube[5][2] = cube[3][0];
  cube[3][0] = c;

  c = cube[0][7];
  cube[0][7] = cube[1][5];
  cube[1][5] = cube[5][1];
  cube[5][1] = cube[3][3];
  cube[3][3] = c;

  c = cube[0][8];
  cube[0][8] = cube[1][2];
  cube[1][2] = cube[5][0];
  cube[5][0] = cube[3][6];
  cube[3][6] = c;

  pannelClockWize(2);
}

// -------------------------------------------------------------------------------------------

void rotateRight()
{
  unsigned c;
  c = cube[2][2];
  cube[2][2] = cube[5][2];
  cube[5][2] = cube[4][6];
  cube[4][6] = cube[0][2];
  cube[0][2] = c;

  c = cube[2][5];
  cube[2][5] = cube[5][5];
  cube[5][5] = cube[4][3];
  cube[4][3] = cube[0][5];
  cube[0][5] = c;

  c = cube[2][8];
  cube[2][8] = cube[5][8];
  cube[5][8] = cube[4][0];
  cube[4][0] = cube[0][8];
  cube[0][8] = c;
  pannelClockWize(3);
}

// -------------------------------------------------------------------------------------------
void rotateUpper()
{
  unsigned c;
  c = cube[1][0];
  cube[1][0] = cube[2][0];
  cube[2][0] = cube[3][0];
  cube[3][0] = cube[4][0];
  cube[4][0] = c;

  c = cube[1][1];
  cube[1][1] = cube[2][1];
  cube[2][1] = cube[3][1];
  cube[3][1] = cube[4][1];
  cube[4][1] = c;

  c = cube[1][2];
  cube[1][2] = cube[2][2];
  cube[2][2] = cube[3][2];
  cube[3][2] = cube[4][2];
  cube[4][2] = c;
  pannelClockWize(0);
}

// -------------------------------------------------------------------------------------------
void zClockWize()
{
  pannelClockWize(0);
  pannelUnClockWize(5);
  unsigned c;
  for (int j = 0; j < 9; j++)
  {
    c = cube[1][j];
    cube[1][j] = cube[2][j];
    cube[2][j] = cube[3][j];
    cube[3][j] = cube[4][j];
    cube[4][j] = c;
  }
}

// -------------------------------------------------------------------------------------------
void yClockWize()
{
  pannelClockWize(1);
  pannelUnClockWize(3);
  unsigned c;
  for (int j = 0; j < 9; j++)
  {
    c = cube[0][j];
    cube[0][j] = cube[2][j];
    cube[2][j] = cube[5][j];
    cube[5][j] = cube[4][8 - j];
    cube[4][8 - j] = c;
  }
}

// -------------------------------------------------------------------------------------------
void xClockWize()
{
  pannelClockWize(2);
  pannelUnClockWize(4);
  unsigned c;

  c = cube[0][0], cube[0][0] = cube[1][6], cube[1][6] = cube[5][8], cube[5][8] = cube[3][2], cube[3][2] = c;
  c = cube[0][1], cube[0][1] = cube[1][3], cube[1][3] = cube[5][7], cube[5][7] = cube[3][5], cube[3][5] = c;
  c = cube[0][2], cube[0][2] = cube[1][0], cube[1][0] = cube[5][6], cube[5][6] = cube[3][8], cube[3][8] = c;
  c = cube[0][3], cube[0][3] = cube[1][7], cube[1][7] = cube[5][5], cube[5][5] = cube[3][1], cube[3][1] = c;
  c = cube[0][4], cube[0][4] = cube[1][4], cube[1][4] = cube[5][4], cube[5][4] = cube[3][4], cube[3][4] = c;
  c = cube[0][5], cube[0][5] = cube[1][1], cube[1][1] = cube[5][3], cube[5][3] = cube[3][7], cube[3][7] = c;
  c = cube[0][6], cube[0][6] = cube[1][8], cube[1][8] = cube[5][2], cube[5][2] = cube[3][0], cube[3][0] = c;
  c = cube[0][7], cube[0][7] = cube[1][5], cube[1][5] = cube[5][1], cube[5][1] = cube[3][3], cube[3][3] = c;
  c = cube[0][8], cube[0][8] = cube[1][2], cube[1][2] = cube[5][0], cube[5][0] = cube[3][6], cube[3][6] = c;
}

// =============================================================================================
int adjustCenterPos()
{
  int p;
  for (p = 0; p < 6; p++)
    if (cubeColor(p, 4) == 0)
      break;
  switch (p)
  {
  case 3:
    xClockWize();
    xClockWize();
    xClockWize();
    break;
  case 4:
    yClockWize();
    yClockWize();
    yClockWize();
    break;
  case 1:
    xClockWize();
    break;
  case 2:
    yClockWize();
    break;
  case 5:
    xClockWize();
    xClockWize();
  }
  while (cubeColor(1, 4) != 1)
  {
    zClockWize();
  }
}
// =============================================================================================

int processCMD(char *commands)
{
  char *p = commands;
  char cmd;
  int isCmd;
  while ((cmd = *p++) != 0)
  {
    isCmd = TRUE;
    allSteps[steps++] = cmd;
    if (steps >= sizeof(allSteps))
      exit(0);
    switch (cmd)
    {
    case 'u':
      rotateUpper(), rotateUpper(), rotateUpper();
      break;
    case 'U':
      rotateUpper();
      break;
    case 'f':
      rotateFront(), rotateFront(), rotateFront();
      break;
    case 'F':
      rotateFront();
      break;
    case 'r':
      rotateRight(), rotateRight(), rotateRight();
      break;
    case 'R':
      rotateRight();
      break;
    case 'l':
      zClockWize();
      rotateFront();
      rotateFront();
      rotateFront();
      zClockWize();
      zClockWize();
      zClockWize();
      break;
    case 'L':
      zClockWize();
      rotateFront();
      zClockWize();
      zClockWize();
      zClockWize();
      break;
    case 'x':
      xClockWize(), xClockWize(), xClockWize();
      break;
    case 'X':
      xClockWize();
      break;
    case 'y':
      yClockWize(), yClockWize(), yClockWize();
      break;
    case 'Y':
      yClockWize();
      break;
    case 'z':
      zClockWize(), zClockWize(), zClockWize();
      break;
    case 'Z':
      zClockWize();
      break;
    case 'c':
    case 'C':
      adjustCenterPos();
      break;
    case ',':
    case ';': // take a break only
      break;
    default:
      isCmd = FALSE;
      steps--;
    }
    allSteps[steps] = 0;
    char filler[60];
    if (!isCmd)
    {
      switch (cmd)
      {
      case '=':
      case '-':
      case '^':
      case 'v':
      case '#':
      case '>':
        memset(filler, cmd, sizeof(filler));
        filler[sizeof(filler) - 1] = 0;
        puts(filler);
        break;
      case 'p':
      case 'P':
        termPrintCube();
        break;
      case 'i':
      case 'I':
        initCube();
        break;
      case '~':
      case '*':
        allSteps[steps = 0] = 0;
        break;
      default:
        break;
      }
    }
  }
  if (steps)
    allSteps[steps++] = '.';
  allSteps[steps] = 0;
}
// =============================================================================================

int isFinished()
{
  unsigned color;
  for (int i = 0; i < 6; i++)
  {
    color = cubeColor(i, 4);
    for (int j = 0; j < 9; j++)
    {
      if (cubeColor(i, j) != color)
        return FALSE;
    }
  }
  return TRUE;
}

int isFirstLayerFinished()
{
  unsigned color;
  color = cubeColor(5, 4);
  for (int j = 0; j < 9; j++)
  {
    if (cubeColor(5, j) != color)
      return FALSE;
  }
  for (int i = 1; i < 5; i++)
  {
    color = cubeColor(i, 4);
    if (cubeColor(i, 6) != color || cubeColor(i, 7) != color || cubeColor(i, 8) != color)
      return FALSE;
  }
  return TRUE;
}

int isSecondLayerFinished()
{
  unsigned color;
  for (int i = 1; i < 5; i++)
  {
    color = cubeColor(i, 4);
    if (cubeColor(i, 3) != color || cubeColor(i, 4) != color || cubeColor(i, 5) != color)
      return FALSE;
  }
  return TRUE;
}

int isThirdLayerFinished()
{
  unsigned color;
  color = cubeColor(0, 4);
  for (int j = 0; j < 9; j++)
  {
    if (cubeColor(0, j) != color)
      return FALSE;
  }
  for (int i = 1; i < 5; i++)
  {
    color = cubeColor(i, 4);
    if (cubeColor(i, 0) != color || cubeColor(i, 1) != color || cubeColor(i, 2) != color)
      return FALSE;
  }
  return TRUE;
}

int isTopCrossFinished()
{
  int result = TRUE;
  for (int i = 0; i < 4; i++)
  {
    result &= cubeColor(0, 7) == cubeColor(0, 4);
    result &= cubeColor(2, 1) == cubeColor(2, 4);
  }
  return result;
}
int isTopCorrnorFinished()
{
  int result = TRUE;
  for (int p = 0; p < 4; p++)
  {
    result &= (cubeColor(0, 6) == cubeColor(0, 4));
    result &= (cubeColor(1, 2) == cubeColor(1, 4));
    result &= (cubeColor(2, 0) == cubeColor(2, 4));
    zClockWize();
  }
  return result;
}
// ==========================================================================================
//調整中6個心點
int STEP0_center()
{
  processCMD("Cp");
}

// ------------------------------------------------------------------------------------
// 調整底十字
int STEP1_bottomCross()
{

  int p;
  int topColor = cubeColor(5, 4); // 頂層(底層)色碼
  //  processCMD("YYP"); // move to bottom!!

  int done;
  for (int i = 0; i < 4; i++) // 必需判段四個中心
  {
    processCMD("ZP"); // Z軸轉一次, 顯示(print)
    done = (cubeColor(0, 5) == topColor); // 如果顏色與底層相同....完成一次
    if (done)
      continue; // ....完成一次

    // 如果在右方中上有一個, 因為上層右上方不正確, 所以透過... R, 調到中間層, 然後透過中間層解法調整
    if (cubeColor(3, 1) == topColor) 
    {
      processCMD("Rp"); //move to layer3, then ignal it
    }
    else if (cubeColor(3, 7) == topColor) // 如果右側底層有一個, 一樣先轉到中間層, 再交給後中間層的來處理
    {
      processCMD("Rp"); //move to layer3, then ignal it
    }

    // 找一下前端是否有目標在...
    for (int i = 0; i < 4; i++)
    {
      if (cubeColor(2, 5) == topColor) // 在FRONT㕅的右側有一個, 所以轉上去即可(上右即完成)
      {
        processCMD("R;p");
        done = TRUE;
        break;
      }
      else if (cubeColor(4, 3) == topColor) // 後端有一個.....轉上來即可
      {
        processCMD("r;p");
        done = TRUE;
        break;
      }
      else if (cubeColor(3, 7) == topColor) // 如果在右側下方有一個, 所以....
      {
        processCMD("rZuRp");
        done = TRUE;
        break;
      }
      // 因為找到並調整後, 會被BREAD掉, 所以到這裏是沒找到符合規則的
      processCMD("Uzp"); // 這次在前/右/後側, 還是沒找到有底層顏色...轉一下方位進行下一面測試
    }
    if (done) // 如果有找到, 表示側面任何一邊有個底層顏色的被完成, 所以頂層再進行下一個調整
      continue;

    // 以上完成側邊檢查
  
    // 這段是把底層調到上面來, 好讓十字可以産生......
    for (int i = 0; i < 4; i++)
    {
      processCMD("Uz");
      if (cubeColor(5, 5) == topColor) 
      {
        processCMD("RR;P");
        done = TRUE;
        break;
      }
    }
  }
// 十字完成了


  // 把十字一個一個調下來....
  for (int i = 0; i < 4; i++)
  {
    processCMD("U");
    if (cubeColor(0, 5) != topColor)
      continue;
    for (int j = 0; j < 4; j++)
    {
      processCMD("Uz"); // 下一個.... Uz ==> 頂層, 全部 ==> 相當是底層轉一下
      if (cubeColor(3, 1) == cubeColor(3, 4)) //
      {
        processCMD("RR;P"); // 轉下去....
        break;
      }
    }
  }
  // 再把CUBE 中心點調整好.....
  processCMD("CP");
}

// ---------------------------------------------------
// 工具...排序....因為要確認頂角相同, 必需比較3個色....先排序比較好比
void sort(unsigned *c)
{
  unsigned x;
  // printf("sort: %d%d%d",c[0],c[1],c[2]);
  if (c[0] > c[1])
    x = c[0], c[0] = c[1], c[1] = x;
  if (c[1] > c[2])
    x = c[1], c[1] = c[2], c[2] = x;
  if (c[0] > c[1])
    x = c[0], c[0] = c[1], c[1] = x;
  // printf(" ==> %d%d%d\n",c[0],c[1],c[2]);
}
// ---------------------------------------------------------
// 是否同一個...
int equals(unsigned *x1, unsigned *x2)
{
  // printf("comp: %d%d%d", x1[0], x1[1], x1[2]);
  // printf(" ==> %d%d%d\n", x2[0], x2[1], x2[2]);
  return (x1[0] == x2[0] && x1[1] == x2[1] && x1[2] == x2[2]);
}


// --------------------------------------------------------------
// 右上前 方那顆是否在正確位置....
// 三方中心點的顏色, 跟這顆比...
// ---------------------------------------------------------------
int rightUpperFrontFinished()
{
  return (cubeColor(0, 8) == cubeColor(0, 4) &&
          cubeColor(2, 2) == cubeColor(2, 1) &&
          cubeColor(3, 0) == cubeColor(3, 1));
}
// ----------------------------------------------------------------
int STEP2_bottomCorrnor()
{
  unsigned c2[3], c0[3], c1[3];
  processCMD("YY"); // 十字完成後, 把底層轉上來....
  int finished = 0;
  int topColor = cubeColor(0, 4);

  // 直到完成4顆
  // 當有一顆完成了, 直接歸零重新計算, 可以降低程式複雜度, 相當於這四個角處理重新啟動
  while (finished < 4)
  {
    processCMD("Up");
    if (rightUpperFrontFinished()) // 如果位置對了
    {
      processCMD("v"); // 顯示分隔符號....無作用
      finished++; // 完成度+1, 當完成4個時, 會離開迴圈...
      continue;
    }

    // 右上頂不正確 , 把這顆頻色留一來, 等一下要判斷是在那個方位...

    c0[0] = cubeColor(0, 4); // 頂中心
    c0[1] = cubeColor(2, 1); // 前中上
    c0[2] = cubeColor(3, 1); // 右中上
    sort(c0); // 排序, 便等一下比對

    int p;
    for (p = 0; p < 4; p++) // 四個底角找找看有沒相同條件的....
    {
      c2[0] = (cubeColor(2, 8)); // 前右下
      c2[1] = (cubeColor(3, 6));
      c2[2] = (cubeColor(5, 2));
      sort(c2); // 排序, 便等一下比對
      if (equals(c0, c2)) // 相同....直接跳開
      {
        break;
      }
      processCMD("Zu"); // 底端(二層) 轉一下試下一個, 因為還沒有要處理中間層, 所以可以這樣做..
    }

    // 顏色有三面, 所以要判斷個可能性
    if (cubeColor(2, 8) == topColor) // 如果前側相同....公式轉...
    {
      processCMD("FxxUxxf;p");
      finished = 0;
      processCMD("u");
      continue;
    }
    if (cubeColor(3, 6) == topColor) // 如果右側相同....公式轉
    {
      processCMD("rxxuxxR;p");
      finished = 0;
      processCMD("u");
      continue;
    }
    if (cubeColor(5, 2) == topColor) // 如果在底層....
    {
      processCMD("rxxUxxRxxUxx");
      processCMD("rxxuxxR;p");
      processCMD("u");
      continue;
    }
    // 看來在底層都找不到了......且這個位置有一顆但方向錯誤.....就轉到中間就好....不用解它...
    // 因為未完成, 所以不會被+1計算, 所以當這個轉到中間後, WHILE會再處理到這個面...
    if ((c1[0] = cubeColor(0, 8)) == topColor ||
        (c1[1] = cubeColor(2, 2)) == topColor ||
        (c1[2] = cubeColor(3, 0)) == topColor)
    {
      processCMD("rxxUxxRup");
      processCMD("u");
      sort(c1);
    }
  }

  // 校正中心點.....
  processCMD("XX");
  while (cubeColor(5, 1) == cubeColor(2, 7))
    processCMD("U");
  while (cubeColor(1, 1) != 1)
    processCMD("Z");
  processCMD("P");
}

// ---------------------------------------------------------------------------------------
int STEP3_layer2()
{
  unsigned c0[20], c1[20], isFinish = 0;

  // 需要完成四面....
  while (isFinish < 4)
  {
    processCMD("ZP"); // 轉個面....
    c0[0] = cubeColor(2, 4); // 保留 前右顏色.....
    c0[1] = cubeColor(3, 4);
    if (c0[0] == (cubeColor(2, 5)) && // 如果這顆相同....
        c0[1] == (cubeColor(3, 3)))
    {
      // printf("finish: %d,%d,%x,%x\n", c0[0], c0[1], cubeColor(2,5], cubeColor(3,3]);
      isFinish++;
      continue;
    }

    // 這顆不對.....尋找在何處....
    int p;
    for (p = 0; p < 4; p++)
    {
      // 如果是公式位置....套公式
      if ((c1[0] = cubeColor(2, 1)) == c0[0] && (c1[1] = cubeColor(0, 7)) == c0[1])
      {
        //printf("fsame: %d,%d,%d,%d\n", c0[0], c0[1], c1[0], c1[1]);
        processCMD("URurufUFp"); // 54212356
        isFinish = 0;
        break;
      }
      else if ((c1[0] = cubeColor(3, 1)) == c0[1] && (c1[1] = cubeColor(0, 5)) == c0[0])
      { // 如果是公式位置....套公式
        //printf("rsame: %d,%d,%d,%d\n", c0[0], c0[1], c1[0], c1[1]);
        processCMD("ufUFURurp"); // 23565421
        isFinish = 0;
        break;
      }
      //printf("compare: %d,%d,%d,%d\n", c0[0], c0[1], c1[0], c1[1]);
      processCMD("U"); // 如果不是....下一個
    }

    if (p == 4) // 為什麼沒找到....原來自己佔用了別人位置....轉到側面去...
    {
      processCMD("URurufUFP"); // 54212356
      isFinish = 0; // 一切重新計算....
    }
  }

  // 回到中心
  while (cubeColor(1, 1) != 1)
    processCMD("Z");
  processCMD("P");
}

// ================================================================================
// 頂十字
// 公式有三個, 但要符會公式圖型...

int STEP4_topCross()
{
  unsigned p, color;
  color = cubeColor(0, 4);

  while (1)
  {
    if (cubeColor(0, 1) == color && cubeColor(0, 3) == color) // 有2個, 但是完成, 還是3個
    {
      if (cubeColor(0, 7) == color && cubeColor(0, 5) == color) // 完成
        break;               //finished
      processCMD("rufUFRP"); //123564 // 只有3個, 轉一次就有公式了...
      continue;
    }
    if (cubeColor(0, 1) == color && cubeColor(0, 7) == color) // 直線....
    {
      processCMD("rufUFRP"); //123564
      continue;
    }
    if (cubeColor(0, 3) == color && cubeColor(0, 5) == color) // 都沒有...頂層轉一下
    {
      processCMD("U");
      continue;
    }
    processCMD("rufUFRP"); //123564
  }
}

// --------------------------------------------------------------------------------
int STEP5_topConner()
{
  unsigned color, isFinish;
  isFinish = 0;
  color = cubeColor(0, 4);
  // printf("Start : STEP5_topConner color=%d\n",color);
  // 找到一個符會條件的....直接套公式轉, 轉成同顏色...
  while (TRUE)
  {
    // type 1
    if (cubeColor(0, 0) == color && cubeColor(0, 2) != color && cubeColor(0, 6) != color && cubeColor(0, 8) != color)
    {
      if (cubeColor(2, 0) == color)
      {
        // puts("TYPE 1");
        processCMD("ruRurUURUp"); // 124215545;
        break;
      }
    }

    // type 2
    if (cubeColor(0, 0) != color && cubeColor(0, 2) != color && cubeColor(0, 6) == color && cubeColor(0, 8) != color)
    {
      if (cubeColor(2, 2) == color)
      {
        // puts("TYPE 2");
        processCMD("RUrURuurup"); //451542212
        break;
      }
    }

    // TYPE c3
    if (cubeColor(0, 0) != color && cubeColor(0, 2) == color && cubeColor(0, 6) != color && cubeColor(0, 8) == color)
    {
      if (cubeColor(2, 0) == color)
      {
        // puts("TYPE 3");
        processCMD("ruRurUURUP"); // 124215545;
        continue;
      }
    }

    // TYPE c4
    if (cubeColor(0, 0) != color && cubeColor(0, 2) != color && cubeColor(0, 6) == color && cubeColor(0, 8) == color)
    {
      if (cubeColor(4, 2) == color)
      {
        // puts("TYPE 4");
        processCMD("ruRurUURUP"); // 124215545;
        continue;
      }
    }

    // TYPE c5
    if (cubeColor(0, 0) != color && cubeColor(0, 2) == color && cubeColor(0, 6) == color && cubeColor(0, 8) != color)
    {
      if (cubeColor(4, 2) == color)
      {
        // puts("TYPE 5");
        processCMD("ruRurUURUP"); // 124215545;
        continue;
      }
    }
    // TYPE c6
    if (cubeColor(0, 0) != color && cubeColor(0, 2) != color && cubeColor(0, 6) != color && cubeColor(0, 8) != color)
    {
      if (cubeColor(1, 0) == color && cubeColor(3, 2) == color && cubeColor(2, 2) == color && cubeColor(3, 0) == color)
      {
        // puts("TYPE 6");
        processCMD("ruRurUURUP"); // 124215545;
        continue;
      }
    }

    // TYPE c7
    if (cubeColor(0, 0) != color && cubeColor(0, 2) != color && cubeColor(0, 6) != color && cubeColor(0, 8) != color)
    {
      // puts("Try to check type 7");
      if (cubeColor(1, 0) == color && cubeColor(1, 2) == color 
      && cubeColor(3, 0) == color && cubeColor(3, 2) == color)
      {
        puts("TYPE 7");
        processCMD("ruRurUURUP"); // 124215545;
        continue;
      }
    }

    processCMD("UP");
  }
}

// =====================================================
// 調整頂角相對位置.....直接套公式

int STEP6_adjustConner()
{
  unsigned color, p, isFinish;
  isFinish = FALSE;
  color = cubeColor(0, 4);
  while (!isFinish)
  {
    // search the same color
    for (p = 0; p < 4; p++)
    {
      if (cubeColor(1, 0) == cubeColor(1, 2))
      {
        if (cubeColor(2, 0) == cubeColor(2, 2) && cubeColor(3, 0) == cubeColor(3, 2))
        {
          isFinish = TRUE;
        }
        break;
      }
    }
    if (isFinish == TRUE)
    {
      break;
    }
    else if (p == 4)
    {
      processCMD("FrFLLfRFLLFFP"); //  61677-34677-66
    }
    else
    {
      processCMD("FrFLLfRFLLFFP"); // 61677-34677-66
    }
  }
}

// ---------------------------------------------------------------------
// 調整頂十字...
int STEP7_adjustCross()
{
  int p;
  while (TRUE)
  {

    for (p = 0; p < 4; p++)
    {
      if (cubeColor(2, 0) == cubeColor(2, 1))
      {
        break;
      }
      processCMD("U");
    }
    if (cubeColor(1, 1) == cubeColor(1, 0))
      break;
    processCMD("ruRurUURU,RUrURuuru,p");
  }

  for (p = 0; p < 4; p++)
  {
    if (cubeColor(1, 4) == 1)
      break;
    processCMD("Z");
  }
  for (p = 0; p < 4; p++)
  {
    if (cubeColor(1, 1) == 1)
      break;
    processCMD("U");
  }
}

// ===============================================================================
// 測試程式1
int main_2(int argc, char const *argv[])
{
  char *testcase = "i*Fpfp";
  processCMD(testcase);
}

// ===============================================================================
// 測試程式2
int main_X(int argc, char const *argv[])
{
  char *exame = "iPxuTxUrufcU*p";
  processCMD(exame);

  STEP0_center();
  STEP1_bottomCross();
  processCMD("*p=");
  STEP2_bottomCorrnor();
  processCMD("*p=");
  STEP3_layer2();
  processCMD("*p=");
  STEP4_topCross();
  processCMD("*p=");
  STEP5_topConner();
  processCMD("*p=");
  STEP6_adjustConner();
  processCMD("*pv");
  STEP7_adjustCross();

  processCMD("vP");
  Serial.println(allSteps);
  // int result=isFirstLayerFinish();
  Serial.printf("isFirstLayerFinish=%d\n", isFirstLayerFinished());
  Serial.printf("isSecondLayerFinish=%d\n", isSecondLayerFinished());
  Serial.printf("isThiLayerFinish=%d\n", isThirdLayerFinished());
  return 0;
}




// ==========================================================================================================

void setup() {

  Serial.begin(115200);
  delay(100);
  initTFT();

  Serial.println("\nStart .....");
  Serial.println("====================================================\n\n");
  initCube();
  printCubeEsp32();

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  initWifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // delay(3000);
  printLocalTime();

  //-------------------------------------------
  //  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
  //    request->send(SPIFFS, "/index.html", "text/html");
  //  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/status", HTTP_GET,  cubeStatus);

  //  server.on("^\\/.*(html|html|css|js|png|svg)$", HTTP_GET, filesystem);
  server.on("/cubeCommand", HTTP_GET, cubeCommand);

  server.onNotFound(notFound);


  server.begin();
}

uint32_t nextSerialLog = 0;
uint32_t nextTFT = 0;
uint32_t nextLocalTime = 0;

void loop() {
  if (nextSerialLog < millis()) {
    nextSerialLog = millis() + 60000;
    printLocalTime();
    printCubeEsp32();
  }
  if (nextTFT < millis()) {
    nextTFT = millis() + 3000;
    drawCube();
  }
  // server.handleClient();
  //delay(10);
}
