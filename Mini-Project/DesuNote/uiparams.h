/*
 * uiparams.h
 *
 * Various parameter defines for the GUI used in the Desu Note app.
 *
 * Created on: 29th April 2022
 *      Author: Thariq Fahry
 * 
 */
#ifndef UIPARAMS_H
#define UIPARAMS_H


// Parameters for writing text to the screen.
#define                     TEXT_SPEED          2    // msleep value between writing characters during writeString()
#define rc                  LT24_WIDTH-1             // Right edge of display.
#define bc                  LT24_HEIGHT-1            // Bottom edge of display.

#define ASST_XRIGHT         169     // Right edge of the Assistant's text region.
#define ASST_YBOT           64      // Bottom edge of the Assistant's text region.

#define HANDWR_XLEFT        62     // Left edge of the handrwiting square.
#define HANDWR_YTOP         169    // Top edge of the handrwiting square.
#define HANDWR_SIZE         116     // Height & width of the handrwiting square.

#define LINEENTRY_TOP       110     // Top of the single-line entry region.
#define LINEENTRY_BOT       122     // Bottom of the single-line entry region.
#define LINEENTRY_LEFTPAD   20

#define EDITOR_YBOT         154     //Bottom of the last line of the text editor.

#define INPUTBUTTON_XLEFT   186     // Left edge of the button used in Name entry, Writing, and Save mode.
#define INPUTBUTTON_YTOP    205     // Top edge of the button used in Name entry, Writing, and Save mode.
#define NEWBUTTON_XLEFT     167     // Left edge of the button used in File mode.
#define NEWBUTTON_YTOP      249     // Top edge of the button used in File mode.
#define BUTTON_SIZE         42      // Height & width of buttons.

#define ASSTBG_COLOUR       0xFF1E  // Assistant text background.
#define EDITOR_BGCOLOUR     0xFE7D  // Editor text background.
#define INTERFACE_DARK      0x52AE  // Single-line entry and handwriting field background.
#define TEXT_COLOUR         0x52AE  // Default text foreground.
#define LE_TEXT_COLOUR      0xFF1E  // Single-line entry text foreground.

#endif
