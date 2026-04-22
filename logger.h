#pragma once

#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

// Logging functions

/*
Usages explained:

# LogNormal
Log status of things

# LogInfo
Log that you are about todo doing something of importance

# LogWarning
Warning something bad just happend / will happen

# LogError
Saying something went wrong

# LogFatial
Saying something went *very* wrong

# LogDebug
Saying infromation useful for debuging

# LogTrace
Debuging information that is often very verbose
    Best to leave off as can kill the output
    Use this when the output can be verbose / ran alot / in loops

# LogTest
Infromation for when tests are ran

# LogFatialExit
Lazy mans way of doing: `LogFatial(...); exit(X);`

# LogAssert, LogAssertNull
Log a fatial message if a condtion is met

# LogExitAssert, LogExitAssertNull
Lazy mans way of doing: `LogAssert(...); exit(X);`

# LogErrorBacktrace
Logs a line of arrows up to a previous log message

# LogErrorBacktraceAndReturn
Lazy mans way of doing: `LogErrorBacktrace(); return X;`
*/
#define Log(lv, fmt, ...) Logger(lv, __FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LogNormal(...) Log(LogLv_NORMAL, __VA_ARGS__)
#define LogInfo(...) Log(LogLv_INFO, __VA_ARGS__)
#define LogWarning(...) Log(LogLv_WARN, __VA_ARGS__)
#define LogError(...) Log(LogLv_ERROR, __VA_ARGS__)
#define LogFatial(...) Log(LogLv_FATIAL, __VA_ARGS__)
#define LogDebug(...) Log(LogLv_DEBUG, __VA_ARGS__)
#define LogTrace(...) Log(LogLv_TRACE, __VA_ARGS__)
#define LogTest(...) Log(LogLv_TEST, __VA_ARGS__)

#define LogFatialExit(code) do {LogFatial("Exiting with code: %i", code); exit(code);}while(0)

// Assert functions
#define STRINGIFY(x) #x
#define LogAssert(expr) do {if (expr){LogFatial("Assertion failed %s", STRINGIFY(expr));}}while(0)
#define LogAssertNull(expr) LogAssert((expr) == NULL)
#define LogExitAssert(expr) do {if (expr){LogFatialExit(-1);}}while(0)
#define LogExitAssertNull(expr) LogExitAssert((expr) == NULL)

// Other
#define LogErrorBacktrace() LogError("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
#define LogErrorBacktraceAndReturn(n) do{LogErrorBacktrace(); return n;}while(0)

// Constants
enum LogLv{
    LogLv_NONE      = 0x0,
    LogLv_NORMAL    = 0x1,
    LogLv_INFO      = 0x2,
    LogLv_WARN      = 0x4,
    LogLv_ERROR     = 0x8,
    LogLv_FATIAL    = 0x10,
    LogLv_DEBUG     = 0x20,
    LogLv_TRACE     = 0x40,
    LogLv_TEST      = 0x80,
    LogLv_ALL       = 0xFF,
};
#define LogLvString(lv)(\
    (lv == LogLv_NORMAL)? " Normal":\
    (lv == LogLv_INFO  )? "  Info ":\
    (lv == LogLv_WARN  )? "Warning":\
    (lv == LogLv_ERROR )? " Error ":\
    (lv == LogLv_FATIAL)? " Fatial":\
    (lv == LogLv_DEBUG )? " Debug ":\
    (lv == LogLv_TRACE )? " Trace ":\
    (lv == LogLv_TEST  )? "  Test ":\
    (lv == LogLv_NONE  )? "  None ":\
                          "???????" )

#ifndef LOG_MASK
#define LOG_MASK LogLv_ALL
#endif


#ifdef _NO_LOGGING

static inline int PassI(void){return 0;}
static inline void Pass(void){;}
#define LogNewSession(arg) PassI()
#define LogRedirectStdOut(arg) PassI()
#define LogSetlevel(lv) Pass()
#define LogSetFile(file) PassI()
#define Logger(...) Pass()

#else

static enum LogLv log_mask = (LOG_MASK);
static FILE* log_file = NULL;

/*
* Sets the logging level
*
* Parameters
*   level: LogLv
*       The level of the log
*/
static inline void LogSetlevel(enum LogLv lv){
    log_mask = lv;
}


/*
* Redirects stdout to a file
*
* Parameters:
*   filename: const char* 
*       The name of the file to redirect to
*       if filename == NULL, then it will redirect back to the console
*
* Returns:
*   0 on success
*/
static inline int LogSetFile(const char* file){
    if (file == NULL){
        log_file = NULL;
        return 0;
    }
    
    if (log_file != stdout && log_file != NULL){
        fclose(log_file);
    }
    log_file = fopen(file, "a");
    return log_file == NULL;
}


/*
* Logs messages to stdout
*
* Parameters
*   level: LogLv
*       The level of the log
*
*   file: const char* 
*       The curent file
*
*   line: int
*       The current line
*
*   func: const char* 
*       The curent function name
*
*   fmt: const char* 
*       A printf format string
*       If this value is NULL,
*         will set the log level to the passed `level`
*
*   ...
*       Printf arguments
*/
static inline void Logger(enum LogLv level, const char* file, int line, const char* func, const char* fmt, ...) {
    // Check if mask is on
    
    if (log_mask & level){
        // Check for color
        #ifdef _LOG_COLOR_ENABLED
            fprintf(log_file? log_file: stdout, "\033[%im",
                (level == LogLv_NORMAL)? 97:
                (level == LogLv_INFO)? 92:
                (level == LogLv_WARN)? 33:
                (level == LogLv_ERROR)? 91:
                (level == LogLv_FATIAL)? 95:
                (level == LogLv_DEBUG)? 94:
                (level == LogLv_TRACE)? 93:
                (level == LogLv_TEST)? 96:
                39
            );
        #endif

        // Get the current time
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        // Log information
        fprintf(log_file? log_file: stdout,
            "%.24s|%s[%s:%i %s]: ",
            asctime(timeinfo), LogLvString(level), file, line, func
        );
        // Flush incase VA_ARGS causes error
        // This ensures that the above is written
        //  and pointer bugs can be found even if
        //  we are trying to log pointer access
        fflush(log_file? log_file: stdout);  // AKA don't remove
        
        // Log arguments
        va_list args;
        va_start(args, fmt);
        vfprintf(log_file? log_file: stdout, fmt, args);
        va_end(args);

        // Check for color
        #ifdef _LOG_COLOR_ENABLED
            fprintf(log_file? log_file: stdout, "\033[1;0m");
        #endif
        
        fprintf(log_file? log_file: stdout, "\n");
    }
}

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>

#define _close close
#define _dup2 dup2
#define _dup dup
#define _open open
#endif

/*
* Redirects stdout to a file
*
* Parameters:
*   filename: const char* 
*       The name of the file to redirect to
*       if filename == NULL, then it will redirect stdout back to the console
*
* Returns:
*   0 on success
*/
static inline int LogRedirectStdOut(const char* filename){
    static int save_fd = -1;
    static int new_fd = -1;
    if (filename == NULL){
        if (new_fd == -1 && save_fd == -1){
            LogWarning("Did not redirect output");
        }
        // Flush buffer and close file
        LogInfo("Redirecting back to STDOUT\n");
        fflush(stdout);
        if (_close(new_fd)){
            LogError("Failed to close FD");
            return 1;
        }
        if (_dup2(save_fd, fileno(stdout)) == -1){
            LogError("Failed to dup2 FD");
            return 2;
        }
    } else {
        LogInfo("Redirected to file: %s", filename);
        // Open file
        new_fd = _open(filename, O_WRONLY|O_CREAT|O_APPEND, 666);
        if (new_fd == -1) {
            LogError("Failed to open file: %s", filename);
            return 3;
        }
        // Redirect stream
        if ((save_fd = _dup(fileno(stdout))) == -1){
            LogError("Failed to dup FD");
            return 4;
        }
        if (_dup2(new_fd, fileno(stdout)) == -1){
            LogError("Failed to dup2 FD");
            save_fd = fileno(stdout);
            return 5;
        }
    }

    return 0;
}

/*
* Adds info for new logging session
*
* Parameters:
*   log_file: const char* 
*       The name of the file to redirect to
*       If NULL, will not attempt to redirect
*
* Returns:
*   0 on success
*/
static inline int LogNewSession(const char* log_filename, bool redirect_sdtout){
    //Check if log_filename is valid
    if (log_filename != NULL){
        if (
            (redirect_sdtout && LogRedirectStdOut(log_filename))
            || LogSetFile(log_filename)
        ) LogErrorBacktraceAndReturn(1);
    }

    // Get the current time
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    // Output
    fprintf(log_file? log_file: stdout, "\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=[%.24s]=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n", asctime(timeinfo));
    fflush(stdout);

    return 0;
}

#endif
