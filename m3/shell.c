/*
 * Hello World program.
 * Author: Ishank Tandon, Fred Zhang, Jackie Zhang, Runzhi Yang
 */

void handleInput(char *name);
int compareStr(char *a, char *b);

int main() {
  char prompt[4];
  char badCommand[13];
  char buffer[512];
  char newLine[3];

  prompt[0] = 'A';
  prompt[1] = ':';
  prompt[2] = '>';
  prompt[3] = '\0';

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

  newLine[0] = '\r';
  newLine[1] = '\n';
  newLine[2] = '\0';

  while (1) {
    interrupt(0x21, 0, prompt, 0, 0);
    interrupt(0x21, 1, buffer, 0, 0);
    interrupt(0x21, 0, buffer, 0, 0);

    handleInput(buffer);

    interrupt(0x21, 0, badCommand, 0, 0);
    interrupt(0x21, 0, newLine, 0, 0);

    interrupt(0x21, 5, 0, 0, 0);
  }
}

void handleInput(char *input) {
  char buf[13312];
  char commandType[5];
  char commandName[512];
  char arg[512];
  int pointer = 0;

  commandType[0] = 't';
  commandType[1] = 'y';
  commandType[2] = 'p';
  commandType[3] = 'e';
  commandType[4] = '\0';

  while (*input != ' ' && *input != '\0') {
    commandName[pointer] = *input;
    input++;
    pointer++;
  }
  commandName[pointer] = '\0';

  pointer = 0;
  while (*input != '\0') {
    arg[pointer] = *input;
    input++;
    pointer++;
  }

  interrupt(0x21, 0, commandName, 0, 0);
  if (compareStr(commandName, commandType)) {
    interrupt(0x21, 3, arg, buf, 0);
    interrupt(0x21, 0, buf, 0, 0);
  }

}

int compareStr(char *a, char *b) {
  while(*a == *b) {
    if (*a == '\0' || *a == '\3' || *a == '\r' || *a == '\n') {
      return 1;
    }
    a++;
    b++;
  }
  return 0;
}
