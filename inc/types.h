// Booleans
typedef char bool;
#define true ((bool) 1)
#define false ((bool) 0)

// Fixed-size integers
typedef char      uint8_t;
typedef char      int8_t;
typedef int       uint32_t;
typedef int       int32_t;
typedef int       size_t;
typedef short int uint16_t;
typedef short int int16_t;


// Fall-through for int64
typedef void uint64_t;
typedef void int64_t;

// Null pointer
#define NULL ((void *) 0x00)
