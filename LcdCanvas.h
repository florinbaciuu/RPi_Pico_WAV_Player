/*-----------------------------------------------------------/
/ LcdCanvas
/------------------------------------------------------------/
/ Copyright (c) 2020, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/-----------------------------------------------------------*/

#ifndef __LCDCANVAS_H_INCLUDED__
#define __LCDCANVAS_H_INCLUDED__

#include "LcdElementBox/LcdElementBox.h"

// Select LCD device
#define USE_ST7735S_160x80

#if defined(USE_ST7735S_160x80)
#include "st7735_80x160/my_lcd.h" // Hardware-specific library for ST7735
#endif

#define FONT_HEIGHT		16

#if 0
//=================================
// Definition of BatteryIconBox class < IconBox
//=================================
class BatteryIconBox : public IconBox
{
public:
	BatteryIconBox(int16_t pos_x, int16_t pos_y, uint16_t fgColor = LCD_WHITE, uint16_t bgColor = LCD_BLACK);
	void draw();
	void setLevel(uint8_t value);
protected:
	uint8_t level;
};
#endif

//=================================
// Definition of LcdCanvas Class
//=================================
class LcdCanvas
{
public:
	LcdCanvas();
    ~LcdCanvas();
	void clear();
	void bye();
	void setListItem(int column, const char *str, const uint8_t icon = ICON16x16_UNDEF, bool isFocused = false);
	void setVolume(uint8_t value);
	void setPlayTime(uint32_t posionSec, uint32_t lengthSec, bool blink = false);
	void setTrack(const char *str);
	void setTitle(const char *str);
	void setAlbum(const char *str);
	void setArtist(const char *str);
	void setYear(const char *str);
	void setBatteryVoltage(uint16_t voltage_x1000);
	void addAlbumArtJpeg(uint16_t file_idx, uint64_t pos, size_t size, bool is_unsync = false);
	void addAlbumArtPng(uint16_t file_idx, uint64_t pos, size_t size, bool is_unsync = false);
	void deleteAlbumArt();
	void switchToListView();
	void switchToPlay();
	void switchToPowerOff(const char *msg_str = NULL);
	void drawListView();
	void drawPlay();
	void drawPowerOff();

protected:
	int play_count;
	const int play_cycle = 400;
	const int play_change = 350;
	#ifdef USE_ST7735S_160x80
	IconScrollTextBox listItem[5] = {
		IconScrollTextBox(16*0, 16*0, ICON16x16_UNDEF, LCD_W, FONT_HEIGHT, LCD_GRAY, LCD_BLACK),
		IconScrollTextBox(16*0, 16*1, ICON16x16_UNDEF, LCD_W, FONT_HEIGHT, LCD_GRAY, LCD_BLACK),
		IconScrollTextBox(16*0, 16*2, ICON16x16_UNDEF, LCD_W, FONT_HEIGHT, LCD_GRAY, LCD_BLACK),
		IconScrollTextBox(16*0, 16*3, ICON16x16_UNDEF, LCD_W, FONT_HEIGHT, LCD_GRAY, LCD_BLACK),
		IconScrollTextBox(16*0, 16*4, ICON16x16_UNDEF, LCD_W, FONT_HEIGHT, LCD_GRAY, LCD_BLACK)
	};
	//BatteryIconBox battery = BatteryIconBox(LCD_W-16, 16*0, LCD_GRAY);
	IconTextBox volume = IconTextBox(16*0, 16*0, ICON16x16_VOLUME, LCD_GRAY);
	TextBox playTime = TextBox(LCD_W, LCD_H-16, LcdElementBox::AlignRight, LCD_GRAY, LCD_BLACK);
	IconScrollTextBox title = IconScrollTextBox(16*0, 16*3, ICON16x16_TITLE, LCD_W, FONT_HEIGHT, LCD_WHITE, LCD_BLACK);
	IconScrollTextBox artist = IconScrollTextBox(16*0, 16*4, ICON16x16_ARTIST, LCD_W, FONT_HEIGHT, LCD_WHITE, LCD_BLACK);
	IconScrollTextBox album = IconScrollTextBox(16*0, 16*5, ICON16x16_ALBUM, LCD_W, FONT_HEIGHT, LCD_WHITE, LCD_BLACK);
	TextBox track = TextBox(16*0, LCD_H-16, LcdElementBox::AlignLeft, LCD_GRAY);
	TextBox msg = TextBox(LCD_W/2, LCD_H/2, LcdElementBox::AlignCenter);
	//ImageBox albumArt = ImageBox(0, (LCD_H - LCD_W)/2, LCD_W, LCD_W);
	LcdElementBox *groupListView[5] = {
		&listItem[0], &listItem[1], &listItem[2], &listItem[3], &listItem[4]
	};
	LcdElementBox *groupPlay[3] = {/*&battery, */&volume, &playTime, &track}; // Common for Play mode 0 and 1
	LcdElementBox *groupPlay0[3] = {&title, &artist, &album}; // Play mode 0 only
	LcdElementBox *groupPlay1[1] = {/*&albumArt, */&msg}; // Play mode 1 only
	LcdElementBox *groupPowerOff[1] = {&msg};
	#endif // USE_ST7735S_160x80
};

#endif // __LCDCANVAS_H_INCLUDED__
