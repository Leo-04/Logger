# Logger
A basic logger for C

## Logging Methods
Levels can be one of:
- `LogLv_NORMAL`
- `LogLv_INFO`
- `LogLv_WARN`
- `LogLv_ERROR`
- `LogLv_FATIAL`
- `LogLv_DEBUG`
- `LogLv_TRACE`
- `LogLv_TEST`
These are bit flags, so `LogLv_NONE` and `LogLv_ALL` are also defined.

Logs a given message at a given level
- `Log(lv, fmt, ...)`
- `LogNormal(fmt, ...)`
- `LogInfo(fmt, ...)`
- `LogWarning(fmt, ...)`
- `LogError(fmt, ...)`
- `LogFatial(fmt, ...)`
- `LogDebug(fmt, ...)`
- `LogTrace(fmt, ...)`
- `LogTest(fmt, ...)`

Sets the levels of logs to be outputed
- `LogSetLevel(lv)`

## Other methods
Sets the file that the logs should be written to, if filename is null, then output to stdout
- `LogSetFile(filename)`

Redirect the standard output to a file
- `LogRedirectStdOut(filename)`

Redirect the output back to standard output
- `LogRedirectStdOut(NULL)`

Writes a message to a given file
If file is NULL, will output to stdout
If redirect_stdout is true, will redirect the standard output to the file
- `LogNewSession(filename, redirect_stdout)`

Gets the string reperastaion of a log level:
- `LogLvString(lv)`

## Defines
Use `-D_NO_LOGGING` to disbale logging functions
Use `-D_LOG_COLOR_ENABLED` to enable colored logging

Define `LOG_MASK` to change the default log level (by default set to `LogLv_ALL`).

## Usages explained:

### LogNormal
Log status of things

### LogInfo
Log that you are about todo doing something of importance

### LogWarning
Warning something bad just happend / will happen

### LogError
Saying something went wrong

### LogFatial
Saying something went *very* wrong

### LogDebug
Saying infromation useful for debuging

### LogTrace
Debuging information that is often very verbose
    Best to leave off as can kill the output
    Use this when the output can be verbose / ran alot / in loops

### LogTest
Infromation for when tests are ran

### LogFatialExit
Lazy mans way of doing: `LogFatial(...); exit(X);`

### LogAssert, LogAssertNull
Log a fatial message if a condtion is met

### LogExitAssert, LogExitAssertNull
Lazy mans way of doing: `LogAssert(...); exit(X);`

### LogErrorBacktrace
Logs a line of arrows up to a previous error log message

### LogErrorBacktraceAndReturn
Lazy mans way of doing: `LogErrorBacktrace(); return X;`

