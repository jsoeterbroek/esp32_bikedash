/**
 * @file common.h
 * Common configuration file
 */

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1


static inline int version_major(void) {
    return VERSION_MAJOR;
}
static inline int version_minor(void) {
    return VERSION_MINOR;
}
static inline int version_patch(void) {
    return VERSION_PATCH;
}
