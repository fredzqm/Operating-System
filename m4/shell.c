/*
 * Hello World program.
 * Author: Ishank Tandon, Fred Zhang, Jackie Zhang, Runzhi Yang
 */

void handleInput(char *name);
int compareStr(char *a, char *b);
void copyFile(char* filename1, char* filename2);
int sizeOfSector(char* filename);
void convertIntToString(char* buffer, int);
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

  deliminator[0] = ' ';
  deliminator[1] = '-';
  deliminator[2] = ' ';
  deliminator[3] = '\0';

  newLine[0] = '\r';
  newLine[1] = '\n';
  newLine[2] = '\0';

  while (1) {
    interrupt(0x21, 0, prompt, 0, 0); // print the prompt
    interrupt(0x21, 1, buffer, 0, 0); // read the instruction
    interrupt(0x21, 0, newLine, 0, 0); // print a new line
    handleInput(buffer); // handle the command
    interrupt(0x21, 0, newLine, 0, 0); // print a new line
    interrupt(0x21, 5, 0, 0, 0); // exit, then the kernel will reload the shell
  }
}

void handleInput(char *input) {
  char buf[13312];
  char* bufPointer = buf;
  char commandName[512];
  char commandArg[512];
  char commandType[5];
  char commandDelete[7];
  char commandExecute[8];
  char commandCreate[7];
  char commandCopy[5];
  char commandDir[4];
  char promptLine[3];
  char badCommand[13];
  int pointer = 0;

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

  commandType[0] = 't';
  commandType[1] = 'y';
  commandType[2] = 'p';
  commandType[3] = 'e';
  commandType[4] = '\0';

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

  promptLine[0] = '>';
  promptLine[1] = ' ';
  promptLine[2] = '\0';

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
    interrupt(0x21, 3, commandArg, buf, 0); // read file
    interrupt(0x21, 0, buf, 0, 0);  // print file
  } else if (compareStr(commandName, commandExecute) == 1) {
    interrupt(0x21, 4, commandArg, 0x2000, 0); // execute file
  } else if (compareStr(commandName, commandDelete) == 1) {
    interrupt(0x21, 7, commandArg, 0, 0); // delete file
  } else if (compareStr(commandName, commandCreate) == 1) {
    int count = 0;
    int readStringSize = 0;
    interrupt(0x21, 0, promptLine, 0, 0); // print prompt 
    interrupt(0x21, 1, buf + count, &readStringSize, 0); // read input
    while(readStringSize != 0) {
      count += readStringSize + 2;
      interrupt(0x21, 0, newLine, 0, 0); // print a new
      interrupt(0x21, 0, promptLine, 0, 0); // print prompt
      interrupt(0x21, 1, buf + count, &readStringSize, 0); // continue to read line
    }
    buf[count - 2] = '\0';
    interrupt(0x21, 8, commandArg, buf, count/512 + 1); // write to file
  } else if (compareStr(commandName, commandCopy) == 1) {
    char arg1[512];
    char arg2[512];
    int ptr = 0;
    int ptrArg2 = 0;

    // interrupt(0x21, 0, commandArg, 0, 0);
    // interrupt(0x21, 0, newLine, 0, 0);

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
    // interrupt(0x21, 0, arg1, 0, 0);
    // interrupt(0x21, 0, deliminator, 0, 0);
    // interrupt(0x21, 0, arg2, 0, 0);
    copyFile(arg1, arg2);
  } else if (compareStr(commandName, commandDir) == 1) {
    int entry, flag;
    char directoryBuffer[512];
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
      interrupt(0x21, 0, filename, 0, 0);
      interrupt(0x21, 0, deliminator, 0, 0);
      interrupt(0x21, 0, number, 0, 0);
      interrupt(0x21, 0, newLine, 0, 0);
    }
  } else {
    // int t;
    // int TEST_SIZE = 1000;
    // char tempBuf[1001];
    // char filename[5];
    // filename[0] = 't';
    // filename[1] = 'e';
    // filename[2] = 's';
    // filename[3] = 't';
    // filename[4] = '\0';
    // for (t = 0; t < TEST_SIZE; t++) {
    //   tempBuf[t] = 48 + mod(t, 10);
    // }
    // tempBuf[TEST_SIZE] = '\0';
    // interrupt(0x21, 8, filename, tempBuf, 2); // writeFile

    interrupt(0x21, 0, badCommand, 0, 0);
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

void copyFile(char* filename1, char* filename2) {
  char buffer[13312];
  char num[10];
  interrupt(0x21, 3, filename1, buffer, 0); // readFile
  interrupt(0x21, 8, filename2, buffer, sizeOfSector(filename1)); // writeFile
}

int sizeOfSector(char* filename) {
  int i, j, count;
  char directoryBuffer[512];
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

int mod(int a, int b) {
  while (a >= b) {
    a = a - b;
  }
  return a;
}