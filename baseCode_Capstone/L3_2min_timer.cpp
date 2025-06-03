#include "L3_2min_timer.h"
#include <string.h>

bool getInputWithinTime(Serial& pc, char* buffer, int bufferSize, float timeLimitSec) {
    Timer inputTimer;
    inputTimer.start();

    // 남은 개행 문자 제거
    while (pc.readable()) {
        char dump = pc.getc();
        if (dump == '\n' || dump == '\r') break;
    }

    int index = 0;
    while (inputTimer.read() < timeLimitSec) {
        if (pc.readable()) {
            char c = pc.getc();

            if (c == '\r' || c == '\n') break;

            if ((c == 0x7F || c == '\b') && index > 0) {
                index--;
                pc.printf("\b \b");
                continue;
            }

            if (index < bufferSize - 1 && c >= 32 && c <= 126) {
                buffer[index++] = c;
            }
        }
    }

    buffer[index] = '\0';
    inputTimer.stop();

    return strlen(buffer) > 0;
}