/**
 * @file   compiler.h
 * @brief  Compiler specifics needed by TICD framework
 */
#ifndef TICD_COMPILER_H_INCLUDE
#define TICD_COMPILER_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

// Doxygen work arounds
#if __DOXYGEN__
    #define PACKED
#else
    #define PACKED __attribute__((packed))
#endif

#ifdef __cplusplus
}
#endif

#endif /* TICD_COMPILER_H_INCLUDE */