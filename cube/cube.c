
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

unsigned char cube[6][9];
char allSteps[256];
int steps = 0;

char *COLOR_RESET = "\x1B[0m";

char *COLOR[] = {
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

char *COLOR_BG[] = {
    "\033[3;42;30m",
    "\033[3;43;30m",
    "\033[3;44;30m",
    "\033[3;104;30m",
    "\033[3;100;30m",
    "\033[3;47;35m",
    "\033[2;47;35m",
    "\033[1;47;35m"};

#define MODE_VALUE 16
int cubeColor(int pannel, int pos)
{
  return (cube[pannel][pos] / MODE_VALUE);
}
int cubeValue(int pannel, int pos)
{
  return (cube[pannel][pos] % MODE_VALUE);
}

void printColor(int pannel, int pos)
{
  int value = cubeValue(pannel, pos);
  char *color = COLOR_BG[cubeColor(pannel, pos)];
  printf("%s%d%s", color, value, COLOR_RESET);
}

void initCube()
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

int printCube()
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

int printCube_0()
{
  printf("      |%d.%d.%d|\n", cube[0][0] & 0x0f, cube[0][1], cube[0][2]);
  printf("      |%d.%d.%d|\n", cube[0][3], cube[0][4], cube[0][5]);
  printf("      |%d.%d.%d|\n", cube[0][6], cube[0][7], cube[0][8]);
  printf("-------------------------\n");
  printf("|%d.%d.%d|%d.%d.%d|%d.%d.%d|%d.%d.%d|\n", cube[1][0], cube[1][1], cube[1][2], cube[2][0], cube[2][1], cube[2][2], cube[3][0], cube[3][1], cube[3][2], cube[4][0], cube[4][1], cube[4][2]);
  printf("|%d.%d.%d|%d.%d.%d|%d.%d.%d|%d.%d.%d|\n", cube[1][3], cube[1][4], cube[1][5], cube[2][3], cube[2][4], cube[2][5], cube[3][3], cube[3][4], cube[3][5], cube[4][3], cube[4][4], cube[4][5]);
  printf("|%d.%d.%d|%d.%d.%d|%d.%d.%d|%d.%d.%d|\n", cube[1][6], cube[1][7], cube[1][8], cube[2][6], cube[2][7], cube[2][8], cube[3][6], cube[3][7], cube[3][8], cube[4][6], cube[4][7], cube[4][8]);
  printf("-------------------------\n");
  printf("      |%d.%d.%d|\n", cube[5][0], cube[5][1], cube[5][2]);
  printf("      |%d.%d.%d|\n", cube[5][3], cube[5][4], cube[5][5]);
  printf("      |%d.%d.%d|\n", cube[5][6], cube[5][7], cube[5][8]);
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
void zClockUnWize() {
  zClockWize(),zClockWize(),zClockWize();
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
void yClockUnWize()
{
  yClockWize(),yClockWize(),yClockWize();
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
void xClockUnWize() {
  xClockWize();
  xClockWize();
  xClockWize();
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
    xClockUnWize();
    break;
  case 4:
    yClockUnWize();
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
        printCube();
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
      processCMD("FrFLLfRFLLFFP"); //	61677-34677-66
    }
    else
    {
      processCMD("FrFLLfRFLLFFP"); //	61677-34677-66
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
int main(int argc, char const *argv[])
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
  puts(allSteps);
  // int result=isFirstLayerFinish();
  printf("isFirstLayerFinish=%d\n", isFirstLayerFinish());
  printf("isSecondLayerFinish=%d\n", isSecondLayerFinish());
  printf("isThiLayerFinish=%d\n", isThirdLayerFinish());
  return 0;
}
