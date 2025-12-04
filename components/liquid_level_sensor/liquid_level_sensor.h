#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/hal.h"
#include <Wire.h>

namespace esphome {
    namespace liquid_level_sensor {

#define THRESHOLD 100
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

        class liquidLevelSensor : public PollingComponent, public sensor::Sensor {
        public:
            liquidLevelSensor() : PollingComponent(1000) {}  // Poll every 1 second; adjust as needed

            void setup() override {
                Wire.begin();  // Initialize I2C (ESPHome handles pin config via YAML)
            }

            void dump_config() override {
                ESP_LOGCONFIG(TAG, "Liquid Level Sensor:");
                LOG_SENSOR("  ", "Sensor", this);
            }

            void update() override {
                unsigned char low_data[8] = { 0 };
                unsigned char high_data[12] = { 0 };

                // Read low 8 sections
                Wire.requestFrom(ATTINY2_LOW_ADDR, (uint8_t)8);
                for (int i = 0; i < 8 && Wire.available(); i++) {
                    low_data[i] = Wire.read();
                }

                // Read high 12 sections
                Wire.requestFrom(ATTINY1_HIGH_ADDR, (uint8_t)12);
                for (int i = 0; i < 12 && Wire.available(); i++) {
                    high_data[i] = Wire.read();
                }

                // Build touch_val bitmask
                uint32_t touch_val = 0;
                for (int i = 0; i < 8; i++) {
                    if (low_data[i] > THRESHOLD) {
                        touch_val |= 1 << i;
                    }
                }
                for (int i = 0; i < 12; i++) {
                    if (high_data[i] > THRESHOLD) {
                        touch_val |= (uint32_t)1 << (8 + i);
                    }
                }

                // Count consecutive triggered sections from the bottom
                uint8_t trig_section = 0;
                while (touch_val & 0x01) {
                    trig_section++;
                    touch_val >>= 1;
                }

                // Calculate and publish percentage
                float level = trig_section * 5.0f;
                publish_state(level);
            }
        };

    }  // namespace liquid_level_sensor
}  // namespace esphome