# CMake file to build KitchenScale applications - here a cmake project for Arduino like targets

# Arduino CMake Toolchain and Board Configuration
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/Arduino-CMake-Toolchain/Arduino-toolchain.cmake")
set(ARDUINO_BOARD "LOLIN(WEMOS) D1 mini Lite [esp8266.d1_mini_lite]" CACHE STRING "Board type")
set(ARDUINO_PORT "/dev/ttyUSB0" CACHE STRING "Upload port")
# Optionally set upload protocol, e.g. "arduino" or "stk500"
# set(ARDUINO_UPLOAD_PROTOCOL "arduino" CACHE STRING "Upload protocol")

cmake_minimum_required(VERSION 3.28.0)
	
project ( KitchenScale CXX)



  include_directories (
  	libraries/Adafruit_BusIO/src
  	)

set(SRCDIR "src")


# ----------------------------
# main App: KitchenScale

#file(GLOB ScaleAppSrcFiles 
#	"${SRCDIR}/kitchenscale.cpp"
#	)

add_executable(KitchenScale src/kitchenscale.cpp)

target_link_arduino_libraries(KitchenScale PRIVATE HX711 Wire core)


target_link_arduino_libraries(KitchenScale PRIVATE Adafruit_BusIO Adafruit_GFX Adafruit_ST77xx )

# This is needed for the generation of HEX binary and uploading it
target_enable_arduino_upload(KitchenScale)

# ----------------------------
#test app: blink

add_executable(blink src/blink.cpp)

target_link_arduino_libraries(blink PRIVATE core )
target_enable_arduino_upload(blink)
