/*

  Sketch to show how a Sprite is created, how to draw pixels
  and text within the Sprite and then push the Sprite onto
  the display screen.

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  A Sprite is notionally an invisible graphics screen that is
  kept in the processors RAM. Graphics can be drawn into the
  Sprite just as it can be drawn directly to the screen. Once
  the Sprite is completed it can be plotted onto the screen in
  any position. If there is sufficient RAM then the Sprite can
  be the same size as the screen and used as a frame buffer.

  A 16 bit Sprite occupies (2 * width * height) bytes in RAM.

  On a ESP8266 Sprite sizes up to 126 x 160 can be accomodated,
  this size requires 40kBytes of RAM for a 16 bit colour depth.
  
  When 8 bit colour depth sprites are created they occupy
  (width * height) bytes in RAM, so larger sprites can be
  created, or the RAM required is halved.

*/


#include <Arduino.h>
#include <TFT_eSPI.h>                 // Include the graphics library (this includes the sprite functions)
#define LV_COLOR_DEPTH 8

#include "esp_pm.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "stop.h"
#include "up.h"
#include "down.h"
#include "command_manager.hpp"
#include "touch_manager.hpp"
#include "commands.hpp"
#include "wifi_manager.hpp"
#include "config.hpp"

#define PEN GPIO_NUM_14

/*
#define Threshold 40
// Set delay after plotting the sprite
#define DELAY 100

//#define CALIBRATION 1
#ifdef CALIBRATION
#include "calibrate.h"
#endif

#define XSTR(x) #x
#define STR(x) XSTR(x)
*/
TFT_eSPI    tft = TFT_eSPI();         // Declare object "tft"

CommandManager commandManager;
TouchManager touchManager(tft, commandManager);
StopCommand stopCmd;
UpCommand upCmd;
DownCommand downCmd;
StandbyCommand standbyCmd;

WifiManager wifiManager(commandManager);
SemaphoreHandle_t xSemaphore = NULL;

extern "C" void IRAM_ATTR triggered_isr(void* instance) {
  gpio_intr_disable(PEN);
  Serial.println("Interrupt!");
	//Serial.printf("Interrupt: %i \n", digitalRead(PEN));
  xSemaphoreGiveFromISR(xSemaphore, pdFALSE);
  gpio_intr_enable(PEN);
}

/*void callback() {
  noInterrupts();
  
  Serial.printf("Interrupt: %i \n", digitalRead(PEN));
}*/


void setup()
{

  xSemaphore = xSemaphoreCreateBinary();

  Serial.begin(115200);

#if !CONFIG_PM_ENABLE
#error "CONFIG_PM_ENABLE missing"
#endif
#if !CONFIG_FREERTOS_USE_TICKLESS_IDLE
#error "CONFIG_FREERTOS_USE_TICKLESS_IDLE missing"
#endif
  esp_pm_config_esp32_t pm_config;
  pm_config.max_freq_mhz = 80;
  pm_config.min_freq_mhz = 13;
  pm_config.light_sleep_enable = true;
  ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );

  rtc_gpio_hold_en(GPIO_NUM_4);
  gpio_config_t config = {
		.pin_bit_mask = 1ull << PEN,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_NEGEDGE
	};

  Serial.begin(115200);
  Serial.println("Welcome... Booting;..");

	ESP_ERROR_CHECK(gpio_config(&config));
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
	ESP_ERROR_CHECK(gpio_isr_handler_add(PEN, triggered_isr, nullptr));
  
  Serial.println("Setting up initial pin state...");
  pinMode(RELAIS_UP, OUTPUT);
  pinMode(RELAIS_DOWN, OUTPUT);

  digitalWrite(RELAIS_UP, HIGH);
  digitalWrite(RELAIS_DOWN, HIGH);
  

  Serial.println("Initiate LCD...");
  tft.init();
  tft.fillScreen(TFT_BLACK);

  //#ifndef CALIBRATION
  // Initialise the TFT registers
 
  Serial.println("Initiate touch manager...");
  commandManager.registerCommand(UP_CMD_TAG, upCmd);
  commandManager.registerCommand(DOWN_CMD_TAG, downCmd);
  commandManager.registerCommand(STOP_CMD_TAG, stopCmd);
  touchManager.createButton(stop_map,41,56,46,46, standbyCmd);
  touchManager.createButton(up_map,41,0,46,46, upCmd);
  touchManager.createButton(down_map,41,112,46,46, downCmd);

  uint16_t calData[5] = { 530, 3290, 406, 3415, 0 };
  tft.setTouch(calData);

  Serial.println("Setting wifi settings");
  wifiManager.connect(WIFI_SSID, WIFI_PASSWORD,
                      MQTT_HOST, MQTT_PORT, MQTT_TOPIC);

  Serial.println("Setting sleep mode settings...");
  // keep RST pin state so that screen stays up in sleep mode

  //pinMode(PEN, INPUT);
  //attachInterrupt(digitalPinToInterrupt(PEN), callback, FALLING);
  esp_sleep_enable_ext0_wakeup(PEN, LOW);
  rtc_gpio_hold_en(GPIO_NUM_32);
  rtc_gpio_hold_en(GPIO_NUM_33);
  Serial.println("Entering light sleep state...");
  Serial.flush();

  
  
  //#endif*/
}

void loop(void)
{
  Serial.begin(115200);
  gpio_intr_disable(PEN);
  #ifndef CALIBRATION
  rtc_gpio_hold_dis(GPIO_NUM_32);
  rtc_gpio_hold_dis(GPIO_NUM_33);
  Serial.println(digitalRead(PEN));
  commandManager.loop();
  touchManager.loop();
  wifiManager.loop();
  delay(1);
  //Serial.println("Before light sleep");
  rtc_gpio_hold_en(GPIO_NUM_32);
  rtc_gpio_hold_en(GPIO_NUM_33);

  Serial.flush();
  Serial.println("loop");
  gpio_intr_enable(PEN);
  xSemaphoreTake( xSemaphore, 1300/portTICK_PERIOD_MS );
  //vTaskDelay(1300/portTICK_PERIOD_MS);
  //delay(1300);
  //esp_light_sleep_start();
  #else
  touch_calibrate(tft);
  #endif


}
