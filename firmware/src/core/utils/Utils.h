#ifndef UTILS_H
#define UTILS_H

#include "Button.h"
#include <Arduino.h>

#define SHOW_MEMORY_USAGE(msg)               \
    do {                                     \
        Utils::showMemoryUsage(true, false); \
        if (msg[0] != '\0') {                \
            Serial.print(" --- ");           \
            Serial.println(msg);             \
        } else {                             \
            Serial.println();                \
        }                                    \
    } while (0)
#define MAX_WRAPPED_LINES 10

enum ScreenMode {
    Light = 0,
    Dark = 1
};

#define ScreenWidth 240
#define ScreenHeight 240
#define ScreenCenterX (ScreenWidth / 2)
#define ScreenCenterY (ScreenHeight / 2)

class Utils {
public:
    static int getWrappedLines(String (&lines)[MAX_WRAPPED_LINES], String str, int limit);
    static String getWrappedLine(String str, int limit, int lineNum, int maxLines);
    static int32_t stringToColor(String color);
    static String formatFloat(float value, int8_t digits);
    static int32_t stringToAlignment(String alignment);
    static uint16_t rgb565dim(uint16_t rgb565, uint8_t brightness, bool swapBytes = false);
    static void rgb565dimBitmap(uint16_t *pixel565, size_t length, uint8_t brightness, bool swapBytes = true);
    static uint32_t rgb565ToRgb888(uint16_t rgb565, bool swapBytes = false);
    static uint16_t rgb888ToRgb565(uint32_t rgb888, bool swapBytes = false);
    static String rgb565ToRgb888html(int color565);
    static int rgb888htmlToRgb565(const String &hexColor);
    static uint16_t grayscaleToTargetColor(uint8_t grayscale, uint8_t targetR8, uint8_t targetG8, uint8_t targetB8, float brightness, bool swapBytes = false);
    static void colorizeImageData(uint16_t *pixels565, size_t length, uint32_t targetColor888, float brightness, bool swapBytes = true);
    static char *copyString(const std::string &originalString);
    static bool compareCharArrays(const char *str1, const char *str2);
    static char *createWithPrefixAndPostfix(const char *prefix, const char *original, const char *postfix);
    static uint8_t stringToButtonId(const String &buttonName);
    static ButtonState stringToButtonState(const String &buttonState);
    static void showMemoryUsage(bool force = false, bool newLine = true);

private:
    static unsigned long s_lastMemoryInfo;
};

#endif
