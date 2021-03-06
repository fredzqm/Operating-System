/*
  @Copyright Team-2D
  Team members: tandoni yangr zhangq2 zhangx2
*/
#define FILE_NAME_LENGTH 6
#define FILE_ENTRY_LENGTH 32
#define FILE_ENTRY_NUM 16
#define SECTOR_SIZE 512
#define SECTOR_TOTAL_NUM 2880
#define PROC_ENTRY_NUM 8

#define MAP_SECTOR 1
#define ROOT_SECTOR 2

typedef struct {
  char name[FILE_NAME_LENGTH];
  int size;
  int isDirectory;
  int isExecutable;
} File;

/* M5 Step 8 */
typedef struct {
  int active; /* 1=active, 0=inactive */
  int sp; /* stack pointer */
  int waiting; /* >=0 waiting, -1=not waiting */
} ProcEntry;
ProcEntry procTable[PROC_ENTRY_NUM];
int currentProcess;

int roundRobin[PROC_ENTRY_NUM];

/* defined sys calls */
void handleInterrupt21(int, int, int, int);
/* standard IO */
void printString(char *chars);
void readString(char *chars);
/* sectors IO */
void readSector(char *chars, int);
void writeSector(char *chars, int);
/* file IO */
void writeFile(char* name, char* buffer, int numberOfSectors, int dirID);
void readFile(char *filename, char *buffer, int dirID);
void deleteFile(char* name, int dirID);
/* program management */
int runProgram(char* name, int dirID);
void terminate();
void clear();

int searchDirectory(char *directoryBuffer, char *name);
void scanDirectory(int dirID, File* fileInfo, int* fileNum);

/* Process management */
void killProcess(int id);

/* Handle timer interrupt */
void handleTimerInterrupt(int segment, int sp);

void shellWait(int process);


/* Utilities */
int mod(int a, int b);
int div(int a, int b);
void convertIntToString(char* buffer, int n);
void printProcTable();


int main() {
  char chars[512];
  char buffer[13312];
  char line[512];
  char shell[6];
  int i;

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
  /* readString(line);  */
  /* printString(line); */

  /* Add more space */
  /* printString("\r\n"); */

  /* Test readSector */
  /* readSector(buffer, 30); */
  /* printString(buffer); */

  /* Test interrupt */
  /* interrupt(0x21, 1, line, 0, 0); */
  /* interrupt(0x21, 0, line, 0, 0); */

  /* Milestone 3 - readFile */
  /* interrupt(0x21, 3, "messag\0", buffer, 0); */ /* read the file into buffer */
  /*interrupt(0x21, 0, buffer, 0, 0); */   /* print out the file */

  /* Milestone 3 - execute */
  /* interrupt(0x21, 4, "tstprg\0", 0x2000, 0); */
  /* interrupt(0x21, 4, "tstpr2\0", 0x2000, 0); */

  /* Initialize Process Table */
  for (i = 0; i < PROC_ENTRY_NUM; i++) {
    procTable[i].active = 0;
    procTable[i].sp = 0xFF00;
    procTable[i].waiting = -1;
  }
  currentProcess = 0;

  /* Interrupt*/
  makeInterrupt21();
  makeTimerInterrupt();

  /* Milestone 3 - shell */
  interrupt(0x21, 4, shell, 0x2000, 0);

  while (1);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
  if (ax == 0) { /* printString */
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
    readFile(bx, cx, ROOT_SECTOR);
  } else if (ax == 4) {
    runProgram(bx, ROOT_SECTOR);
  } else if (ax == 5) {
    terminate();
  } else if (ax == 6) {
    writeSector(bx, cx);
  } else if (ax == 7) {
    deleteFile(bx, ROOT_SECTOR);
  } else if (ax == 8) {
    writeFile(bx, cx, dx, ROOT_SECTOR);
  } else if (ax == 9) {
    scanDirectory(bx, cx, dx);
  } else if (ax == 10) {
    killProcess(bx);
  } else if (ax == 11) {
    shellWait(runProgram(bx, ROOT_SECTOR));
  } else if (ax == 12) {
    clear();
  } else if (ax == 13) {
    printProcTable();
  } else {
    char errorMsg[8];
    errorMsg[0] = 'E';
    errorMsg[1] = 'r';
    errorMsg[2] = 'r';
    errorMsg[3] = 'o';
    errorMsg[4] = 'r';
    errorMsg[5] = '!';
    errorMsg[6] = '!';
    errorMsg[7] = '\0';
    printString(errorMsg);
  }
}

void clear() {
  int i;
  int j;
  char blankSpace[2];


  char al = 0;
  char bl = 0;
  char dl = 0;
  char ah = 0x02;
  char bh = 0x00;
  char dh = 0x00;
  int bx = bh * 256 + bl;
  int dx = dl * 256 + dh;
  int ax = ah * 256 + al;

  blankSpace[0] = '\n';
  blankSpace[1] = '\0';
  for(i=0; i<25; i++) {
    printString(blankSpace);
  }
  interrupt(0x10, ax, bx, 0, dx);
  interrupt(0x10, 0x42 * 256, 0, 0, 0);
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

void readFile(char *filename, char *buffer, int dirID) {
  /* int i, j, k; */
  int entryOffset;
  char directoryBuffer[SECTOR_SIZE];
  int bufferPointer = 0;
  int sectorPointer = 0;
  readSector(&directoryBuffer, dirID);

  entryOffset = searchDirectory(directoryBuffer, filename);

  if (entryOffset < 0) {
    char errorMessage[15];
    errorMessage[0] = 'F';
    errorMessage[1] = 'i';
    errorMessage[2] = 'l';
    errorMessage[3] = 'e';
    errorMessage[4] = ' ';
    errorMessage[5] = 'n';
    errorMessage[6] = 'o';
    errorMessage[7] = 't';
    errorMessage[8] = ' ';
    errorMessage[9] = 'f';
    errorMessage[10] = 'o';
    errorMessage[11] = 'u';
    errorMessage[12] = 'n';
    errorMessage[13] = 'd';
    errorMessage[14] = '\0';
    printString(errorMessage);

    *buffer = '\0';
    return;
  }

  sectorPointer = entryOffset + 6;

  while (directoryBuffer[sectorPointer] != 0) {
    readSector(buffer + bufferPointer, directoryBuffer[sectorPointer]);
    bufferPointer += SECTOR_SIZE;
    sectorPointer++;
  }
}

int runProgram(char* name, int dirID) {
  char errorMessage[10];
  char number[10];
  char newline[3];
  char programBuffer[13312];
  int i, t, current, curProcUser;
  int segment2;

  readFile(name, programBuffer, ROOT_SECTOR);

  setKernelDataSegment();

  for (i = 0; i < PROC_ENTRY_NUM; i++) {
    if (procTable[i].active == 0 && procTable[i].waiting == -1) {
      procTable[i].sp = 0xFF00;
      t = i;
      break;
    }
  }

  restoreDataSegment();

  /* convertIntToString(number, t); */
  /* number[1] = ' '; */
  /* number[2] = '\0'; */
  /* printString(number); */

  segment2 = (t + 2) * 0x1000;
  for (i = 0; i <= 13312; i++) {
    putInMemory(segment2, i, programBuffer[i]);
  }
  initializeProgram(segment2);
  setKernelDataSegment();
  procTable[t].active = 1;
  restoreDataSegment();
  return t;
}

void killProcess(int id) {
  setKernelDataSegment();
  procTable[id].active = 0;
  restoreDataSegment();
}

void handleTimerInterrupt(int segment, int sp) {
  int curProcUser, curProcSegUser, curProcSpUser, count, wait, parentWait;

  setKernelDataSegment();
  curProcSegUser = segment;
  curProcSpUser = sp;
  curProcUser = div(curProcSegUser, 0x1000) - 2;

  if (curProcUser >= 0) {
    procTable[curProcUser].sp = sp;
  }

  for (count = 0; count <= PROC_ENTRY_NUM; count++) {
    curProcUser = (curProcUser + 1) % PROC_ENTRY_NUM;

    /* TODO: Not very efficient, wait to be changed */
    wait = procTable[curProcUser].waiting;
    if (wait != -1) { /* Current process is waiting for `wait` */
      parentWait = procTable[wait].waiting;
      if (!procTable[wait].active && parentWait == -1) {
        procTable[curProcUser].waiting = -1;
        procTable[curProcUser].active = 1;
      }
    }
    if (procTable[curProcUser].active == 1) {
      currentProcess = curProcUser;
      curProcSegUser = currentProcess * 0x1000 + 0x2000;
      curProcSpUser = procTable[currentProcess].sp;
      break;
    }
  }
  restoreDataSegment();
  returnFromTimer(curProcSegUser, curProcSpUser);
}


void terminate() {
  /* int i; */
  /* char shell[6]; */
  /* shell[0] = 't'; */
  /* shell[1] = 'e'; */
  /* shell[2] = 'r'; */
  /* shell[3] = '\n'; */
  /* shell[4] = '\r'; */
  /* shell[5] = '\0'; */
  /* printString(shell); */
  setKernelDataSegment();
  procTable[currentProcess].active = 0;
  restoreDataSegment();
  while(1);
  /* for (i = 0 ; i < -1; i++); */
  /* runProgram(shell); */
  /* interrupt(0x21, 4, shell, 0x2000, 0); */
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

void deleteFile(char* name, int dirID) {
  int entryOffset, j;
  char directoryBuffer[SECTOR_SIZE];
  char mapBuffer[SECTOR_SIZE];
  /* int sectorPointer = 0; */
  readSector(directoryBuffer, dirID);
  readSector(mapBuffer, MAP_SECTOR);

  entryOffset = searchDirectory(directoryBuffer, name);
  
  if (entryOffset < 0) {
    char errorMessage[];
    errorMessage[0] = 'F';
    errorMessage[1] = 'i';
    errorMessage[2] = 'l';
    errorMessage[3] = 'e';
    errorMessage[4] = ' ';
    errorMessage[5] = 'n';
    errorMessage[6] = 'o';
    errorMessage[7] = 't';
    errorMessage[8] = ' ';
    errorMessage[9] = 'f';
    errorMessage[10] = 'o';
    errorMessage[11] = 'u';
    errorMessage[12] = 'n';
    errorMessage[13] = 'd';
    errorMessage[14] = '\0';
    printString(errorMessage);
    return;
  }

  directoryBuffer[entryOffset] = 0;
  for (j = FILE_NAME_LENGTH; j < FILE_ENTRY_LENGTH; j++) {
    mapBuffer[directoryBuffer[entryOffset + j]] = 0;
    if (directoryBuffer[entryOffset + j] == 0) {
      break;
    }
  }

  writeSector(directoryBuffer, 2);
  writeSector(mapBuffer, 1);
}

void writeFile(char* name, char* buffer, int numberOfSectors, int dirID) {
  int i, j, entryOffset;
  char directoryBuffer[SECTOR_SIZE];
  char mapBuffer[SECTOR_SIZE];
  char sectorPointers[26];
  char errorMessage[19];
  /* char debug[3]; */
  
  readSector(directoryBuffer, ROOT_SECTOR);

  /* try to see if this file already exists */
  entryOffset = searchDirectory(directoryBuffer, name);
  if (entryOffset < 0) { 
    /* find an empty entry to store file info */
    i = findEmptyEntry(directoryBuffer);
    if (i < 0) {
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
    entryOffset = i ;
  }

  readSector(mapBuffer, MAP_SECTOR);
  /* find enough sectors for storage */
  i = 0;
  for (j = 0 ; j < numberOfSectors; j++) {
    while(mapBuffer[i]!=0x00) {
      i++;
      if (i > SECTOR_TOTAL_NUM){
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
    i++;
  }

  /* write file name, and append '\0' at the end if there is space */
  for (j = 0; j < FILE_NAME_LENGTH; j++) {
    directoryBuffer[entryOffset+j] = name[j];
    if (name[j] == '\0') {
      break;
    }
  }

  /* write data into sectors */
  for (j = 0; j < numberOfSectors; j++) {
    mapBuffer[sectorPointers[j]] = 0xFF;
    directoryBuffer[entryOffset+FILE_NAME_LENGTH+j] = sectorPointers[j];
    writeSector(buffer, sectorPointers[j]);
    buffer += SECTOR_SIZE;
  }
  directoryBuffer[entryOffset+FILE_NAME_LENGTH+numberOfSectors] = 0; /* end the sector pointer sequence */
  
  /* write back root directory and map */
  writeSector(directoryBuffer, ROOT_SECTOR);
  writeSector(mapBuffer, MAP_SECTOR);
}

int findEmptyEntry(char* directoryBuffer) {
  int i;
  /* Do not use i = 0! */
  /* When i == 0, he first 32 bytes contain information about this directory */
  for (i = 32; i < SECTOR_SIZE; i+= FILE_ENTRY_LENGTH) {
    if (directoryBuffer[i] == 0) {
      return i;
    }
  }
  return -1;
}

/* return the offset of the matched pointer */
int searchDirectory(char* directoryBuffer, char *name) {
  int i, j;
  for (i = 0; i < FILE_ENTRY_NUM; i++) {
    int match = 1;
    for (j = 0; j < FILE_NAME_LENGTH; j++){
      if (directoryBuffer[i*FILE_ENTRY_LENGTH + j] != name[j]){
        match = 0;
        break;
      }
      if (directoryBuffer[i*FILE_ENTRY_LENGTH + j] == 0) {
        break;
      }
    }
    if (match) {
      return i*FILE_ENTRY_LENGTH;
    }
  }
  return -1;
}


void scanDirectory(int dirID, File* fileInfo, int* fileNum) {
  int entry, flag;
  char directoryBuffer[SECTOR_SIZE];
  readSector(directoryBuffer, dirID);
  /* interrupt(0x21, 2, directoryBuffer, 2, 0); */
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
  }
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

void shellWait(int process) {
   setKernelDataSegment();
   procTable[0].waiting = process;
   procTable[0].active = 0;
   restoreDataSegment();
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

void printProcTable() {
  char errorMessage[10];
  char number[10];
  char newline[3];
  char programBuffer[13312];
  ProcEntry te[PROC_ENTRY_NUM];
  int i, t, current, curProcUser;

  /* ------------ print proc table out */
  for (i = 0; i < PROC_ENTRY_NUM; i++) {
    setKernelDataSegment();
    t = procTable[i].active;
    restoreDataSegment(); 
    te[i].active = t ;
    setKernelDataSegment(); 
    t = procTable[i].sp;
    restoreDataSegment();
    te[i].sp = t;
  }

  setKernelDataSegment(); 
  t = currentProcess;
  restoreDataSegment();
  curProcUser = t;

  errorMessage[0] = ' ';
  errorMessage[1] = ':';
  errorMessage[2] = ' ';
  errorMessage[3] = '\0';
  newline[0] = '\r';
  newline[1] = '\n';
  newline[2] = '\0';
  for (i = 0; i < PROC_ENTRY_NUM; i++) {
    convertIntToString(number, i );
    printString(number);
    printString(errorMessage);
    convertIntToString(number, te[i].active );
    printString(number);
    printString(errorMessage);
    convertIntToString(number, te[i].sp/0x100);
    printString(number);
    printString(errorMessage);
    printString(newline);
  }

  errorMessage[0] = 'C';
  errorMessage[1] = 'u';
  errorMessage[2] = 'r';
  errorMessage[3] = 'r';
  errorMessage[4] = 'e';
  errorMessage[5] = 'n';
  errorMessage[6] = 't';
  errorMessage[7] = ':';
  errorMessage[8] = ' ';
  errorMessage[9] = '\0';
  printString(errorMessage);
  convertIntToString(number, curProcUser);
  printString(number);
  printString(newline);
}
