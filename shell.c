/* @Copyright 2016
 * Author: Ishank Tandon, Fred Zhang, Jackie Zhang, Runzhi Yang
 */


#define SECTOR_SIZE 512
#define MAX_FILE_SIZE 13312
#define ROOT_SECTOR 2

void handleInput(char *input);

/* variable functionality we implemented*/
void cat(char* filename);
void touch(char* filename);
void cp(char* filename1, char* filename2);
void exec(char* filename);
void ls();
void cd(char* commandType);

/* syscall methods, implemented with interrupt*/
void printString(char *chars);
void readString(char *chars, int* readSize);
void writeFile(char* filename, char* buffer, int numberOfSectors);
void readFile(char *filename, char *buffer);
void deleteFile(char* filename);
void executeFile(char* filename);
void execforeground(char* filename);
void kill(int id);
void terminate();
void clear();
void ps();

/* Customization */
void bgcolor(char *color);

/* helper methods*/
int sizeOfSector(char* filename);
void convertIntToString(char* buffer, int);
int compareStr(char *a, char *b);
int mod(int a, int b);


char newLine[3];
char deliminator[4];

int main() {
  char prompt[4];
  char buffer[512];

  prompt[0] = 'A';
  prompt[1] = ':';
  prompt[2] = '>';
  prompt[3] = '\0';
  newLine[0] = '\r';
  newLine[1] = '\n';
  newLine[2] = '\0';

  deliminator[0] = ' ';
  deliminator[1] = '-';
  deliminator[2] = ' ';
  deliminator[3] = '\0';

  enableInterrupts();

  clear();

  while (1) {
    printString(prompt);
    readString(buffer, 0);
    printString(newLine);
    handleInput(buffer);
    printString(newLine);
    /* terminate();*/
  }
}

void handleInput(char *input) {
  char commandName[512];
  char commandArg[512];
  char commandArg2[512];

  char commandType[5];
  char commandCat[4];
  char commandDelete[7];
  char commandRm[3];
  char commandExecute[8];
  char commandCreate[7];
  char commandTouch[6];
  char commandKill[5];
  char commandCopy[5];
  char commandCp[3];
  char commandDir[4];
  char commandLs[3];
  char badCommand[13];
  char commandCd[3];
  char commandQuit[5];
  char commandClear[6];
  char commandHelp[5];
  char commandPs[3];
  char commandFgColor[8];
  char commandBgColor[8];
  char commandExeforeground[15];
  int pointer = 0;

  char helpFileName[8];

  commandType[0] = 't';
  commandType[1] = 'y';
  commandType[2] = 'p';
  commandType[3] = 'e';
  commandType[4] = '\0';

  commandCat[0] = 'c';
  commandCat[1] = 'a';
  commandCat[2] = 't';
  commandCat[3] = '\0';

  commandExecute[0] = 'e';
  commandExecute[1] = 'x';
  commandExecute[2] = 'e';
  commandExecute[3] = 'c';
  commandExecute[4] = 'u';
  commandExecute[5] = 't';
  commandExecute[6] = 'e';
  commandExecute[7] = '\0';

  commandDelete[0] = 'd';
  commandDelete[1] = 'e';
  commandDelete[2] = 'l';
  commandDelete[3] = 'e';
  commandDelete[4] = 't';
  commandDelete[5] = 'e';
  commandDelete[6] = '\0';

  commandKill[0] = 'k';
  commandKill[1] = 'i';
  commandKill[2] = 'l';
  commandKill[3] = 'l';
  commandKill[4] = '\0';

  commandRm[0] = 'r';
  commandRm[1] = 'm';
  commandRm[2] = '\0';

  commandCreate[0] = 'c';
  commandCreate[1] = 'r';
  commandCreate[2] = 'e';
  commandCreate[3] = 'a';
  commandCreate[4] = 't';
  commandCreate[5] = 'e';
  commandCreate[6] = '\0';

  commandTouch[0] = 't';
  commandTouch[1] = 'o';
  commandTouch[2] = 'u';
  commandTouch[3] = 'c';
  commandTouch[4] = 'h';
  commandTouch[5] = '\0';
  
  commandCopy[0] = 'c';
  commandCopy[1] = 'o';
  commandCopy[2] = 'p';
  commandCopy[3] = 'y';
  commandCopy[4] = '\0';

  commandCp[0] = 'c';
  commandCp[1] = 'p';
  commandCp[2] = '\0';

  commandDir[0] = 'd';
  commandDir[1] = 'i';
  commandDir[2] = 'r';
  commandDir[3] = '\0';

  commandLs[0] = 'l';
  commandLs[1] = 's';
  commandLs[2] = '\0';

  commandCd[0] = 'c';
  commandCd[1] = 'd';
  commandCd[2] = '\0';

  commandQuit[0] = 'q';
  commandQuit[1] = 'u';
  commandQuit[2] = 'i';
  commandQuit[3] = 't';
  commandQuit[4] = '\0';

  commandClear[0] = 'c';
  commandClear[1] = 'l';
  commandClear[2] = 'e';
  commandClear[3] = 'a';
  commandClear[4] = 'r';
  commandClear[5] = '\0';

  commandExeforeground[0] = 'e';
  commandExeforeground[1] = 'x';
  commandExeforeground[2] = 'e';
  commandExeforeground[3] = 'c';
  commandExeforeground[4] = 'f';
  commandExeforeground[5] = 'o';
  commandExeforeground[6] = 'r';
  commandExeforeground[7] = 'e';
  commandExeforeground[8] = 'g';
  commandExeforeground[9] = 'r';
  commandExeforeground[10] = 'o';
  commandExeforeground[11] = 'u';
  commandExeforeground[12] = 'n';
  commandExeforeground[13] = 'd';
  commandExeforeground[14] = '\0';

  commandHelp[0] = 'h';
  commandHelp[1] = 'e';
  commandHelp[2] = 'l';
  commandHelp[3] = 'p';
  commandHelp[4] = '\0';

  commandPs[0] = 'p';
  commandPs[1] = 's';
  commandPs[2] = '\0';

  commandBgColor[0] = 'b';
  commandBgColor[1] = 'g';
  commandBgColor[2] = 'c';
  commandBgColor[3] = 'o';
  commandBgColor[4] = 'l';
  commandBgColor[5] = 'o';
  commandBgColor[6] = 'r';
  commandBgColor[7] = '\0';

  commandFgColor[0] = 'f';
  commandFgColor[1] = 'g';
  commandFgColor[2] = 'c';
  commandFgColor[3] = 'o';
  commandFgColor[4] = 'l';
  commandFgColor[5] = 'o';
  commandFgColor[6] = 'r';
  commandFgColor[7] = '\0';

  helpFileName[0] = 'h';
  helpFileName[1] = 'e';
  helpFileName[2] = 'l';
  helpFileName[3] = 'p';
  helpFileName[4] = '.';
  helpFileName[5] = 't';
  helpFileName[6] = 'x';
  helpFileName[7] = '\0';

  /* process command line input */
  while (*input != ' ' && *input != '\r' && *input != '\n' && *input != '\0') {
    commandName[pointer] = *input;
    input++;
    pointer++;
  }
  commandName[pointer] = '\0';

  input++;
  pointer = 0;
  while (*input != ' ' && *input != '\r' && *input != '\n' && *input != '\0') {
    commandArg[pointer] = *input;
    input++;
    pointer++;
  }
  commandArg[pointer] = '\0';

  input++;
  pointer = 0;
  while (*input != ' ' && *input != '\r' && *input != '\n' && *input != '\0') {
    commandArg2[pointer] = *input;
    input++;
    pointer++;
  }
  commandArg2[pointer] = '\0';

  if (compareStr(commandName, commandType) == 1) {
    cat(commandArg);
  } else if (compareStr(commandName, commandCat) == 1) {
    cat(commandArg);
  } else if (compareStr(commandName, commandExecute) == 1) {
    executeFile(commandArg);
  } else if (compareStr(commandName, commandDelete) == 1) {
    deleteFile(commandArg); /* delete file*/
  } else if (compareStr(commandName, commandRm) == 1) {
    deleteFile(commandArg); /* delete file*/
  } else if (compareStr(commandName, commandCreate) == 1) {
    touch(commandArg); /* create file*/
  } else if (compareStr(commandName, commandTouch) == 1) {
    touch(commandArg); /* create file*/
  } else if (compareStr(commandName, commandKill) == 1) {
    kill((int) commandArg[0] - 48);/* kill process*/
  } else if (compareStr(commandName, commandQuit) == 1) {
    terminate();
  } else if (compareStr(commandName, commandClear) == 1) {
    clear();
  } else if (compareStr(commandName, commandExeforeground) == 1) {
    execforeground(commandArg);
  } else if (compareStr(commandName, commandHelp) == 1) {
    cat(helpFileName);
  } else if (compareStr(commandName, commandPs) == 1) {
    ps();
  } else if (compareStr(commandName, commandCopy) == 1) {
    cp(commandArg, commandArg2);
  } else if (compareStr(commandName, commandCp) == 1) {
    cp(commandArg, commandArg2);
  } else if (compareStr(commandName, commandDir) == 1) {
    ls();
  } else if (compareStr(commandName, commandLs) == 1) {
    ls();
  } else if (compareStr(commandName, commandBgColor) == 1) {
    bgcolor(commandArg);
  } else if (compareStr(commandName, commandFgColor) == 1) {
    // fgcolor(commandArg);
  } else if (compareStr(commandName, commandCd) == 1) {
    cd(commandType);
  } else {
    char commandArg[512];
    badCommand[0] = 'B';
    badCommand[1] = 'a';
    badCommand[2] = 'd';
    badCommand[3] = ' ';
    badCommand[4] = 'C';
    badCommand[5] = 'o';
    badCommand[6] = 'm';
    badCommand[7] = 'm';
    badCommand[8] = 'a';
    badCommand[9] = 'n';
    badCommand[10] = 'd';
    badCommand[11] = '!';
    badCommand[12] = '\0';
    printString(badCommand);
  }

}

void cat(char* filename) {
  char buf[MAX_FILE_SIZE];
  readFile(filename, buf);
  printString(buf);
}

void touch(char* filename) {
  char buf[MAX_FILE_SIZE];
  char promptLine[3];
  int count = 0;
  int readStringSize = 0;

  promptLine[0] = '>';
  promptLine[1] = ' ';
  promptLine[2] = '\0';

  printString(promptLine); /* print prompt */
  readString(buf + count, &readStringSize); /* read input*/
  while(readStringSize != 0) {
    count += readStringSize + 2;
    printString(newLine); /* print a new*/
    printString(promptLine); /* print prompt*/
    readString(buf + count, &readStringSize); /* read input*/
  }
  buf[count - 2] = '\0';
  writeFile(filename, buf, count/SECTOR_SIZE + 1);
}

void cp(char* filename1, char* filename2) {
  char buffer[13312];
  char num[10];
  readFile(filename1, buffer);
  writeFile(filename2, buffer, sizeOfSector(filename1));
}

void ls() {
  int entry, flag;
  char directoryBuffer[SECTOR_SIZE];
  interrupt(0x21, 2, directoryBuffer, 2, 0);
  for (entry = 0; entry < 16; entry++) {
    char filename[7];
    char number[50];
    int pointer;
    int j;
    pointer = entry * 32;
    if (directoryBuffer[pointer] == 0x00) {
      continue;
    }
    flag = 0;
    for (j = 0; j < 6; j++) {
      if (directoryBuffer[pointer + j] == 0) {
        flag = 1;
      }
      if (!flag) {
        filename[j] = directoryBuffer[pointer + j];
      } else {
        filename[j] = '\0';
        break;
      }
    }
    convertIntToString(number, sizeOfSector(filename));
    printString(filename);
    printString(deliminator);
    printString(number);
    printString(newLine);
  }
}

void cd(char* commandType) {

}

int sizeOfSector(char* filename) {
  int i, j, count;
  char directoryBuffer[SECTOR_SIZE];
  int sectorPointer = 0;

  interrupt(0x21, 2, &directoryBuffer, 2, 0); /* readSector*/

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
    return 0;
  }

  count = 0;
  while (directoryBuffer[sectorPointer] != 0) {
    sectorPointer++;
    count++;
  }

  return count;
}


void convertIntToString(char* buffer, int n) {
  int i = 0;
  int t = 0;
  int isNeg = n < 0;
  int n1 = isNeg ? -n : n;

  while(n1!=0) {
      buffer[i++] = mod(n1, 10) + '0';
      n1=n1/10;
  }
  if(isNeg) {
      buffer[i++] = '-';
  }
  buffer[i] = '\0';
  for(t = 0; t < i/2; t++) {
      buffer[t] ^= buffer[i-t-1];
      buffer[i-t-1] ^= buffer[t];
      buffer[t] ^= buffer[i-t-1];
  }
  if (n == 0) {
      buffer[0] = '0';
      buffer[1] = '\0';
  }
}


int compareStr(char *a, char *b) {
  while (1) {
    if (*a != *b) {
      return 0;
    }
    if (*a == '\0' || *a == '\r' || *a == '\n') {
      break;
    }
    a++;
    b++;
  }
  return 1;
}


int mod(int a, int b) {
  while (a >= b) {
    a = a - b;
  }
  return a;
}




/* ----------- standard IO*/
void printString(char *chars) { /* make a sys call to kernel */
  interrupt(0x21, 0, chars, 0, 0); /*print the prompt*/
}

void readString(char *chars, int* readSize) { /* make a sys call to kernel */
  interrupt(0x21, 1, chars, readSize, 0); /* read the instruction */
}

/* ----------- File IO*/
void writeFile(char* filename, char* buffer, int numberOfSectors) { /* make a sys call to kernel */
  interrupt(0x21, 8, filename, buffer, numberOfSectors); /* writeFile*/
}

void readFile(char *filename, char *buffer) { /* make a sys call to kernel */
  interrupt(0x21, 3, filename, buffer, 0); /* readFile*/
}

void deleteFile(char* filename) { /* make a sys call to kernel */
  interrupt(0x21, 7, filename, 0, 0); /* delete file*/
}

void executeFile(char* filename) { /* make a sys call to kernel*/
  interrupt(0x21, 4, filename, 0x2000, 0); /* execute file at 0x2000 segment*/
}

void execforeground(char* filename) { /* make a sys call to kernel*/
  interrupt(0x21, 11, filename, 0x2000, 0); /* execute file at 0x2000 segment*/
}

/* ----------- Process control*/
void kill(int id) {
  interrupt(0x21, 10, id, 0, 0);
}

void terminate() { /* make a sys call to kernel */
  interrupt(0x21, 5, 0, 0, 0); /* exit, then the kernel will reload the shell*/
}

void clear() {
  interrupt(0x21, 12, 0, 0, 0);
}

void ps() {
  interrupt(0x21, 13, 0, 0, 0); /* Print process table */
}

/* Customization */
void bgcolor(char *color) {
  char colorBlack[6];
  char colorPurple[7];
  int i;
  int j;
  char blankSpace[2];
  char al = 0;
  char bl = 0;
  char ah = 0x0B;
  char bh = 0x00;
  int bx;
  int ax = ah * 256 + al;

  colorBlack[0] = 'b';
  colorBlack[1] = 'l';
  colorBlack[2] = 'a';
  colorBlack[3] = 'c';
  colorBlack[4] = 'k';
  colorBlack[5] = '\0';

  colorPurple[0] = 'p';
  colorPurple[1] = 'u';
  colorPurple[2] = 'r';
  colorPurple[3] = 'p';
  colorPurple[4] = 'l';
  colorPurple[5] = 'e';
  colorPurple[6] = '\0';

  if (compareStr(color, colorPurple)) {
    bl = 0x05;
    bx = bh * 256 + bl;
    interrupt(0x10, ax, bx, 0, 0);
  } else if (compareStr(color, colorBlack)) {
    bl = 0x00;
    bx = bh * 256 + bl;
    interrupt(0x10, ax, bx, 0, 0);
  }
}

