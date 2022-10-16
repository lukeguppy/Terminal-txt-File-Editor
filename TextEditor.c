#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int readLine(char*** strArray) {
    int wordLength = 0, newWord = 1, c, numOfStrings = 0;
    char** inputs = calloc(1, sizeof(char*));
    *strArray = calloc(1, sizeof(char*));

    while((c = getchar()) != '\n' && c != EOF) {
        if (c != ' ') {
            if (newWord) { 
                wordLength = 0; 
                numOfStrings++;
                newWord = 0; // now not a new word
                // increase the memory allocation
                char** temp = realloc(inputs, (numOfStrings) * sizeof(char*)); 
                inputs = temp;
                // allocate new memory for new string
                inputs[numOfStrings - 1] = (char*) calloc(2, sizeof(char));
            } else { // increase memory allocation for current string
                inputs[numOfStrings - 1] = (char*) realloc(inputs[numOfStrings - 1], (wordLength + 2) * sizeof(char));
            }
            //set char of current string and set end to null terminator
            inputs[numOfStrings - 1][wordLength] = c;
            inputs[numOfStrings - 1][wordLength + 1] = '\0';
            wordLength++;
        } else if (wordLength > 0) newWord = 1;
    }

    // set the array given to the read input
    *strArray = realloc(*strArray, (numOfStrings) * sizeof(char*));
    *strArray = inputs;
    // return number of strings read successfully
    
    return numOfStrings;
}

int istxtFile(char* fileName) {
    char* end = calloc(5, sizeof(char));
    if (sizeof(fileName) < 6 * sizeof(char)) return 0; // 6 chars min so it must include more than ".txt" with null terminator 
    for (int i = 0; i < 4; i++) end[i] = fileName[strlen(fileName) - 4 + i]; // set end to last 4 chars of file name
    if (!strcmp(".txt", end)) return 1; // compare end to .txt to see if it is a text file
    return 0;
}

int fileExists(char* fileName) {
    FILE *file;
    if ((file = fopen(fileName, "r"))) { // attempt to open
        // opened successfully
        fclose(file);
        return 1;
    }
    return 0; // failed to open
}

int isSystemFile(char* fileName) {
    // compare file name to given files needed to execute commands and output message if matching
    if (!strcmp("temp.txt", fileName) || !strcmp("undo.txt", fileName)) {
        printf("Cannot use file '%s'", fileName);
        return 1;
    }
    return 0;
}

int naturalNum(char* str) {
    int i = 0, result = 0;
    while (str[i] != '\0') { // until end of string
        if (!isdigit(str[i])) return -1; // if not a digit return -1
        result *= 10; // left denary shift result by 1 digit
        result += (int) str[i] - '0'; // add integer version of char to the result 
        i++; // next char index
    }
    return result; // return natural number
}

int numlines(char* fileName) {
    FILE *file;
    int numOfLines = 1; // start at first line
    char temp;

    if (!fileExists(fileName)) { // cant use existing files
        printf("File doesn't exist\n");
        return 0;
    }

    if (!istxtFile(fileName)) { // only applies to txt files
        printf("Text file name not given\n");
        return 0;
    }

    file = fopen(fileName, "r"); // only need to read the data

    while ((temp = fgetc(file)) != EOF) {
        if (temp == '\n') numOfLines++; // increase the number of lines for every
    }

    fclose(file);
    return numOfLines;
}

void showCommands() {
    // prints available commands for user 
    printf("createfile fileName.txt                          - create a new file\n");
    printf("copyfile sourceName.txt targetName.txt           - copy a source file to a target file (removes contents of target file)\n");
    printf("deletefile fileName.txt                          - deletes a given file\n");
    printf("showfile fileName.txt                            - displays a given file to the terminal\n");
    printf("appendline fileName.txt contents...              - appends a line to a given file\n");
    printf("deleteline fileName.txt lineNumber               - deletes a line from a given file\n");
    printf("insertline fileName.txt lineNumber contents...   - inserts a line at a given position to the file and shifts the rest of the lines\n");
    printf("showline fileName.txt lineNumber                 - shows a specific line of a given file\n");
    printf("showlog                                          - shows the log of changes made to files\n");
    printf("numlines fileName.txt                            - shows the number of lines for a given file\n");
    printf("appendtoline fileName.txt lineNumber contents... - shows the number of lines for a given file\n");
    printf("undo                                             - undoes last change to a file (does not apply to creating a file)\n");
    printf("exit                                             - exits terminal\n");
}

int createfile(char* fileName) {
    FILE *file;
    if (fileExists(fileName)) { // cant create a file which already exists
        printf("File already exists\n");
        return 0;
    }

    if (!istxtFile(fileName)) { // can only create txt files
        printf("Text file name not given\n");
        return 0;
    }

    if (isSystemFile(fileName)) return 0; // cannot create files needed by the program for functions

    file = fopen(fileName, "w"); // opening file creates it
    fclose(file); // nothing needs to be written so closed
    return 1;
}

int copyfile(char* sourceName, char* targetName, int allowSystemFiles) {
    FILE *source, *target;
    char temp;

    if (!fileExists(sourceName) && !allowSystemFiles) { // cant copy from a file which doesnt exist unless creating it when undoing
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(sourceName)) { // Only applies to txt files
        printf("Source file name not given\n");
        return 0;
    }

    if (!allowSystemFiles && (isSystemFile(targetName) || isSystemFile(sourceName))) return 0; // cant use system files unless system file permission given

    if (!istxtFile(targetName)) { // Only applies to txt files
        printf("Target file name not given\n");
        return 0;
    }

    // if not copying to undo.txt then copy to undo.txt so command can be undone
    if (strcmp(targetName, "undo.txt") && fileExists(targetName)) copyfile(targetName, "undo.txt", 1);

    if (fileExists(targetName)) remove(targetName); // remove target to clear it

    source = fopen(sourceName, "r"); // read from source
    target = fopen(targetName, "w"); // write to target

    while ((temp = fgetc(source)) != EOF) fprintf(target, "%c", temp); // print each char from source to target

    fclose(source);
    fclose(target);
    return 1;
}

int deletefile(char* fileName) {
    FILE *file;

    if (isSystemFile(fileName)) return 0; // cant delete system files needed for commands

    if (!istxtFile(fileName)) { // Only applies to txt files
        printf("Source file name not given\n");
        return 0;
    }

    if (fileExists(fileName)) {
        copyfile(fileName, "undo.txt", 1); // copy to undo.txt so command can be undone
        remove(fileName); // delete
        return 1; // success
    } else {
        printf("File doesn't exist\n"); // cant delete a file which doesnt exist
        return 0; // fail
    }
}

int showfile(char* fileName) {
    FILE *file;
    char temp;

    if (!fileExists(fileName)) { // cant show a file which doesnt exist
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(fileName)) { // only applies to txt files
        printf("Source file name not given\n");
        return 0;
    }

    file = fopen(fileName, "r"); // only need to read file

    while ((temp = fgetc(file)) != EOF) printf("%c", temp); // print each char
    printf("\n"); // new line at EOF
    fclose(file);
    return 1;
}

int appendline(char** input, int numOfInputs) {
    FILE *file;
    char temp;
    int index = 2;

    if (!fileExists(input[1])) { // cant append to non existent files
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(input[1])) { // can only use txt files
        printf("Source file name not given\n");
        return 0;
    }

    if (isSystemFile(input[1])) return 0; // cant write to system files

    copyfile(input[1], "undo.txt", 1); // copy to undo.txt so command can be undone
    file = fopen(input[1], "a"); // open file in append mode 
    fprintf(file, "\n"); // new line so it doesnt add to end of last line

    while (index < numOfInputs) { // for rest of strings read from terminal
        fprintf(file, input[index]); // print string to new line
        if (index < numOfInputs - 1) fprintf(file, " "); // spaces after all words except the last
        index++;
    }

    fclose(file);
    return 1;
}

int deleteline(char* fileName, char* lineNumber) {
    FILE *file, *tempFile;
    char tempChar;
    int wordIndex = 3, lineIndex = 1, lineNum = 1;
    
    if (!fileExists(fileName)) { // cannot delete from files which dont exist
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(fileName)) { // can only work on txt files
        printf("Source file name not given\n");
        return 0;
    }

    if (isSystemFile(fileName)) return 0; // cant delete from system files

    if ((lineNum = naturalNum(lineNumber)) == -1) { // checks the given line number is a natural number
        printf("Line number not given");
        return 0;
    }
    
    file = fopen(fileName, "r+"); // read and write permissions
    tempFile = fopen("temp.txt", "w"); // write to temp file

    while ((tempChar = fgetc(file)) != EOF) { // get each char until EOF
        if (tempChar == '\n') lineIndex++; // line index increases for every new line char
        if (lineIndex != lineNum) fprintf(tempFile, "%c", tempChar); // print char unless at 'delete line'
    }

    fclose(file);
    fclose(tempFile);

    if (lineIndex < lineNum) { // if it didnt reach the delete line then cant delete given line
        printf("Line doesn't exist in file\n");
        return 0;
    }

    copyfile(fileName, "undo.txt", 1); // copy to undo.txt so command can be undone
    copyfile("temp.txt", fileName, 1); // copy temp file with deleted line to the original
    remove("temp.txt"); // remove the unnecessary temp file
    return 1;
}

int insertline(char** input, int numOfInputs) {
    FILE *file, *tempFile;
    char tempChar;
    int inputIndex = 3, lineIndex = 1, lineNum;
    
    if (!fileExists(input[1])) { // cant write to a file which doesnt exist
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(input[1])) { // only applies to txt files
        printf("Source file name not given\n");
        return 0;
    }

    if (isSystemFile(input[1])) return 0; // cannot write to system files

    if ((lineNum = naturalNum(input[2])) == -1) { // checks the line number given is a natural number 
        printf("Line number not given");
        return 0;
    }
    
    file = fopen(input[1], "r+"); // open in read and write mode
    tempFile = fopen("temp.txt", "w"); // open temp file in read mode

    tempChar = fgetc(file); // get first char
    do {
        if (lineIndex == lineNum) { // dont write the char if it is on the 'insert line'
            while (inputIndex < numOfInputs) { // for each string given from user input
                fprintf(tempFile, input[inputIndex]);// write the string to temp file
                if (inputIndex < numOfInputs - 1) fprintf(tempFile, " "); // space after each string unless last string
                inputIndex++; // next string
            }
            fprintf(tempFile, "\n"); // new line
            lineIndex++; // register written line
        } else {
            fprintf(tempFile, "%c", tempChar); // write char from original file to temp file
            if (tempChar == '\n') lineIndex++; // if new line increase line index
            tempChar = fgetc(file); // get the next char ready
        }
    } while (tempChar != EOF); // end if EOF reached

    fclose(file);
    fclose(tempFile);

    if (lineIndex < lineNum) { // line number bigger than num of lines in file
        printf("Line doesn't exist in file\n");
        remove("temp.txt");
        return 0;
    }

    copyfile(input[1], "undo.txt", 1); // copy to undo.txt so command can be undone
    copyfile("temp.txt", input[1], 1); // copy temp file to original
    remove("temp.txt"); // remove unnecessary temp file
    return 1;
}

int showline(char* fileName, char* lineNumber) {
    FILE *file;
    char tempChar;
    int wordIndex = 3, lineIndex = 1, lineNum;
    
    if (!fileExists(fileName)) { // cannot show line if file doesnt exist
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(fileName)) { // can only show lines of txt files
        printf("Source file name not given\n");
        return 0;
    }

    if ((lineNum = naturalNum(lineNumber)) == -1) { // checks line number given is actually a natural number 
        printf("Line number not given");
        return 0;
    }
    
    file = fopen(fileName, "r"); // only needs to read from file
    while ((tempChar = fgetc(file)) != EOF) { // until EOF
        if (lineIndex == lineNum) printf("%c", tempChar); // if on the correct line print the char
        if (tempChar == '\n') lineIndex++; // increase line index for new line char
    }

    if (lineIndex == lineNum) printf("\n"); // print new line char as not included in line for last line of file
    else if (lineIndex < lineNum) { // if the line doesnt exist in file
        printf("Line doesn't exist in file\n");
        return 0;
    }

    fclose(file);
    return 1;
}

int appendtoline(char** input, int numOfInputs) {
    FILE *file, *tempFile;
    char tempChar;
    int inputIndex = 3, lineIndex = 1, lineNum;
    
    if (!fileExists(input[1])) { // can only write to existing files
        printf("Source doesn't exist\n");
        return 0;
    }

    if (!istxtFile(input[1])) { // only applies to txt files
        printf("Source file name not given\n");
        return 0;
    }

    if (isSystemFile(input[1])) return 0; // cannot write to system files

    if ((lineNum = naturalNum(input[2])) == -1) { // checks given line num input is a natural num and converts
        printf("Line number not given");
        return 0;
    }
    
    copyfile(input[1], "undo.txt", 1); // copies original file to undo.txt so it can be undone
    file = fopen(input[1], "r+"); // file opened in read and write mode
    tempFile = fopen("temp.txt", "w"); // only need to write to temp file

    tempChar = fgetc(file); // get first char from file
    while (tempChar != EOF) { // until EOF
        if (lineIndex == lineNum) { // if on 'append line'
            while (tempChar != '\n' && tempChar != EOF) { // until end of line or end of file
                fprintf(tempFile, "%c", tempChar); // write original char to temp
                tempChar = fgetc(file); // get next char
            }
            while (inputIndex < numOfInputs) { // for each string given as input
                fprintf(tempFile, " "); // space assumed from original line and before each string from input
                fprintf(tempFile, input[inputIndex]); // print input string to temp
                inputIndex++; // next input string
            }
            lineIndex++; // next line
        } else {
            fprintf(tempFile, "%c", tempChar); // print the current char to the temp file
            if (tempChar == '\n') lineIndex++; // increase line index for each new line char
            tempChar = fgetc(file); // next char from original file
        }
    }

    fclose(file);
    fclose(tempFile);

    if (lineIndex == lineNum) printf("\n"); // file is shorter than the given line
    else if (lineIndex < lineNum) {
        printf("Line doesn't exist in file\n");
        remove("temp.txt");
        return 0;
    }

    copyfile(input[1], "undo.txt", 1); // copies original file to undo.txt so it can be undone
    copyfile("temp.txt", input[1], 1); // copy back to original
    remove("temp.txt"); // delete unnecessary temp file
    return 1;
}

int undo(char* previousFile) {
    FILE *undo;
    if (previousFile == NULL) { // no changes made yet so prev file is NULL
        printf("Nothing to undo\n");
        return 0;
    }

    // switch undo.txt and previous file 
    copyfile("undo.txt", "temp.txt", 1);
    copyfile(previousFile, "undo.txt", 1);
    copyfile("temp.txt", previousFile, 1);
    remove("temp.txt"); // remove unnecessary temp
    return 1;
}

void logLineFromIndex(char** line, int strIndex, int totalNumOfWords) {
    FILE *log;
    log = fopen("log.txt", "a"); // append to log file
    while (strIndex < totalNumOfWords) { // for each string in input (from indexed string)
        fprintf(log, line[strIndex]); // print string to log
        if (strIndex < totalNumOfWords - 1) fprintf(log, " "); // space after each string except the last
        else fprintf(log, "\n"); // new line at end of inputs
        strIndex++; // next string index
    }
    fprintf(log, "New number of lines in file '%s': %d\n", line[1], numlines(line[1])); // print updated line num
    fclose(log);
} 

void inputTerminal(char* previousFile) {
    char** inputs = calloc(1, sizeof(char*));
    int numOfInputs, status;
    FILE *log;

    status = 1; // status whether to exit terminal editor or not
    printf("> "); // aesthetic to make commands given apparent
    numOfInputs = readLine(&inputs); // read terminal line to inputs and record the number of input strings

    if (numOfInputs > 0) { // only if atleast one string was given
        // compare first input string to command names
        if (!strcmp(inputs[0], "commands")) showCommands(); // display commands
        else if (!strcmp(inputs[0], "createfile")) {
            if (numOfInputs > 1) { // if enough inputs given to create a file
                if(createfile(inputs[1])) { // if successful creation of file
                    // print report to terminal and to log
                    printf("Created file '%s'\n", inputs[1]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nCreated file '%s'\n", inputs[1]);
                    fprintf(log, "Number of lines in file '%s': 0\n", inputs[1]);
                    fclose(log);
                }
            } else printf("File not given\n");
        } else if (!strcmp(inputs[0], "copyfile")) {
            if (numOfInputs > 2) { // if enough inputs given to copy
                if(copyfile(inputs[1], inputs[2], 0)) {
                    // print report to terminal and to log
                    printf("Copied file '%s' to '%s'\n", inputs[1], inputs[2]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nCopied file '%s' to '%s'\n", inputs[1], inputs[2]);
                    fprintf(log, "New number of lines in file '%s': %d\n", inputs[2], numlines(inputs[2]));
                    fclose(log);
                    // set previous file to edited file
                    previousFile = malloc(sizeof(inputs[1]));
                    strcpy(previousFile, inputs[2]);
                }
            } else printf("Missing input parameter\n");
        } else if (!strcmp(inputs[0], "deletefile")) {
            if (numOfInputs > 1) { // if enough inputs given to delete
                if(deletefile(inputs[1])) {
                    // print report to terminal and to log
                    printf("Deleted file '%s'\n", inputs[1]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nDeleted file '%s'\n", inputs[1]);
                    fclose(log);
                    // set previous file to edited file
                    previousFile = malloc(sizeof(inputs[1]));
                    strcpy(previousFile, inputs[1]);
                }
            } else printf("File not given\n");
        } else if (!strcmp(inputs[0], "showfile")) {
            if (numOfInputs > 1) { // if enough inputs given to show a file
                showfile(inputs[1]);
            } else printf("File not given\n");
        } else if (!strcmp(inputs[0], "appendline")) {
            if (numOfInputs > 2) { // if enough inputs given to append a line to a file
                if(appendline(inputs, numOfInputs)) {
                    // print report to terminal and to log
                    printf("Appended line to '%s'\n", inputs[1]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nAppended line (to '%s'): ", inputs[1]);
                    fclose(log);
                    logLineFromIndex(inputs, 2, numOfInputs);
                    fprintf(log, "New number of lines in file '%s': %d\n", inputs[1], numlines(inputs[1]));
                    // set previous file to edited file
                    previousFile = malloc(sizeof(inputs[1]));
                    strcpy(previousFile, inputs[1]);
                }
            } else printf("Missing input parameter\n");
        } else if (!strcmp(inputs[0], "deleteline")) {
            if (numOfInputs > 2) { // if enough inputs given to delete a line from a file
                if(deleteline(inputs[1], inputs[2])) {
                    // print report to terminal and to log
                    printf("Deleted line '%s' from '%s'\n", inputs[2], inputs[1]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nDeleted line '%s' from '%s'\n", inputs[2], inputs[1]);
                    fprintf(log, "New number of lines in file '%s': %d\n", inputs[1], numlines(inputs[1]));
                    fclose(log);
                    // set previous file to edited file
                    previousFile = malloc(sizeof(inputs[1]));
                    strcpy(previousFile, inputs[1]);
                }
            } else printf("Missing input parameter\n");
        } else if (!strcmp(inputs[0], "insertline")) {
            if (numOfInputs > 2) { // if enough inputs given to insert a line to a file
                if(insertline(inputs, numOfInputs)) {
                    // print report to terminal and to log
                    printf("Inserted line to '%s' at line %s\n", inputs[1], inputs[2]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nInserted at line %s (to '%s'): ", inputs[2], inputs[1]);
                    fclose(log);
                    logLineFromIndex(inputs, 3, numOfInputs);
                    // set previous file to edited file
                    previousFile = malloc(sizeof(inputs[1]));
                    strcpy(previousFile, inputs[1]);
                }
            } else printf("Missing input parameter\n");
        } else if (!strcmp(inputs[0], "showline")) {
            if (numOfInputs > 2) showline(inputs[1], inputs[2]); // if enough inputs given to show a line
            else printf("Missing input parameter\n");
        } else if (!strcmp(inputs[0], "showlog")) {
            showfile("log.txt");
        } else if (!strcmp(inputs[0], "numlines")) {
            if (numOfInputs > 1) printf("%d\n", numlines(inputs[1])); // if enough inputs given to return the number of lines
            else printf("File not given\n"); // otherwise file not given
        } else if (!strcmp(inputs[0], "appendtoline")) {
            if (numOfInputs > 2) { // if enough inputs given to append to a line
                if(appendtoline(inputs, numOfInputs)) {
                    // print report to terminal and to log
                    printf("Appended to line '%s' in '%s'\n", inputs[2], inputs[1]);
                    log = fopen("log.txt", "a");
                    fprintf(log, "\nAppended to line '%s' in '%s': ", inputs[2], inputs[1]);
                    fclose(log);
                    logLineFromIndex(inputs, 2, numOfInputs);
                    // set previous file to edited file
                    previousFile = malloc(sizeof(inputs[1]));
                    strcpy(previousFile, inputs[1]);
                }
            } else printf("Missing input parameter\n");
        } else if (!strcmp(inputs[0], "undo")) {
            if (undo(previousFile)) {
                // print report to terminal and to log
                printf("Previous command undone\n");
                log = fopen("log.txt", "a");
                fprintf(log, "\nPrevious command undone\n");
                fprintf(log, "New number of lines in file '%s': %d\n", previousFile, numlines(previousFile));
                fclose(log);
            }
        } else if (!strcmp(inputs[0], "exit")) {
            status = 0; // status 0 to exit terminal editor
            remove("undo.txt"); // remove undo file as not needed
        }
        else printf("Command '%s' unknown\n", inputs[0]); // if reached then command not recognised
    } 
    
    for (int i = 0; i < numOfInputs; i++) free(inputs[i]); // free memory for each string in input
    free(inputs); // free pointer of inputs array
    if (status) inputTerminal(previousFile); // if not exiting editor then recurse for next command
    return; // leave editor terminal
}

int main() {
    FILE *undo;
    if (!fileExists("undo.txt")) { // create undo.txt if it doesnt exist
        undo = fopen("undo.txt", "w"); // creates undo.txt
        fclose(undo);
    }
    if (fileExists("temp.txt")) { // will over-write temp.txt if it exists so exit
        printf("Cannot run terminal while file 'temp.txt' exists\n");
        return 0;
    }
    printf("Type 'commands' to view commands\n"); // intro so the user can type 'commands' to see available commands
    inputTerminal(NULL); // call terminal editor with no previous file to edit
    return 0;
} 