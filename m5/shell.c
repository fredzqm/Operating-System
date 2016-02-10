/* @Copyright 2016
 * Author: Ishank Tandon, Fred Zhang, Jackie Zhang, Runzhi Yang
 */


#define SECTOR_SIZE 512
#define MAX_FILE_SIZE 13312
#define ROOT_SECTOR 2

void handleInput(char *input);

// variable functionality we implemented
void cat(char* filename);
void touch(char* filename);
void cp(char* filename1, char* filename2);
void exec(char* filename);
void ls();
void cd(char* commandType);


// syscall methods, implemented with interrupt
void printString(char *chars);
void readString(char *chars, int* readSize);
void writeFile(char* filename, char* buffer, int numberOfSectors);
void readFile(char *filename, char *buffer);
void deleteFile(char* filename);
void executeFile(char* filename);
void kill(int id);
void exit();


// helper methods
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

  while (1) {
    printString(prompt);
    readString(buffer, 0);
    printString(newLine);
    handleInput(buffer);
    printString(newLine);
    // exit();
  }
}

void handleInput(char *input) {
  char commandName[512];
  char commandArg[512];

  char commandType[5];
  char commandCat[4];
  char commandDelete[7];
  char commandRm[3];
  char commandExecute[8];
  char commandCreate[7];
  char commandKill[5];
  char commandCopy[5];
  char commandDir[4];
  char badCommand[13];
  char commandCd[3];
  int pointer = 0;

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

  commandCopy[0] = 'c';
  commandCopy[1] = 'o';
  commandCopy[2] = 'p';
  commandCopy[3] = 'y';
  commandCopy[4] = '\0';

  commandDir[0] = 'd';
  commandDir[1] = 'i';
  commandDir[2] = 'r';
  commandDir[3] = '\0';

  commandCd[0] = 'c';
  commandCd[1] = 'd';
  commandCd[2] = '\0';

  while (*input != ' ' && *input != '\r' && *input != '\n' && *input != '\0') {
    commandName[pointer] = *input;
    input++;
    pointer++;
  }
  commandName[pointer] = '\0';

  input++;
  pointer = 0;
  while (*input != '\r' && *input != '\n' && *input != '\0') {
    commandArg[pointer] = *input;
    input++;
    pointer++;
  }
  commandArg[pointer] = '\0';

  if (compareStr(commandName, commandType) == 1) {
    cat(commandArg);
  } else if (compareStr(commandName, commandCat) == 1) {
    cat(commandArg);
  } else if (compareStr(commandName, commandExecute) == 1) {
    executeFile(commandArg);
  } else if (compareStr(commandName, commandDelete) == 1) {
    deleteFile(commandArg); // delete file
  } else if (compareStr(commandName, commandRm) == 1) {
    deleteFile(commandArg); // delete file
  } else if (compareStr(commandName, commandCreate) == 1) {
    touch(commandArg); // create file
  } else if (compareStr(commandName, commandKill) == 1) {
    kill((int) commandArg[0] - 48);// kill process
  } else if (compareStr(commandName, commandCopy) == 1) {
    char arg1[512];
    char arg2[512];
    int ptr = 0;
    int ptrArg2 = 0;


    while(commandArg[ptr] != ' ') {
      arg1[ptr] = commandArg[ptr];
      ptr++;
    }
    arg1[ptr] = '\0';
    ptr++;
    while(commandArg[ptr] != '\0' && commandArg[ptr] != '\r' &&
          commandArg[ptr] != '\n' && commandArg[ptr] != ' ') {
      arg2[ptrArg2] = commandArg[ptr];
      ptr++;
      ptrArg2++;
    }
    arg2[ptrArg2] = '\0';
    cp(arg1, arg2);
  } else if (compareStr(commandName, commandDir) == 1) {
    ls();
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

  printString(promptLine); // print prompt 
  readString(buf + count, &readStringSize); // read input
  while(readStringSize != 0) {
    count += readStringSize + 2;
    printString(newLine); // print a new
    printString(promptLine); // print prompt
    readString(buf + count, &readStringSize); // read input
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

  interrupt(0x21, 2, &directoryBuffer, 2, 0); // readSector

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




// ----------- standard IO
void printString(char *chars) { // make a sys call to kernel 
  interrupt(0x21, 0, chars, 0, 0); //print the prompt
}

void readString(char *chars, int* readSize) { // make a sys call to kernel 
  interrupt(0x21, 1, chars, readSize, 0); // read the instruction 
}

// ----------- File IO
void writeFile(char* filename, char* buffer, int numberOfSectors) { // make a sys call to kernel 
  interrupt(0x21, 8, filename, buffer, numberOfSectors); // writeFile
}

void readFile(char *filename, char *buffer) { // make a sys call to kernel 
  interrupt(0x21, 3, filename, buffer, 0); // readFile
}

void deleteFile(char* filename) { // make a sys call to kernel 
  interrupt(0x21, 7, filename, 0, 0); // delete file
}

void executeFile(char* filename) { // make a sys call to kernel
  interrupt(0x21, 4, filename, 0x2000, 0); // execute file at 0x2000 segment
}


// ----------- Process control
void kill(int id) {
  interrupt(0x21, 10, id, 0, 0);
}

void exit() { // make a sys call to kernel 
  interrupt(0x21, 5, 0, 0, 0); // exit, then the kernel will reload the shell
}
