#pragma once

#include "Base.h"
#include "../TsCore/JsonEngine.h"

class JsonEngine;

void typeTextWithSave(const std::string& text, int color, int delayMs, bool waitAtEnd, JsonEngine* JSengine);