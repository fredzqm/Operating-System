/* @Copyright Team-2D
  Team members: tandoni yangr zhangq2 zhangx2
*/

void printString(char *chars);
void readString(char *chars);
void readSector(char *chars, int);
void writeSector(char *chars, int);
void writeFile(char* name, char* buffer, int numberOfSectors);
int mod(int a, int b);
int div(int a, int b);
void handleInterrupt21(int, int, int, int);
void readFile(char *filename, char *buffer);
void executeProgram(char* name, int segment);
void terminate();
void deleteFile(char* name);

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
  // readString(line); 
  // printString(line);

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
  /* interrupt(0x21, 4, "tstprg\0", 0x2000, 0); */
  /* interrupt(0x21, 4, "tstpr2\0", 0x2000, 0); */

  /* Milestone 3 - shell */
  interrupt(0x21, 4, shell, 0x2000, 0);

}

void printString(char *chars) {
  char al;
  char ah;
  int ax;
  while (*chars != '\0') {
    interrupt(0x10, 0xe * 256 + *chars, 0, 0, 0);
    chars++;
  }
}

void readString(char *chars) {
  int size = 0;
  char ret;
  
  ret = interrupt(0x16, 0, 0, 0, 0);
  while (ret != 0xd) {
    if (ret == 0x8) { /* Backspace */
      if (size > 0) {
        interrupt(0x10, 0xe * 256 + ret, 0, 0, 0);
        interrupt(0x10, 0xe * 256 + ' ', 0, 0, 0);
        interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
        *(chars + (--size)) = '\0';
      }
    } else {
      interrupt(0x10, 0xe * 256 + ret, 0, 0, 0);
      *(chars + size++) = ret;
    }
    ret = interrupt(0x16, 0, 0, 0, 0);
  }
  *(chars + size) = '\r';
  *(chars + size +1) = '\n';
  *(chars + size +2) = '\0';
}

int readStringAndReturnSize(char *chars) {
  int size = 0;
  char ret;
  
  ret = interrupt(0x16, 0, 0, 0, 0);
  while (ret != 0xd) {
    if (ret == 0x8) { /* Backspace */
      if (size > 0) {
        interrupt(0x10, 0xe * 256 + ret, 0, 0, 0);
        interrupt(0x10, 0xe * 256 + ' ', 0, 0, 0);
        interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
        *(chars + (--size)) = '\0';
      }
    } else {
      interrupt(0x10, 0xe * 256 + ret, 0, 0, 0);
      *(chars + size++) = ret;
    }
    ret = interrupt(0x16, 0, 0, 0, 0);
  }
  *(chars + size) = '\r';
  *(chars + size +1) = '\n';
  *(chars + size +2) = '\0';

  return size;
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
  char errorMsg[8];
  errorMsg[0] = 'E';
  errorMsg[1] = 'r';
  errorMsg[2] = 'r';
  errorMsg[3] = 'o';
  errorMsg[4] = 'r';
  errorMsg[5] = '!';
  errorMsg[6] = '!';
  errorMsg[7] = '\0';

  if (ax == 0) {
    printString(bx);
  } else if (ax == 1) {
    if (cx == 0) {
      readStringAndReturnSize(bx);
    } else {
      *((int*)cx) = readStringAndReturnSize(bx);
    }
  } else if (ax == 2) {
    readSector(bx, cx);
  } else if (ax == 3) {
    readFile(bx, cx);
  } else if (ax == 4) {
    executeProgram(bx, cx);
  } else if (ax == 5) {
    terminate();
  } else if (ax == 6) {
    writeSector(bx, cx);
  } else if (ax == 7) {
    deleteFile(bx);
  } else if (ax == 8) {
    writeFile(bx, cx, dx);
  } else {
    printString(errorMsg);
  }
}

void readFile(char *filename, char *buffer) {
  int i, j, k;
  char directoryBuffer[512];
  int bufferPointer = 0;
  int sectorPointer = 0;
  readSector(&directoryBuffer, 2);

  for (i = 0; i < 16; i++) {
    int match = 1;
    for (j = 0; j < 6; j++){
      if (directoryBuffer[i*32 + j] != filename[j]) {
        match = 0;
        break;
      }
      if (directoryBuffer[i*32 + j] == 0) {
        break;
      }
    }
    if (match) {
      sectorPointer = i*32+6;
      break;
    }
  }

  if (sectorPointer == 0) {
    *buffer = '\0';
    return;
  }

  while (directoryBuffer[sectorPointer] != 0) {
    readSector(buffer + bufferPointer, directoryBuffer[sectorPointer]);
    bufferPointer += 512;
    sectorPointer++;
  }
}

void executeProgram(char* name, int segment) {
  char programBuffer[13312];
  int programPointer;
  readFile(name, programBuffer);
  for (programPointer = 0; programPointer <= 13312; programPointer++) {
    putInMemory(segment, programPointer, programBuffer[programPointer]);
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

void writeSector(char *chars, int sector) {
  int ah = 3;
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

void deleteFile(char* name) {
  int i, j;
  char directoryBuffer[512];
  char mapBuffer[512];

  int sectorPointer = 0;

  readSector(directoryBuffer, 2);
  readSector(mapBuffer, 1);

  for (i = 0; i < 16; i++) {
    int match = 1;
    for (j = 0; j < 6; j++){
      if (directoryBuffer[i*32 + j] != name[j]){
        match = 0;
        break;
      }
      if (directoryBuffer[i*32 + j] == 0) {
        break;
      }
    }
    if (match) {
      sectorPointer = i*32+j;
      directoryBuffer[i*32] = 0x00;
      writeSector(directoryBuffer, 2);
      while (directoryBuffer[sectorPointer] != 0 && sectorPointer < i*32) {
        mapBuffer[directoryBuffer[sectorPointer]] = 0x00;
        sectorPointer++;
      }
      writeSector(mapBuffer, 1);
      break;
    }
  }
}

void writeFile(char* name, char* buffer, int numberOfSectors) {
  int i, j, flag;
  char directoryBuffer[512];
  char mapBuffer[512];
  char sectorPointers[26];
  char errorMessage[10];
  readSector(directoryBuffer, 2);
  readSector(mapBuffer, 1);

  i = 0;
  for (j = 0 ; j < numberOfSectors; j++) {
    while(mapBuffer[i]!=0x00) {
      i++;
      if (i > 2880){
        errorMessage[0] = 'D';
        errorMessage[1] = 'i';
        errorMessage[2] = 's';
        errorMessage[3] = 'k';
        errorMessage[4] = ' ';
        errorMessage[5] = 'f';
        errorMessage[6] = 'u';
        errorMessage[7] = 'l';
        errorMessage[8] = 'l';
        errorMessage[9] = '\0';
        printString(errorMessage);
        return;
      }
    }
    sectorPointers[j] = i;
  }

  i = 0;
  while(directoryBuffer[i]!=0x00) {
    i+=32;
  }

  if (i > 512) {
    errorMessage[0] = 'F';
    errorMessage[1] = 'i';
    errorMessage[2] = 'l';
    errorMessage[3] = 'e';
    errorMessage[4] = ' ';
    errorMessage[5] = 'f';
    errorMessage[6] = 'u';
    errorMessage[7] = 'l';
    errorMessage[8] = 'l';
    errorMessage[9] = '\0';
    printString(errorMessage);
    return;
  }

  flag = 0;
  for (j = 0; j < 6; j++) {
    if (name[j] == '\0') {
      flag = 1;
    }
    if (flag == 0) {
      directoryBuffer[i+j] = name[j];
    } else {
      directoryBuffer[i+j] = '\0';
    }
  }

  for (j = 0; j < numberOfSectors; j++) {
    mapBuffer[sectorPointers[j]] = 0xFF;
    directoryBuffer[i+j+6] = sectorPointers[j];
    writeSector(buffer, sectorPointers[j]);
    printString(errorMessage);
    buffer += 512;
  }

  writeSector(directoryBuffer, 2);
  writeSector(mapBuffer, 1);
}