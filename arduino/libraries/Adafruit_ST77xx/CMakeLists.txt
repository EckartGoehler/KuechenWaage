# Adafruit ST7735 and ST7789 Library
# BSD License

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "src/Adafruit_ST7735.cpp"  "src/Adafruit_ST7735.h"  "src/Adafruit_ST7789.cpp" "src/Adafruit_ST7789.h" "src/Adafruit_ST77xx.cpp" "src/Adafruit_ST77xx.h"
                       INCLUDE_DIRS "."
                       REQUIRES arduino Adafruit_GFX)

project(Adafruit-ST77xx-Library)
