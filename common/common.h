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
//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int id;
  float batt_v;
  float temp;
  float hum;
  unsigned int readingId;
  int8_t fuel_perc;
  long gps_lat; // latitude - ex '30.239773'
  long gps_lng; // longitude - ex '-97.815685'
  uint8_t gps_time_hour;
  uint8_t gps_time_minute;
  uint8_t gps_time_second;
  double gps_speed_kmph; // current ground speed
  double gps_altitude_meters; // latest altitude fix
  int8_t gps_age; // mls since last update
} struct_message;

