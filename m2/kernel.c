/* @Copyright Team-2D
  Team members: tandoni yangr zhangq2 zhangx2
*/

void printString(char *);
void readString(char *);
void readSector(char *, int);
int mod(int, int);
int div(int, int);
void handleInterrupt21(int, int, int, int);

int main() {
  char* chars[80];
  char* buffer[512];
  char* line[80];

  /* Test printString */
  printString("Hello World!\0");
  printString("\r\n\0");

  /* Test readString */
  printString("Please enter your string: \0");
  readString(chars);
  printString("\r\n\0");
  printString(chars);
  printString("\r\n\0");

  /* Test readSector */
  readSector(buffer, 30);
  printString(buffer);
  printString("\r\n\0");

  /* Test 0x21 Interrupt */
  makeInterrupt21();
  printString("Please enter your string: \0");
  interrupt(0x21,1,line,0,0);
  printString("\r\n\0");
  interrupt(0x21,0,line,0,0);

  printString("\r\n\0");
  printString("That's it. Thank you for running it.\0");

  while (1) {}
}

void printString(char *chars) {
  char al;
  char ah;
  int ax;
  while (*chars != '\0') {
    al = *chars;
    ah = 0xe;
    ax = ah * 256 + al;
    interrupt(0x10, ax, 0, 0, 0);
    chars++;
  }
}

void readString(char *chars) {
  int size = 0;
  char ret;

  char* backspace = " Backspace ";

  ret = interrupt(0x16, 0, 0, 0, 0);
  while (ret != 0xd) {
    if (ret == 0x8) {
      if (size > 0) {
        /* Backspace */
        size--;
        *(chars + size) = '\0';
        printString(backspace);
      }
    } else {
      *(chars + size) = ret;
      *(chars + size + 1) = '\0';
      size++;
    }
    ret = interrupt(0x16, 0, 0, 0, 0);
  }

}

void readSector(char *chars, int sector) {
  int ah = 2;
  int al = 1;
  int bx = chars;
  int ch = div(sector, 36);
  int cl = mod(sector, 18) + 1;
  int dh = mod(div(sector, 18), 2);
  int dl = 0;

  int ax = ah * 256 + al;
  int cx = ch * 256 + cl;
  int dx = dh * 256 + dl;

  interrupt(0x13, ax, bx, cx, dx);
}

int mod(int a, int b) {
  while (a >= b) {
    a = a - b;
  }
  return a;
}

int div(int a, int b) {
  int quotient = 0;
  while ((quotient + 1) * b <= a) {
    quotient = quotient + 1;
  }
  return quotient;
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
  if (ax == 0) {
    printString(bx);
  } else if (ax == 1) {
    readString(bx);
  } else if (ax == 2) {
    readSector(bx, cx);
  } else {
    printString("Error!!!!!");
  }
}



