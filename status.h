/* function return codes */

#ifndef STATUS_H
#define STATUS_H

typedef int boolean;

#define FALSE 0
#define TRUE 1

typedef int status;

#define FAILED(x) ((x) < OK)

#define SIG_ERROR_BASE -128
#define ERRNO_ERROR_BASE_1 0
#define ERRNO_ERROR_BASE_2 70
#define CACHESTER_ERROR_BASE -220

#define OK 0
/* #define EOF (CACHESTER_ERROR_BASE - 1) */
#define SYNTAX_ERROR (CACHESTER_ERROR_BASE - 2)
#define BLOCKED (CACHESTER_ERROR_BASE - 3)
#define NOT_FOUND (CACHESTER_ERROR_BASE - 4)
#define NO_MEMORY (CACHESTER_ERROR_BASE - 5)
#define NO_HEARTBEAT (CACHESTER_ERROR_BASE - 6)
#define PROTOCOL_ERROR (CACHESTER_ERROR_BASE - 7)
#define WRONG_FILE_VERSION (CACHESTER_ERROR_BASE - 8)
#define WRONG_WIRE_VERSION (CACHESTER_ERROR_BASE - 9)
#define WRONG_DATA_VERSION (CACHESTER_ERROR_BASE - 10)
#define SEQUENCE_OVERFLOW (CACHESTER_ERROR_BASE - 11)
#define UNEXPECTED_SOURCE (CACHESTER_ERROR_BASE - 12)
#define CHANGE_QUEUE_OVERRUN (CACHESTER_ERROR_BASE - 13)
#define BUFFER_TOO_SMALL (CACHESTER_ERROR_BASE - 14)
#define INVALID_ADDRESS (CACHESTER_ERROR_BASE - 15)
#define DEADLOCK_DETECTED (CACHESTER_ERROR_BASE - 16)
#define STORAGE_ORPHANED (CACHESTER_ERROR_BASE - 17)
#define STORAGE_RECREATED (CACHESTER_ERROR_BASE - 18)
#define STORAGE_CORRUPTED (CACHESTER_ERROR_BASE - 19)
#define STORAGE_READ_ONLY (CACHESTER_ERROR_BASE - 20)
#define NO_CHANGE_QUEUE (CACHESTER_ERROR_BASE - 21)
#define INVALID_SLOT (CACHESTER_ERROR_BASE - 22)
#define INVALID_DEVICE (CACHESTER_ERROR_BASE - 23)
#define INVALID_FORMAT (CACHESTER_ERROR_BASE - 24)
#define UNKNOWN_FILE_TYPE (CACHESTER_ERROR_BASE - 25)
#endif
