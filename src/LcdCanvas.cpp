/*-----------------------------------------------------------/
/ LcdCanvas
/------------------------------------------------------------/
/ Copyright (c) 2020-2021, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/-----------------------------------------------------------*/

#include "LcdCanvas.h"

#include <cstdio>
#include <cstring>
#include "ImageFitter.h"
#include "LcdCanvasIcon.h"

uint8_t* ICON2PTR(IconIndex_t index)
{
    return (index == IconIndex_t::UNDEF) ? nullptr : &Icon16[32*static_cast<int>(index)];
}

//=================================
// Implementation of BatteryIconBox class
//=================================
BatteryIconBox::BatteryIconBox(int16_t pos_x, int16_t pos_y, uint16_t fgColor, uint16_t bgColor)
    : IconBox(pos_x, pos_y, ICON2PTR(IconIndex_t::BATTERY), fgColor, bgColor), level(0) {}

void BatteryIconBox::draw()
{
    if (!isUpdated) { return; }
    isUpdated = false;
    clear();
    LCD_ShowIcon(pos_x, pos_y, icon, !bgOpaque, fgColor);
    uint16_t color = (level >= 50) ? 0x0600 : (level >= 20) ? 0xc600 : 0xc000;
    if (level/10 < 9) {
        LCD_Fill(pos_x+4, pos_y+4, pos_x+4+8-1, pos_y+4+10-level/10-1-1, bgColor);
    }
    LCD_Fill(pos_x+4, pos_y+13-level/10, pos_x+4+8-1, pos_y+13-level/10+level/10+1-1, color);
}

void BatteryIconBox::setLevel(uint8_t value)
{
    value = (value <= 100) ? value : 100;
    if (this->level == value) { return; }
    this->level = value;
    update();
}

//=================================
// Implementation of LcdCanvas class
//=================================
void LcdCanvas::configureLcd(uint cfg_id)
{
    pico_st7735_80x160_config_t lcd_cfg[3] = {
        {
            SPI_CLK_FREQ_DEFAULT,
            spi1,
            PIN_LCD_SPI1_CS_DEFAULT,
            PIN_LCD_SPI1_SCK_DEFAULT,
            PIN_LCD_SPI1_MOSI_DEFAULT,
            PIN_LCD_DC_DEFAULT,
            PIN_LCD_RST_DEFAULT,
            PIN_LCD_BLK_DEFAULT,
            PWM_BLK_DEFAULT,
            INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
            RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
            ROTATION_DEFAULT,
            H_OFS_DEFAULT,
            V_OFS_DEFAULT,
            X_MIRROR_DEFAULT
        },
        {
            SPI_CLK_FREQ_DEFAULT,
            spi1,
            PIN_LCD_SPI1_CS_DEFAULT,
            PIN_LCD_SPI1_SCK_DEFAULT,
            PIN_LCD_SPI1_MOSI_DEFAULT,
            PIN_LCD_DC_DEFAULT,
            PIN_LCD_RST_DEFAULT,
            PIN_LCD_BLK_DEFAULT,
            PWM_BLK_DEFAULT,
            0,  //INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
            RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
            ROTATION_DEFAULT,
            0,  //H_OFS_DEFAULT,
            24,  //V_OFS_DEFAULT
            X_MIRROR_DEFAULT
        },
        {
            SPI_CLK_FREQ_DEFAULT,
            spi1,
            PIN_LCD_SPI1_CS_DEFAULT,
            PIN_LCD_SPI1_SCK_DEFAULT,
            PIN_LCD_SPI1_MOSI_DEFAULT,
            PIN_LCD_DC_DEFAULT,
            PIN_LCD_RST_DEFAULT,
            PIN_LCD_BLK_DEFAULT,
            PWM_BLK_DEFAULT,
            INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
            0,  //RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
            ROTATION_DEFAULT,
            H_OFS_DEFAULT,
            V_OFS_DEFAULT,
            1  //X_MIRROR_DEFAULT
        }
    };
    if (cfg_id < sizeof(lcd_cfg) / sizeof(pico_st7735_80x160_config_t)) {
        LCD_Config(&lcd_cfg[cfg_id]);
    }
    LCD_Init();
    LCD_Clear(BLACK);
    BACK_COLOR=BLACK;
}

LcdCanvas& LcdCanvas::instance()
{
    // since LcdCanvas refers to LCD_H(), LCD_V() in its initialization, the instance must be generated dynamically after LCD initialization
    static LcdCanvas* _instance = nullptr; // Singleton
    if (_instance == nullptr) {
        _instance = new LcdCanvas();
    }
    return *_instance;
}

void LcdCanvas::switchToOpening()
{
    clear(true);
    msg.setText("");
    for (int i = 0; i < (int) (sizeof(groupOpening)/sizeof(*groupOpening)); i++) {
        groupOpening[i]->update();
    }
}

void LcdCanvas::switchToListView()
{
    clear(true);
    msg.setText("");
    battery.setBgOpaque(true);
    for (int i = 0; i < (int) (sizeof(groupListView)/sizeof(*groupListView)); i++) {
        groupListView[i]->update();
    }
}

void LcdCanvas::switchToPlay()
{
    clear(false);
    msg.setText("");
    battery.setBgOpaque(false);
    for (int i = 0; i < (int) (sizeof(groupPlay)/sizeof(*groupPlay)); i++) {
        groupPlay[i]->update();
    }
    for (int i = 0; i < (int) (sizeof(groupPlay0)/sizeof(*groupPlay0)); i++) {
        groupPlay0[i]->update();
    }
    for (int i = 0; i < (int) (sizeof(groupPlay1)/sizeof(*groupPlay1)); i++) {
        groupPlay1[i]->update();
    }
    play_count = 0;
}

void LcdCanvas::switchToPowerOff()
{
    clear(true);
    for (int i = 0; i < (int) (sizeof(groupPowerOff)/sizeof(*groupPowerOff)); i++) {
        groupPowerOff[i]->update();
    }
}

void LcdCanvas::clear(bool bgOpaque)
{
    LCD_FillBackground(0, 0, LCD_W()-1, LCD_H()-1, !bgOpaque, LCD_BLACK);
}

void LcdCanvas::setRotation(uint8_t rot)
{
    LCD_SetRotation(rot);
}

void LcdCanvas::drawOpening()
{
    for (int i = 0; i < (int) (sizeof(groupOpening)/sizeof(*groupOpening)); i++) {
        groupOpening[i]->draw();
    }
}

void LcdCanvas::drawListView()
{
    for (int i = 0; i < (int) (sizeof(groupListView)/sizeof(*groupListView)); i++) {
        groupListView[i]->draw();
    }
}

void LcdCanvas::drawPlay()
{
    for (int i = 0; i < (int) (sizeof(groupPlay)/sizeof(*groupPlay)); i++) {
        groupPlay[i]->draw();
    }
    if (play_count % play_cycle < play_change || !image.hasImage()) { // Play mode 0 display
        for (int i = 0; i < (int) (sizeof(groupPlay0)/sizeof(*groupPlay0)); i++) {
            groupPlay0[i]->draw();
        }
        if (play_count % play_cycle == play_change-1 && image.hasImage()) { // Play mode 0 -> 1
            clear(false);
            for (int i = 0; i < (int) (sizeof(groupPlay)/sizeof(*groupPlay)); i++) {
                groupPlay[i]->update();
            }
            for (int i = 0; i < (int) (sizeof(groupPlay1)/sizeof(*groupPlay1)); i++) {
                groupPlay1[i]->update();
            }
        }
    } else { // Play mode 1 display
        for (int i = 0; i < (int) (sizeof(groupPlay1)/sizeof(*groupPlay1)); i++) {
            groupPlay1[i]->draw();
        }
        if (play_count % play_cycle == play_cycle-1) { // Play mode 1 -> 0
            clear(false);
            for (int i = 0; i < (int) (sizeof(groupPlay)/sizeof(*groupPlay)); i++) {
                groupPlay[i]->update();
            }
            for (int i = 0; i < (int) (sizeof(groupPlay0)/sizeof(*groupPlay0)); i++) {
                groupPlay0[i]->update();
            }
        }
    }
    play_count++;
}

void LcdCanvas::drawPowerOff()
{
    for (int i = 0; i < (int) (sizeof(groupPowerOff)/sizeof(*groupPowerOff)); i++) {
        groupPowerOff[i]->draw();
    }
}

void LcdCanvas::setImageJpeg(const char* filename)
{
    uint16_t* img_ptr;
    uint16_t w, h;
    image.getImagePtr(&img_ptr, &w, &h);
    imgFit.config(img_ptr, w, h);
    imgFit.loadJpegFile(filename);
    imgFit.getSizeAfterFit(&w, &h);
    image.setImageSize(w, h);
    image.update();
}

void LcdCanvas::resetImage()
{
    image.resetImage();
}

void LcdCanvas::setMsg(const char* str, bool blink)
{
    msg.setText(str);
    msg.setBlink(blink);
}

void LcdCanvas::setListItem(int column, const char* str, const IconIndex_t index, bool isFocused)
{
    uint16_t color[2] = {LCD_GRAY, LCD_GBLUE};
    listItem[column].setIcon(ICON2PTR(index));
    listItem[column].setFgColor(color[isFocused]);
    listItem[column].setText(str);
    listItem[column].setScroll(isFocused); // Scroll for focused item only
}

void LcdCanvas::setVolume(uint8_t value)
{
    volume.setFormatText("%3d", (int) value);
}

void LcdCanvas::setAudioLevel(float levelL, float levelR)
{
    levelMeterL.setLevel(levelL);
    levelMeterR.setLevel(levelR);
}

void LcdCanvas::setBitRes(uint16_t value)
{
    // compose Icon for bit resolution part (upper half)
    switch(value) {
        case 16: memcpy(&bitSampIcon[0], ICON2PTR(IconIndex_t::_16BIT), 16); break;
        case 24: memcpy(&bitSampIcon[0], ICON2PTR(IconIndex_t::_24BIT), 16); break;
        case 32: memcpy(&bitSampIcon[0], ICON2PTR(IconIndex_t::_32BIT), 16); break;
        default: memset(&bitSampIcon[0], 0, 16); break;
    }
    bitSamp.setIcon(bitSampIcon);
}

void LcdCanvas::setSampleFreq(uint32_t sampFreq)
{
    // compose Icon for sampling frequency part (lower half)
    switch(sampFreq) {
        case 44100:  memcpy(&bitSampIcon[16], ICON2PTR(IconIndex_t::_44_1KHZ)  + 16, 16); break;
        case 48000:  memcpy(&bitSampIcon[16], ICON2PTR(IconIndex_t::_48_0KHZ)  + 16, 16); break;
        case 88200:  memcpy(&bitSampIcon[16], ICON2PTR(IconIndex_t::_88_2KHZ)  + 16, 16); break;
        case 96000:  memcpy(&bitSampIcon[16], ICON2PTR(IconIndex_t::_96_0KHZ)  + 16, 16); break;
        case 176400: memcpy(&bitSampIcon[16], ICON2PTR(IconIndex_t::_176_4KHZ) + 16, 16); break;
        case 192000: memcpy(&bitSampIcon[16], ICON2PTR(IconIndex_t::_192_0KHZ) + 16, 16); break;
        default: memset(&bitSampIcon[16], 0, 16); break;
    }
    bitSamp.setIcon(bitSampIcon);
}

void LcdCanvas::setPlayTime(uint32_t positionSec, uint32_t lengthSec, bool blink)
{
    //playTime.setFormatText("%lu:%02lu / %lu:%02lu", positionSec/60, positionSec%60, lengthSec/60, lengthSec%60);
    playTime.setFormatText("%lu:%02lu", positionSec/60, positionSec%60);
    playTime.setBlink(blink);
    timeProgress.setLevel((float) positionSec/lengthSec);
}

void LcdCanvas::setTrack(const char* str)
{
    track.setText(str);
}

void LcdCanvas::setTitle(const char* str)
{
    title.setText(str);
}

void LcdCanvas::setAlbum(const char* str)
{
    album.setText(str);
}

void LcdCanvas::setArtist(const char* str)
{
    artist.setText(str);
}

uint16_t LcdCanvas::getTiledImage(uint16_t x, uint16_t y)
{
    return image.getPixel(x, y, true);
}

/*
void LcdCanvas::setYear(const char*str)
{
    year.setText(str);
}
*/

void LcdCanvas::setBatteryVoltage(uint16_t voltage_x1000)
{
    const uint16_t lvl100 = 4100;
    const uint16_t lvl0 = 2900;
    battery.setLevel(((voltage_x1000 - lvl0) * 100) / (lvl100 - lvl0));
}
