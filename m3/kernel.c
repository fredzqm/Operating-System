/* @Copyright Team-2D
  Team members: tandoni yangr zhangq2 zhangx2
*/

void printString(char *chars);
void readString(char *chars);
void readSector(char *chars, int);
int mod(int a, int b);
int div(int a, int b);
void handleInterrupt21(int, int, int, int);
void readFile(char *filename, char *buffer);
void executeProgram(char* name, int segment);
void terminate();

int main() {
  char chars[512];
  char buffer[13312];
  char line[512];
  char shell[6];

  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';

  /* Test printString */
  /* printString("Hello World!\r\n\0"); */

  /* Test readString */
  /* printString("Enter a line: \0"); */
  /* readString(line); */
  /* printString(line); */

  /* Add more space */
  /* printString("\r\n"); */

  /* Test readSector */
  /* readSector(buffer, 30); */
  /* printString(buffer); */

  /* Interrupt*/
  makeInterrupt21();
  /* interrupt(0x21, 1, line, 0, 0); */
  /* interrupt(0x21, 0, line, 0, 0); */

  /* Milestone 3 - readFile */
  /* interrupt(0x21, 3, "messag\0", buffer, 0); */ /* read the file into buffer */
  /*interrupt(0x21, 0, buffer, 0, 0); */   /* print out the file */

  /* Milestone 3 - executeProgram */
  // interrupt(0x21, 4, "tstprg\0", 0x2000, 0);
  // interrupt(0x21, 4, "tstpr2\0", 0x2000, 0);

  /* Milestone 3 - shell */
  interrupt(0x21, 4, shell, 0x2000, 0);

  // while (1) {}
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
    if (ret == 0x8) { /* Backspace */
      if (size > 0) {
        *(chars + (--size)) = '\0';
        printString(backspace);
      }
    } else {
      *(chars + size++) = ret;
    }
    ret = interrupt(0x16, 0, 0, 0, 0);
  }
  *(chars + size) = '\r';
  *(chars + size +1) = '\n';
  *(chars + size +2) = '\0';
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
  } else if (ax == 3) {
    readFile(bx, cx);
  } else if (ax == 4) {
    executeProgram(bx, cx);
  } else if (ax == 5) {
    terminate();
  } else {
    printString("Error!!!!!");
  }
}

void readFile(char *filename, char *buffer) {
  int i, j;
  char directoryBuffer[512];
  int sectorPointer = 0;
  readSector(&directoryBuffer, 2);

  for (i = 0; i < 16; i++) {
    int match = 1;
    for (j = 0; j < 6; j++){
      if (directoryBuffer[i*32 + j] != filename[j]){
        match = 0;
        break;
      }
    }
    if (match) {
      sectorPointer = i*32+j;
      break;
    }
  }

  if (sectorPointer == 0) {
    return;
  }

  while (directoryBuffer[sectorPointer] != 0) {
    readSector(buffer, directoryBuffer[sectorPointer]);
    buffer += 512;
    sectorPointer++;
  }
}

void executeProgram(char* name, int segment) {
  char programBuffer[13312];
  int programPointer = 0;
  readFile(name, programBuffer);
  /* '\3' means end of text. could be a bug. */
  while (programBuffer[programPointer] != '\0') {
    putInMemory(segment, programPointer, programBuffer[programPointer]);
    programPointer++;
  }
  launchProgram(segment);
}

void terminate() {
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';
  interrupt(0x21, 4, shell, 0x2000, 0);
}
