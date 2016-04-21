void printString(char *chars);
void readString(char *chars, int* readSize);
void writeFile(char* filename, char* buffer, int numberOfSectors);
void readFile(char *filename, char *buffer);
void deleteFile(char* filename);
void executeFile(char* filename);
void exit();


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
void exit() { // make a sys call to kernel 
  interrupt(0x21, 5, 0, 0, 0); // exit, then the kernel will reload the shell
}
