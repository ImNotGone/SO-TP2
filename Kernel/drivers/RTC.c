// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <drivers/RTC.h>

extern uint8_t sys_RTC(uint8_t mode);

static uint8_t binaryCodedDecimalToDecimal(uint8_t BCD) {
    return (BCD >> 4) * 10 + (BCD & 0x0F);
}

static uint8_t getRTC(uint8_t mode) {
    return binaryCodedDecimalToDecimal(sys_RTC(mode));
}

uint8_t getRTCSeconds() {
    return getRTC(RTC_SECONDS);
}

uint8_t getRTCMinutes() {
    return getRTC(RTC_MINUTES);
}

uint8_t getRTCHours() {
    return getRTC(RTC_HOURS);
}

uint8_t getRTCDayOfWeek() {
    return getRTC(RTC_DAY_OF_WEEK);
}

uint8_t getRTCDayOfMonth() {
    return getRTC(RTC_DAY_OF_MONTH);
}

uint8_t getRTCMonth() {
    return getRTC(RTC_MONTH);
}

uint8_t getRTCYear() {
    return getRTC(RTC_YEAR);
}

// Returns the current time in seconds
uint64_t getTotalSeconds() {
    uint64_t seconds = getRTCSeconds();
    seconds += getRTCMinutes() * 60;
    seconds += getRTCHours() * 3600;
    seconds += getRTCDayOfMonth() * 86400;

    // TODO: tener en cuenta los años bisiestos y los distintos meses, creo q no importa?

    seconds += getRTCMonth() * 2592000; // Month is not exact, but it's close enough
    seconds += getRTCYear() * 31104000; // Year is not exact, but it's close enough
    return seconds;
}


