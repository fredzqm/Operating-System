/*
 * Hello World program.
 * Author: Ishank Tandon, Fred Zhang, Jackie Zhang, Runzhi Yang
 */

void handleInput(char *name);
int compareStr(char *a, char *b);
void copyFile(char* filename1, char* filename2);
int sizeOfSector(char* filename);

char newLine[3];

int main() {
  char prompt[4];
  char buffer[512];
  // char newLine[3];

  prompt[0] = 'A';
  prompt[1] = ':';
  prompt[2] = '>';
  prompt[3] = '\0';


  newLine[0] = '\r';
  newLine[1] = '\n';
  newLine[2] = '\0';

  while (1) {
    interrupt(0x21, 0, prompt, 0, 0); // give prompt
    interrupt(0x21, 1, buffer, 0, 0); // read instruction
    // interrupt(0x21, 0, buffer, 0, 0);
    interrupt(0x21, 0, newLine, 0, 0);
    handleInput(buffer);
    interrupt(0x21, 0, newLine, 0, 0);
    interrupt(0x21, 5, 0, 0, 0);
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
  char promptLine[3];
  char badCommand[13];
  char commandCopy[5];
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
    interrupt(0x21, 3, commandArg, buf, 0);
    interrupt(0x21, 0, buf, 0, 0);
  } else if (compareStr(commandName, commandExecute) == 1) {
    interrupt(0x21, 4, commandArg, 0x2000, 0);
  } else if (compareStr(commandName, commandDelete) == 1) {
    interrupt(0x21, 7, commandArg, 0, 0);
  } else if (compareStr(commandName, commandCreate) == 1) {
    int count = 0;
    int readStringSize = 0;
    interrupt(0x21, 0, promptLine, 0);
    interrupt(0x21, 1, buf + count, &readStringSize, 0);
    while(readStringSize != 0) {
      count += readStringSize + 2;
      interrupt(0x21, 0, newLine, 0);
      interrupt(0x21, 0, promptLine, 0);
      interrupt(0x21, 1, buf + count, &readStringSize, 0);
    }
    buf[count - 2] = '\0';
    interrupt(0x21, 8, commandArg, buf, count/512 + 1);
  } else if (compareStr(commandName, commandCopy) == 1) {
    char arg1[6];
    char arg2[6];
    int ptr = 0;
    int ptrArg2 = 0;
    while(commandArg[ptr] != ' ') {
      arg1[ptr] = commandArg[ptr];
      ptr++;
    }
    ptr++;
    while(commandArg[ptr] != '\0') {
      arg2[ptrArg2] = commandArg[ptr];
      ptr++;
      ptrArg2++;
    }
    copyFile(arg1, arg2);
  } else {
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
  interrupt(0x21, 4, filename1, buffer);
  interrupt(0x21, 8, filename2, buffer, sizeOfSector(filename1));
}

int sizeOfSector(char* filename) {
    char buffer[13312];
    int i = 0;
    interrupt(0x21, 4, filename, buffer);
    while(buffer[i] != '\0') {
      i++;
    }
    return i/512 + 1;
}


