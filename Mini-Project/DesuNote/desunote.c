/*
 * desunote.c
 *
 * A touch-screen based note-taking app with handwriting recognition.
 *
 * Created on: 29th April 2022
 *      Author: Thariq Fahry
 * 
 */
#include "desunote.h"
#include "../Graphics/graphics.h"

// Standard library imports.
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// FAT32 driver imports.
// note: FF_USE_FIND needs to be changed to 1 in ffconf.h
// in order to use f_findfirst and f_findnext. This file
// will not compile otherwise.
#include "../CustomProjectDrivers/FatFS/diskio.h"
#include "../CustomProjectDrivers/FatFS/ff.h"

// Drivers.
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_usleep/HPS_usleep.h"
#include "../CustomProjectDrivers/AD7843_TouchScreen/AD7843_TouchScreen.h"
#include "../CustomProjectDrivers/HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "../HelperFunctions/HelperFunctions.h"
#include "uiparams.h"


// Shorthand defines for common functions.
#define cd() LT24_clearDisplay(LT24_BLACK)
#define wt() wait_for_touch()
#define wtr() wait_for_touchrandelease()
#define bg(bgname) LT24_copyFrameBuffer(bgname,0,0,240,320)

bool isFileSystemInitialised = false;
FATFS filesystem;                   // Global filesystem object.

// Function prototypes.
void initfs(void);
int getfiles(void);
void Tutorial(void);
void Files(void);
void Writing(void);

//
// Main app enty point. Only ever call this function from outside this file.
//
int desunote(void) {
    // Call initfs when starting the app, and check its return value to avoid
    // having to check isFileSystemInitialised every time we do anything 
    // file-system related.
    initfs();
    if (!isFileSystemInitialised) { printf("File system failed to initialise.\n"); return 1; }

    // By default, the app goes through the Tutorial, then loops File mode
    // forever, where the user can create and edit notes.
    Tutorial();
    while (1) Files();
    return 0;
}


//
// Tutorial. This runs at the start. Optional.
//
void Tutorial(void) {
    int nameindex = 0;
    char msgbuf[100] = "\0";
    char name[20] = "\0";
    Point touchpos;

    // Display intro graphic.
    bg(title);
    msleep(2000);
    
    // Write initial messages to display.
    bg(single_line_bg);
    writeString("Hello, and welcome to to Desu Note. I'm Kanna-san! I'm a fully-connected feedforward neural network trained on the EMNIST dataset.", 
    0, 0, ASST_XRIGHT, ASST_YBOT, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);
    wtr(); msleep(100);
    bg(single_line_bg);
    writeString("First, tell me your name. Write each letter in the box below!", 
    0, 0, ASST_XRIGHT, ASST_YBOT, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);

    // Get the player's name and store it in the name buffer.
    name[nameindex] = read_handwriting(HANDWR_XLEFT, HANDWR_YTOP, HANDWR_SIZE);  //get first letter; ugly but necessary
    writeString(name, LINEENTRY_LEFTPAD, LINEENTRY_TOP+2, rc, LINEENTRY_BOT, 1, 1, LE_TEXT_COLOUR, INTERFACE_DARK, 0);
    nameindex++;
    while (1) {
        touchpos = wt();
        // if the OK button is pressed, break out of the loop.
        if (within(touchpos.x, INPUTBUTTON_XLEFT, INPUTBUTTON_XLEFT+BUTTON_SIZE) && within(touchpos.y, INPUTBUTTON_YTOP, INPUTBUTTON_YTOP+BUTTON_SIZE)) break;
        // Otherwise, continue printing characters on to the display.
        else {
            name[nameindex] = read_handwriting(HANDWR_XLEFT, HANDWR_YTOP, HANDWR_SIZE);
            writeString(name, LINEENTRY_LEFTPAD, LINEENTRY_TOP+2, rc, LINEENTRY_BOT, 1, 1, LE_TEXT_COLOUR, INTERFACE_DARK, 0);
            nameindex++;
        }
    }
    bg(single_line_bg);

    // Greet player using their name.
    strcpy(msgbuf, "Hi, ");
    strcat(msgbuf, (char*)name);
    strcat(msgbuf, "! Let me show you around.");
    writeString((char*)msgbuf, 0, 0, ASST_XRIGHT, ASST_YBOT, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);
    msleep(100);wtr(); //debounce
    bg(single_line_bg);
}


//
// Files mode, This lists all .txt files on the SD card and allows creation
// of new notes.
//
void Files(void) {
    Point touchpos;
    
    // Draw Files mode UI.
    bg(file_bg);
    writeString("This is the Files mode. Press + to create a note!",
        0, 0, ASST_XRIGHT, ASST_YBOT, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);
    getfiles();

    // Wait for user input.
    while (1) {
        touchpos = wtr();
        if (within(touchpos.x, NEWBUTTON_XLEFT, NEWBUTTON_XLEFT+BUTTON_SIZE) && within(touchpos.y, NEWBUTTON_YTOP, NEWBUTTON_YTOP+BUTTON_SIZE)) {
            msleep(100);
            Writing();
            break;
        }
    }
}


//
// Writing mode. This allows the user to enter text to be stored in a .txt
// file.
//
void Writing(void) {
    char writebuf[100] = "\0";
    char fnamebuf[100] = "\0";
    int writeIndex = 0;
    Point touchpos;

    FRESULT fr;
    FIL file;

    // Draw the text editing interface.
    bg(editor_bg);

    writeString("This is the Writing interface. Write some characters and press the S button to save!",
        0, 0, ASST_XRIGHT, ASST_YBOT, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);

    // Print characters entered by the user onto the screen.
    while (1) {
        touchpos = wt();

        // If the user presses the Save button, break out of the loop and 
        // draw the Save file interface.
        if (within(touchpos.x, INPUTBUTTON_XLEFT, INPUTBUTTON_XLEFT+BUTTON_SIZE) && within(touchpos.y, INPUTBUTTON_YTOP, INPUTBUTTON_YTOP+BUTTON_SIZE)) break;

        // Otherwise, print each character to the editing area.
        else {
            writebuf[writeIndex] = read_handwriting(HANDWR_XLEFT, HANDWR_YTOP, HANDWR_SIZE);
            writeString(writebuf, 10, ASST_YBOT + 10, rc - 10, EDITOR_YBOT, 1, 10, TEXT_COLOUR, EDITOR_BGCOLOUR, 0);
            writeIndex++;
        }
    }
    bg(single_line_bg);

    // Draw the Save File interface.
    writeString("What would you like to call it?", 0, 0, ASST_XRIGHT, ASST_YBOT, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);

    // Print filename entered by the user onto the screen.
    writeIndex = 0;
    while (1) {
        touchpos = wt();
        //If the user presses the OK button, break out of the loop and save the file.
        if (within(touchpos.x, INPUTBUTTON_XLEFT, INPUTBUTTON_XLEFT+BUTTON_SIZE) && within(touchpos.y, INPUTBUTTON_YTOP, INPUTBUTTON_YTOP+BUTTON_SIZE) && writeIndex>0) break;
        // Otherwise, continue adding characters to the filename buffer.
        else {
            fnamebuf[writeIndex] = read_handwriting(HANDWR_XLEFT, HANDWR_YTOP, HANDWR_SIZE);
            writeString(fnamebuf, LINEENTRY_LEFTPAD, LINEENTRY_TOP+2, rc, LINEENTRY_BOT, 1, 1, LE_TEXT_COLOUR, INTERFACE_DARK, 0);
            writeIndex++;
        }
    }

    // Append .txt to the file name and save it to disk.
    //
    // Debug print statements that print the return code of the f_open,
    // f_printf, and f_save operations have been commented out.
    strcat(fnamebuf, ".txt");
    fr = f_open(&file, fnamebuf, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    // printf("%s open fr = %i\n", fnamebuf, (int)fr);
    writeIndex = f_printf(&file, writebuf);
    f_printf(&file, "\n");
    // printf("%s num chars written = %i\n", fnamebuf, writeIndex);
    fr = f_close(&file);
    // printf("%s close fr = %i\n", fnamebuf, (int)fr);
}


//
// Utility functions.
//

//Function to initialise the file system, if not already.
void initfs(void) {
    FRESULT fr;
    //Initialise the file system, if not already
    if (!isFileSystemInitialised) {
        fr = f_mount(&filesystem, "", 0);
        if (!fr) isFileSystemInitialised = true;
        printf("mounted? = %i\n", (int)fr);
    }
}

// Get all first notes in root directory, and print to LCD.
int getfiles(void) {
    DIR dir;
    FILINFO info;
    FRESULT fr;
    int filenum = 0;

    // Find all files with a *.txt extension in the "/" directory.
    // In this case, "/" = root of the SD card.
    fr = f_findfirst(&dir, &info, "/", "*.txt");

    // Loop through all files in the root directory and print their names to
    // the LCD.
    while (fr == FR_OK && info.fname[0]) {
        if (filenum % 2) writeString((char*)info.fname, 30, ASST_YBOT+2 + filenum * 10, rc, bc, 1, 1, TEXT_COLOUR, EDITOR_BGCOLOUR,TEXT_SPEED);
        else             writeString((char*)info.fname, 30, ASST_YBOT+2 + filenum * 10, rc, bc, 1, 1, TEXT_COLOUR, ASSTBG_COLOUR, TEXT_SPEED);
        fr = f_findnext(&dir, &info);
        filenum++;
    }
    f_closedir(&dir);

    // Return the number of .txt files found.
    return filenum;
}
