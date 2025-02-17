/**
 * @file common.h
 * Common configuration file
 */

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1

// when developing indoors GPS does not work, set this to work with 
// test NMEA data.
// #define GPS_INDOOR_TEST 0
#define GPS_INDOORS_TEST 1

static inline int version_major(void) {
    return VERSION_MAJOR;
}
static inline int version_minor(void) {
    return VERSION_MINOR;
}
static inline int version_patch(void) {
    return VERSION_PATCH;
}

