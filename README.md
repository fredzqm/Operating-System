Team-2D
Team members: tandoni yangr zhangq2 zhangx2

# Usage

Executables files can be built with the make command.
make -- creates everything, and loads into floppya.img
After you have all the components built, you can use "make run" to run the program. So the commands to enter are "make", followed by "make run".

Alternatively, you can build each component one at a time as well.
make bootload -- create bootload
make kernel -- crate kernel
make loadFile -- create loadFile
make shell -- create shell
make load -- load files into floppya.img

After you finish testing, you can run
make clean -- cleans assembly and object files

# What we did

## Milestone 3
Here's what we did for Milestone 3:
  * Added the following functionality in kernel.c:
    void readFile(char *filename, char *buffer);
    void executeProgram(char* name, int segment);
    void terminate();
  * Created shell.c and let shell handle user's input.
  * Added the following 0x21 interrupts
    ax = 3: Read file
    ax = 4: Execute program
    ax = 5: Terminate, which reloads shell

To verify, first build the executables. After you run `make run`, you should see the box.
Now do the following:
  * Input "this" and it should tell you that you entered a bad command
  * Input "type" with no arguments and it should print an empty string
  * Input "type messag" and it should print the contents in message.txt
  * Input "execute tstpr2" and it should execute the tstpr2
  * Input "execute tstprg" and it should execute the tstprg and never exit

## Milestone 4
Here's what we did for Milestone 4:
  * Added the following functionality in kernel.c:
    void writeFile(char* name, char* buffer, int numberOfSectors);
    void deleteFile(char* name);
  * Added the following 0x21 interrupts
    ax = 6: Write to sector
    ax = 7: Delete file
    ax = 8: Write file
  * Added functions in shell.c.
To verify the functions in shell.c, first build the executables according to the "Usage" section.
After you run `make run`, you should see the box.
Now do the following:
  * Input "copy messag m2", and then input "dir", you should see the file 'm2' at the end of the list.
  * Input "type m2" and you should see the contents of file `messag` and they should be the same.
  * Input "create test", you should now see a prompt "> ". Input the following:
    > 123
    > 456
    > 7
    >
    Now, if you input "dir", you should see a file named `test` at the end of the list.
    If you input "type test", you should see the contents of it which is what you entered.
  * Input "copy shell s", and input "execute s" and you should be in another shell program.
    Now input "dir", you should see a file named `s` that has the same number of sectors as `shell` has.


## Milestone 5
Added multiprocessing functionality to the current OS. We also implemented Process Blocking and Management. We also added special features like quit, clear, ps, help, print size of dir, cat, and rm.

# Troubleshooting
1. Check if buffer size is appropriate
2. Check if index is off by 1, or index never increments
3. Check if floppya.img is changed
