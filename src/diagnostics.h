#pragma once
#include <string>
#include <Wire.h>  // Serial
const bool diagnostics_on = true;
void sendToLog(std::string msg) {
  if (diagnostics_on) {
    Serial.println(msg.c_str());
  }
}