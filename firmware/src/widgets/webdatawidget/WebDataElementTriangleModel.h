#ifndef WEB_DATA_ELEMENT_TRIANGLE_MODEL_H
#define WEB_DATA_ELEMENT_TRIANGLE_MODEL_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#include "Utils.h"
#include "WebDataElement.h"

class WebDataElementTriangleModel : public WebDataElement {
public:
    int32_t getX();
    void setX(int32_t x);
    int32_t getY();
    void setY(int32_t y);

    int32_t getX2();
    void setX2(int32_t x);
    int32_t getY2();
    void setY2(int32_t y);
    int32_t getX3();
    void setX3(int32_t x);
    int32_t getY3();
    void setY3(int32_t y);

    bool getFilled();
    void setFilled(bool filled);

    void setColor(int32_t color);
    void setColor(String color);
    int32_t getColor();

    void parseData(const JsonObject &doc, int32_t defaultColor, int32_t defaultBackground) override;
    void draw(ScreenManager &manager) override;

private:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_x2 = 0;
    int32_t m_y2 = 0;
    int32_t m_x3 = 0;
    int32_t m_y3 = 0;
    bool m_filled = false;
    int32_t m_color = -1;
};
#endif
