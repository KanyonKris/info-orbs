#include "widgets/clockWidget.h"

#include <config_helper.h>
#include <globalTime.h>

ClockWidget::ClockWidget(ScreenManager& manager) : Widget(manager) {
}

ClockWidget::~ClockWidget() {
}

void ClockWidget::setup() {
    m_lastDisplay1Digit = "";
    m_lastDisplay2Digit = "";
    m_lastDisplay4Digit = "";
    m_lastDisplay5Digit = "";
}

void ClockWidget::draw(bool force) {
    m_manager.setFont(CLOCK_FONT);
    GlobalTime* time = GlobalTime::getInstance();
    
    if (m_lastDisplay1Digit != m_display1Digit || force) {
        displayDigit(0, m_lastDisplay1Digit, m_display1Digit, FOREGROUND_COLOR);
        m_lastDisplay1Digit = m_display1Digit;
    }
    if (m_lastDisplay2Digit != m_display2Digit || force) {
        displayDigit(1, m_lastDisplay2Digit, m_display2Digit, FOREGROUND_COLOR);
        m_lastDisplay2Digit = m_display2Digit;
    }
    if (m_lastDisplay4Digit != m_display4Digit || force) {
        displayDigit(3, m_lastDisplay4Digit, m_display4Digit, FOREGROUND_COLOR);
        m_lastDisplay4Digit = m_display4Digit;
    }
    if (m_lastDisplay5Digit != m_display5Digit || force) {
        displayDigit(4, m_lastDisplay5Digit, m_display5Digit, FOREGROUND_COLOR);
        m_lastDisplay5Digit = m_display5Digit;
    }

    if (m_secondSingle != m_lastSecondSingle || force) {
        if (m_secondSingle % 2 == 0) {
            displayDigit(2, "", ":", FOREGROUND_COLOR, false);
        } else {
            displayDigit(2, "", ":", BG_COLOR, false);
        }
#if SHOW_SECOND_TICKS == true
        displaySeconds(2, m_lastSecondSingle, TFT_BLACK);
        displaySeconds(2, m_secondSingle, FOREGROUND_COLOR);
#endif
        m_lastSecondSingle = m_secondSingle;
        if (!FORMAT_24_HOUR && SHOW_AM_PM_INDICATOR && m_type != ClockType::NIXIE) {
            displayAmPm(FOREGROUND_COLOR);
        }
    }
}

void ClockWidget::displayAmPm(uint32_t color) {
    GlobalTime* time = GlobalTime::getInstance();
    m_manager.selectScreen(2);
    m_manager.setFontColor(color, TFT_BLACK);
    String am_pm = time->isPM() ? "PM" : "AM";
    m_manager.drawString(am_pm, SCREEN_SIZE / 4 * 3, SCREEN_SIZE / 2, 25, Align::MiddleCenter);
}

void ClockWidget::update(bool force) {
    if (millis() - m_secondTimerPrev < m_secondTimer && !force) {
        return;
    }

    GlobalTime* time = GlobalTime::getInstance();
    m_hourSingle = time->getHour();

    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();

    if (m_lastHourSingle != m_hourSingle || force) {
        if (m_hourSingle < 10) {
            if (FORMAT_24_HOUR) {
                m_display1Digit = "0";
            } else {
                m_display1Digit = " ";
            }
        } else {
            m_display1Digit = int(m_hourSingle/10);
        }
        m_display2Digit = m_hourSingle % 10;

        m_lastHourSingle = m_hourSingle;
    }

    if (m_lastMinuteSingle != m_minuteSingle || force) {
        String currentMinutePadded = String(m_minuteSingle).length() == 1 ? "0" + String(m_minuteSingle) : String(m_minuteSingle);

        m_display4Digit = currentMinutePadded.substring(0, 1);
        m_display5Digit = currentMinutePadded.substring(1, 2);

        m_lastMinuteSingle = m_minuteSingle;
    }
}

void ClockWidget::change24hMode() {
    GlobalTime* time = GlobalTime::getInstance();
    time->setFormat24Hour( !time->getFormat24Hour() );
    draw(true);
}

void ClockWidget::changeClockType() {
    switch (m_type)
    {
    case ClockType::NORMAL:
        m_type = ClockType::NIXIE;
        break;
    
    case ClockType::NIXIE:
        m_type = ClockType::NORMAL;
        break;
    }
    m_manager.clearAllScreens();
    draw(true);
}

void ClockWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        changeClockType();
    } else if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        change24hMode();
    }
}

DigitOffset ClockWidget::getOffsetForDigit(const String &digit) {
    if (digit.length() > 0) {
        char c = digit.charAt(0);
        if (c >= '0' && c <= '9') {
            // get digit offsets
            return m_digitOffsets[c - '0'];
        }
    }
    // not a valid digit
    return {0, 0};
}

void ClockWidget::displayDigit(int displayIndex, const String& lastDigit, const String& digit, uint32_t color, bool shadowing) {
    if (m_type == ClockType::NIXIE) {
        if (digit == ":" && color == BG_COLOR) {
            // Show colon off
            displayNixie(displayIndex, " ");
        } else {
            displayNixie(displayIndex, digit);
        }
    } else {
        // Normal clock
        int fontSize = CLOCK_FONT_SIZE;
        char c = digit.charAt(0);
        bool isDigit = c >= '0' && c <= '9';
        int defaultX = SCREEN_SIZE / 2 + (isDigit ? CLOCK_OFFSET_X_DIGITS : CLOCK_OFFSET_X_COLON);
        int defaultY = SCREEN_SIZE / 2;
        DigitOffset digitOffset = getOffsetForDigit(digit);
        DigitOffset lastDigitOffset = getOffsetForDigit(lastDigit);
        m_manager.selectScreen(displayIndex);
        if (shadowing) {
            m_manager.setFontColor(BG_COLOR, TFT_BLACK);
            if (CLOCK_FONT == DSEG14) {
                // DSEG14 (from DSEGstended) uses # to fill all segments
                m_manager.drawString("#", defaultX, defaultY, fontSize, Align::MiddleCenter);
            } else if (CLOCK_FONT == DSEG7) {
                // DESG7 uses 8 to fill all segments
                m_manager.drawString("8", defaultX, defaultY, fontSize, Align::MiddleCenter);
            } else {
                // Other fonts can't be shadowed
                m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
                m_manager.drawString(lastDigit, defaultX + lastDigitOffset.x, defaultY + lastDigitOffset.y, fontSize, Align::MiddleCenter);    
            }
        } else {
            m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
            m_manager.drawString(lastDigit, defaultX + lastDigitOffset.x, defaultY + lastDigitOffset.y, fontSize, Align::MiddleCenter);
        }
        m_manager.setFontColor(color, TFT_BLACK);
        m_manager.drawString(digit, defaultX + digitOffset.x, defaultY + digitOffset.y, fontSize, Align::MiddleCenter);
    }
}

void ClockWidget::displayDigit(int displayIndex, const String& lastDigit, const String& digit, uint32_t color) {
    displayDigit(displayIndex, lastDigit, digit, color, SHADOWING);
}

void ClockWidget::displaySeconds(int displayIndex, int seconds, int color) {
    if (m_type == ClockType::NIXIE && color == FOREGROUND_COLOR) {
        // Special color (orange) for nixie
        color = 0xfd40;
    }
    m_manager.selectScreen(displayIndex);
    if (seconds < 30) {
        m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds + 180, 6 * seconds + 180 + 6, color, TFT_BLACK);
    } else {
        m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds - 180, 6 * seconds - 180 + 6, color, TFT_BLACK);
    }
}

void ClockWidget::displayNixie(int displayIndex, String digit) {
    if (digit.length() != 1) {
        return;
    }
    int m_digit;
    m_manager.selectScreen(displayIndex);
    TJpgDec.setJpgScale(1);
    switch (digit.charAt(0)) {
    case '0':
        TJpgDec.drawJpg(7, 0, nixie_0_start, nixie_0_end - nixie_0_start);
        break;
    case '1':
        TJpgDec.drawJpg(7, 0, nixie_1_start, nixie_1_end - nixie_1_start);
        break;
    case '2':
        TJpgDec.drawJpg(7, 0, nixie_2_start, nixie_2_end - nixie_2_start);
        break;
    case '3':
        TJpgDec.drawJpg(7, 0, nixie_3_start, nixie_3_end - nixie_3_start);
        break;
    case '4':
        TJpgDec.drawJpg(7, 0, nixie_4_start, nixie_4_end - nixie_4_start);
        break;
    case '5':
        TJpgDec.drawJpg(7, 0, nixie_5_start, nixie_5_end - nixie_5_start);
        break;
    case '6':
        TJpgDec.drawJpg(7, 0, nixie_6_start, nixie_6_end - nixie_6_start);
        break;
    case '7':
        TJpgDec.drawJpg(7, 0, nixie_7_start, nixie_7_end - nixie_7_start);
        break;
    case '8':
        TJpgDec.drawJpg(7, 0, nixie_8_start, nixie_8_end - nixie_8_start);
        break;
    case '9':
        TJpgDec.drawJpg(7, 0, nixie_9_start, nixie_9_end - nixie_9_start);
        break;
    case ' ':
        TJpgDec.drawJpg(7, 0, nixie_colon_off_start, nixie_colon_off_end - nixie_colon_off_start);
        break;
    case ':':
        TJpgDec.drawJpg(7, 0, nixie_colon_on_start, nixie_colon_on_end - nixie_colon_on_start);
        break;
    }
}

String ClockWidget::getName() {
    return "Clock";
}