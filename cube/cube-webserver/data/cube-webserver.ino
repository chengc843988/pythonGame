#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "time.h"
#include "AsyncTCP.h"

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFiMulti.h>
#include <LiquidCrystal_I2C.h>

// const char* ssid = "tv2730-OptiPlex-3050";
// const char* password =  "22611182";

const char *ap_ssid[] = {"tv2730-OptiPlex-3050", "TT49", "71216457", "BUGABDRIOD", NULL};
const char *ap_pwd[] = {"22611182", "22927399", "22611182", "12345678", NULL};
const uint8_t ap_idx = 0;

const long gmtOffset_sec = 28800;
// const char* ntpServer = "pool.ntp.org";
// const char* ntpServer = "asia.pool.ntp.org";
//const char* ntpServer = "tim.google.com";
const char* ntpServer = "216.239.35.4";
const int   daylightOffset_sec = 0;
tm timeInfo;
unsigned long lstNTPtime;
unsigned long lastEntryTime;

AsyncWebServer server(80);

char timeStringBuff[50]; //50 chars should be enough
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

char cmd[20];
char inverse;
int cube[7][9];

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

// DDDUUUUUURRLRRLRRLFFFFFFFFFDDDDDDUUURLLRLLRLLBBBBBBBBB
// UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB
const char CUBE_LAYOUT[] = "ULFRBD";
const char CUBE_COLOR[] =  "WOGRBY?";
const uint16_t COLOR_VALUE[] = {TFT_WHITE, TFT_ORANGE, TFT_GREEN , TFT_RED , TFT_BLUE, TFT_YELLOW , TFT_MAROON};

void initCube() {
  for (int j = 0; j < 6; j++)
    for (int i = 0; i < 9; i++) cube[j][i] = CUBE_COLOR[j];
  cube[6][0] = 0;
}

uint16_t colorToValue(char color) {
  int i;
  for (i = 0; i < 6; i++) {
    if (CUBE_COLOR[i] == color) return COLOR_VALUE[i];
  }
  for (i = 0; i < 6; i++) {
    if (CUBE_LAYOUT[i] == color) return COLOR_VALUE[i];
  }
  return COLOR_VALUE[6];
}
char layoutToColor(char layoutCode) {
  int i;
  // return layoutCode;
  for (i = 0; i < 6; i++) {
    if (CUBE_LAYOUT[i] == layoutCode) break;
  }
  //Serial.printf("(%c,%d,%c)",layoutCode,i,CUBE_COLOR[i]);

  return CUBE_COLOR[i];
}


void mapToCube(const char *p) {
  //"UUUUUUUUUR...F...D...L...B..."

  //RRRUUUUUURRDRRDRRDFFFFFFFFFDDDDDDLLLULLULLULLBBBBBBBBB
  // UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB
  //  for(int i=0;i<5;i++) {
  //    cube[i][2]=layoutToColor(*p++);
  //    cube[i][5]=layoutToColor(*p++);
  //    cube[i][8]=layoutToColor(*p++);
  //    cube[i][1]=layoutToColor(*p++);
  //    cube[i][4]=layoutToColor(*p++);
  //    cube[i][7]=layoutToColor(*p++);
  //    cube[i][0]=layoutToColor(*p++);
  //    cube[i][3]=layoutToColor(*p++);
  //    cube[i][6]=layoutToColor(*p++);
  //  }

  int idx[] = {0, 3, 2, 5, 1, 4};
  for (int j = 0; j < 6; j++) {
    cube[idx[j]][0] = layoutToColor(*p++);
    cube[idx[j]][1] = layoutToColor(*p++);
    cube[idx[j]][2] = layoutToColor(*p++);
    cube[idx[j]][3] = layoutToColor(*p++);
    cube[idx[j]][4] = layoutToColor(*p++);
    cube[idx[j]][5] = layoutToColor(*p++);
    cube[idx[j]][6] = layoutToColor(*p++);
    cube[idx[j]][7] = layoutToColor(*p++);
    cube[idx[j]][8] = layoutToColor(*p++);
  }

}

void mapToCube_xxx(const char *p) {
  //"UUUUUUUUUR...F...D...L...B..."

  for (int i = 0; i < 9; i++) cube[0][i] = layoutToColor(p[0 + i]);
  for (int i = 0; i < 9; i++) cube[3][i] = layoutToColor(p[9 + i]);
  for (int i = 0; i < 9; i++) cube[2][i] = layoutToColor(p[18 + i]);
  for (int i = 0; i < 9; i++) cube[5][i] = layoutToColor(p[27 + i]);
  for (int i = 0; i < 9; i++) cube[1][i] = layoutToColor(p[36 + i]);
  for (int i = 0; i < 9; i++) cube[4][i] = layoutToColor(p[45 + i]);

  //  for (int i = 0; i < 9; i++) cube[0][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[1][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[2][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[3][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[4][i] = *p++;
  //  for (int i = 0; i < 9; i++) cube[5][i] = *p++;

}
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

LiquidCrystal_I2C lcd(0x3F, 20, 4);

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#define TFT_GREY 0x0004

#define LED_BUILTIN 2
#define BUTTON_BUILTIN 0
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
    c = colorToValue(cube[0][i]);
    y = tft.height() - (X_OFFSET + (i % 3 + 3) * X_SCALE) - X_SIZE;
    x = Y_OFFSET + (i / 3) * Y_SCALE;
    tft.fillRect(x, y, X_SIZE, Y_SIZE, c);
  }

  for (int j = 1; j < 5; j++) {
    for (int i = 0; i < 9; i++) {
      c = colorToValue(cube[j][i]);
      y = tft.height() - (X_OFFSET + (i % 3 + (j - 1) * 3) * X_SCALE) - X_SIZE;
      x = Y_OFFSET + (i / 3 + 3) * Y_SCALE;
      tft.fillRect(x, y, X_SIZE, Y_SIZE, c);
    }

  }

  for (int i = 0; i < 9; i++) {
    c = colorToValue(cube[5][i]);
    y = tft.height() - (X_OFFSET + (i % 3 + 3) * X_SCALE) - X_SIZE;
    x = Y_OFFSET + (i / 3 + 6) * Y_SCALE;
    tft.fillRect(x, y, X_SIZE,  Y_SIZE, c);
  }


  //  for (int i = 0; i < 6; i++) {
  //    for (int j = 0; j < 9; j++) {
  //      c = colorToValue(cube[i][j]);
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

int isFinish()
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

int isFirstLayerFinish()
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

int isSecondLayerFinish()
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

int isThirdLayerFinish()
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

int isTopCrossFinish()
{
  int result = TRUE;
  for (int i = 0; i < 4; i++)
  {
    result &= cubeColor(0, 7) == cubeColor(0, 4);
    result &= cubeColor(2, 1) == cubeColor(2, 4);
  }
  return result;
}
int isTopCorrnorFinish()
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

int STEP0_center()
{
  processCMD("Cp");
}

int STEP1_bottomCross()
{

  int p;
  int topColor = cubeColor(5, 4);
  //  processCMD("YYP"); // move to bottom!!

  int done;
  for (int i = 0; i < 4; i++)
  {
    processCMD("ZP");
    done = (cubeColor(0, 5) == topColor);
    if (done)
      continue;

    if (cubeColor(3, 1) == topColor)
    {
      processCMD("Rp"); //move to layer3, then ignal it
    }
    else if (cubeColor(3, 7) == topColor)
    {
      processCMD("Rp"); //move to layer3, then ignal it
    }

    for (int i = 0; i < 4; i++)
    {
      if (cubeColor(2, 5) == topColor)
      {
        processCMD("R;p");
        done = TRUE;
        break;
      }
      else if (cubeColor(4, 3) == topColor)
      {
        processCMD("r;p");
        done = TRUE;
        break;
      }
      else if (cubeColor(3, 7) == topColor)
      {
        processCMD("rZuRp");
        done = TRUE;
        break;
      }
      processCMD("Uzp");
    }
    if (done)
      continue;
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

  // OK....take back...

  for (int i = 0; i < 4; i++)
  {
    processCMD("U");
    if (cubeColor(0, 5) != topColor)
      continue;
    for (int j = 0; j < 4; j++)
    {
      processCMD("Uz");
      if (cubeColor(3, 1) == cubeColor(3, 4))
      {
        processCMD("RR;P");
        break;
      }
    }
  }
  // processCMD("YYCP");
  processCMD("CP");
}

// ---------------------------------------------------
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

int equals(unsigned *x1, unsigned *x2)
{
  // printf("comp: %d%d%d", x1[0], x1[1], x1[2]);
  // printf(" ==> %d%d%d\n", x2[0], x2[1], x2[2]);
  return (x1[0] == x2[0] && x1[1] == x2[1] && x1[2] == x2[2]);
}

int rightUpperFrontFinished()
{
  return (cubeColor(0, 8) == cubeColor(0, 4) &&
          cubeColor(2, 2) == cubeColor(2, 1) &&
          cubeColor(3, 0) == cubeColor(3, 1));
}
// ---------------------------------------------------
int STEP2_bottomCorrnor()
{
  unsigned c2[3], c0[3], c1[3];
  processCMD("YY");
  int finished = 0;
  int topColor = cubeColor(0, 4);

  while (finished < 4)
  {
    processCMD("Up");
    if (rightUpperFrontFinished())
    {
      processCMD("v");
      finished++;
      continue;
    }

    c0[0] = cubeColor(0, 4);
    c0[1] = cubeColor(2, 1);
    c0[2] = cubeColor(3, 1);
    sort(c0);

    int p;
    for (p = 0; p < 4; p++)
    {
      c2[0] = (cubeColor(2, 8));
      c2[1] = (cubeColor(3, 6));
      c2[2] = (cubeColor(5, 2));
      sort(c2);
      if (equals(c0, c2))
      {
        break;
      }
      processCMD("Zu");
    }
    if (cubeColor(2, 8) == topColor)
    {
      processCMD("FxxUxxf;p");
      finished = 0;
      processCMD("u");
      continue;
    }
    if (cubeColor(3, 6) == topColor)
    {
      processCMD("rxxuxxR;p");
      finished = 0;
      processCMD("u");
      continue;
    }
    if (cubeColor(5, 2) == topColor)
    {
      processCMD("rxxUxxRxxUxx");
      processCMD("rxxuxxR;p");
      processCMD("u");
      continue;
    }
    if ((c1[0] = cubeColor(0, 8)) == topColor ||
        (c1[1] = cubeColor(2, 2)) == topColor ||
        (c1[2] = cubeColor(3, 0)) == topColor)
    {
      processCMD("rxxUxxRup");
      processCMD("u");
      sort(c1);
    }
  }
  processCMD("XX");
  while (cubeColor(5, 1) == cubeColor(2, 7))
    processCMD("U");
  while (cubeColor(1, 1) != 1)
    processCMD("Z");
  processCMD("P");
}

int STEP3_layer2()
{
  unsigned c0[20], c1[20], isFinish = 0;
  while (isFinish < 4)
  {
    processCMD("ZP");
    c0[0] = cubeColor(2, 4);
    c0[1] = cubeColor(3, 4);
    if (c0[0] == (cubeColor(2, 5)) &&
        c0[1] == (cubeColor(3, 3)))
    {
      // printf("finish: %d,%d,%x,%x\n", c0[0], c0[1], cubeColor(2,5], cubeColor(3,3]);
      isFinish++;
      continue;
    }

    int p;
    for (p = 0; p < 4; p++)
    {
      if ((c1[0] = cubeColor(2, 1)) == c0[0] && (c1[1] = cubeColor(0, 7)) == c0[1])
      {
        //printf("fsame: %d,%d,%d,%d\n", c0[0], c0[1], c1[0], c1[1]);
        processCMD("URurufUFp"); // 54212356
        isFinish = 0;
        break;
      }
      else if ((c1[0] = cubeColor(3, 1)) == c0[1] && (c1[1] = cubeColor(0, 5)) == c0[0])
      {
        //printf("rsame: %d,%d,%d,%d\n", c0[0], c0[1], c1[0], c1[1]);
        processCMD("ufUFURurp"); // 23565421
        isFinish = 0;
        break;
      }
      //printf("compare: %d,%d,%d,%d\n", c0[0], c0[1], c1[0], c1[1]);
      processCMD("U");
    }
    if (p == 4)
    {
      processCMD("URurufUFP"); // 54212356
      isFinish = 0;
    }
  }

  while (cubeColor(1, 1) != 1)
    processCMD("Z");
  processCMD("P");
}

int STEP4_topCross()
{
  unsigned p, color;
  color = cubeColor(0, 4);

  while (1)
  {
    if (cubeColor(0, 1) == color && cubeColor(0, 3) == color)
    {
      if (cubeColor(0, 7) == color && cubeColor(0, 5) == color)
        break;               //finished
      processCMD("rufUFRP"); //123564
      continue;
    }
    if (cubeColor(0, 1) == color && cubeColor(0, 7) == color)
    {
      processCMD("rufUFRP"); //123564
      continue;
    }
    if (cubeColor(0, 3) == color && cubeColor(0, 5) == color)
    {
      processCMD("U");
      continue;
    }
    processCMD("rufUFRP"); //123564
  }
}

int STEP5_topConner()
{
  unsigned color, isFinish;
  isFinish = 0;
  color = cubeColor(0, 4);
  // printf("Start : STEP5_topConner color=%d\n",color);
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

int main_2(int argc, char const *argv[])
{
  char *testcase = "i*Fpfp";
  processCMD(testcase);
}

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
  Serial.printf("isFirstLayerFinish=%d\n", isFirstLayerFinish());
  Serial.printf("isSecondLayerFinish=%d\n", isSecondLayerFinish());
  Serial.printf("isThiLayerFinish=%d\n", isThirdLayerFinish());
  return 0;
}




// ==========================================================================================================

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
  //delay(100);
}
