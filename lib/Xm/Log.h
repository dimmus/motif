#ifndef XM_LOG_H_
#define XM_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

/*============================================================================*
 *                                  Types                                     *
 *============================================================================*/

/**
 * @typedef XmBool
 * Boolean type for XM logging system
 */
typedef enum _XmBool
{
   XM_FALSE = 0,
   XM_TRUE  = 1
} XmBool;

/**
 * @typedef XmLogLevel
 * Log levels for the logging system
 */
typedef enum _XmLogLevel
{
   XM_LOG_LEVEL_UNKNOWN  = -1,
   XM_LOG_LEVEL_CRITICAL = 0,
   XM_LOG_LEVEL_ERR      = 1,
   XM_LOG_LEVEL_WARN     = 2,
   XM_LOG_LEVEL_INFO     = 3,
   XM_LOG_LEVEL_DBG      = 4
} XmLogLevel;

/**
 * @def XM_LOG_LEVELS
 * Number of log levels
 */
#define XM_LOG_LEVELS 5

/**
 * @typedef XmLogDomain
 * Log domain structure
 */
typedef struct _XmLogDomain
{
   const char *name;
   const char *color;
   const char *domain_str;
   size_t      namelen;
   int         level;
   XmBool      deleted;
} XmLogDomain;

/**
 * @typedef XmLogPrintCb
 * Callback function type for printing log messages
 */
typedef void (*XmLogPrintCb)(const XmLogDomain *d,
                             XmLogLevel         level,
                             const char        *file,
                             const char        *fnc,
                             int                line,
                             const char        *fmt,
                             void              *data,
                             va_list            args);

/*============================================================================*
 *                                 Colors                                     *
 *============================================================================*/

#define LOG_COLOR_RESET  "\033[0m"
#define LOG_COLOR_RED    "\033[31m"
#define LOG_COLOR_GREEN  "\033[32m"
#define LOG_COLOR_YELLOW "\033[33m"
#define LOG_COLOR_BLUE   "\033[34m"
#define LOG_COLOR_CYAN   "\033[36m"

/*============================================================================*
 *                              Logging Macros                                *
 *============================================================================*/

/**
 * @def XM_LOG_DOMAIN_GLOBAL
 * Global logging domain identifier
 */
extern int XM_LOG_DOMAIN_GLOBAL;

/**
 * @def XM_LOG(level, fmt, ...)
 * Log a message at the specified level using the global domain
 */
#define XM_LOG(level, fmt, ...) \
   XmLogPrint(XM_LOG_DOMAIN_GLOBAL, level, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_CRIT(fmt, ...)
 * Log a critical message
 */
#define XM_LOG_CRIT(fmt, ...) \
   XM_LOG(XM_LOG_LEVEL_CRITICAL, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_ERR(fmt, ...)
 * Log an error message
 */
#define XM_LOG_ERR(fmt, ...) \
   XM_LOG(XM_LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_WARN(fmt, ...)
 * Log a warning message
 */
#define XM_LOG_WARN(fmt, ...) \
   XM_LOG(XM_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_INFO(fmt, ...)
 * Log an info message
 */
#define XM_LOG_INFO(fmt, ...) \
   XM_LOG(XM_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DBG(fmt, ...)
 * Log a debug message
 */
#define XM_LOG_DBG(fmt, ...) \
   XM_LOG(XM_LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DOM(level, domain, fmt, ...)
 * Log a message at the specified level using a specific domain
 */
#define XM_LOG_DOM(level, domain, fmt, ...) \
   XmLogPrint(domain, level, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DOM_CRIT(domain, fmt, ...)
 * Log a critical message to a specific domain
 */
#define XM_LOG_DOM_CRIT(domain, fmt, ...) \
   XM_LOG_DOM(XM_LOG_LEVEL_CRITICAL, domain, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DOM_ERR(domain, fmt, ...)
 * Log an error message to a specific domain
 */
#define XM_LOG_DOM_ERR(domain, fmt, ...) \
   XM_LOG_DOM(XM_LOG_LEVEL_ERR, domain, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DOM_WARN(domain, fmt, ...)
 * Log a warning message to a specific domain
 */
#define XM_LOG_DOM_WARN(domain, fmt, ...) \
   XM_LOG_DOM(XM_LOG_LEVEL_WARN, domain, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DOM_INFO(domain, fmt, ...)
 * Log an info message to a specific domain
 */
#define XM_LOG_DOM_INFO(domain, fmt, ...) \
   XM_LOG_DOM(XM_LOG_LEVEL_INFO, domain, fmt, ##__VA_ARGS__)

/**
 * @def XM_LOG_DOM_DBG(domain, fmt, ...)
 * Log a debug message to a specific domain
 */
#define XM_LOG_DOM_DBG(domain, fmt, ...) \
   XM_LOG_DOM(XM_LOG_LEVEL_DBG, domain, fmt, ##__VA_ARGS__)

/*============================================================================*
 *                                 API                                        *
 *============================================================================*/

/**
 * @brief Initialize the logging system.
 *
 * This function sets up the logging system and creates the global domain.
 * Must be called before any logging operations.
 *
 * @return #XM_TRUE on success, #XM_FALSE on failure.
 */
XmBool XmLogInit(void);

/**
 * @brief Shutdown the logging system.
 *
 * This function cleans up the logging system and frees all resources.
 * Should be called before program exit.
 *
 * @return #XM_TRUE on success, #XM_FALSE on failure.
 */
XmBool XmLogShutdown(void);

/**
 * @brief Register a new logging domain.
 *
 * @param[in] name The name of the domain.
 * @param[in] color The color to use for this domain (can be NULL).
 *
 * @return Domain identifier on success, -1 on failure.
 */
int XmLogDomainRegister(const char *name, const char *color);

/**
 * @brief Unregister a logging domain.
 *
 * @param[in] domain The domain identifier to unregister.
 */
void XmLogDomainUnregister(int domain);

/**
 * @brief Set the log level for a domain by name.
 *
 * @param[in] domain_name The name of the domain.
 * @param[in] level The log level to set.
 */
void XmLogDomainLevelSet(const char *domain_name, int level);

/**
 * @brief Get the log level for a domain by name.
 *
 * @param[in] domain_name The name of the domain.
 *
 * @return The current log level for the domain, or #XM_LOG_LEVEL_UNKNOWN if not found.
 */
int XmLogDomainLevelGet(const char *domain_name);

/**
 * @brief Set the log level for a registered domain.
 *
 * @param[in] domain The domain identifier.
 * @param[in] level The log level to set.
 */
void XmLogDomainRegisteredLevelSet(int domain, int level);

/**
 * @brief Get the log level for a registered domain.
 *
 * @param[in] domain The domain identifier.
 *
 * @return The current log level for the domain, or #XM_LOG_LEVEL_UNKNOWN if invalid.
 */
int XmLogDomainRegisteredLevelGet(int domain);

/**
 * @brief Set the print callback function.
 *
 * @param[in] cb The callback function to use for printing log messages.
 * @param[in] data User data to pass to the callback.
 */
void XmLogPrintCbSet(XmLogPrintCb cb, void *data);

/**
 * @brief Set the global log level.
 *
 * @param[in] level The log level to set.
 */
void XmLogLevelSet(int level);

/**
 * @brief Get the global log level.
 *
 * @return The current global log level.
 */
int XmLogLevelGet(void);

/**
 * @brief Check if the current thread is the main thread.
 *
 * @return #XM_TRUE if this is the main thread, #XM_FALSE otherwise.
 */
XmBool XmLogThreadsMainCheck(void);

/**
 * @brief Initialize thread support.
 *
 * This function must be called before using any thread-related logging functions.
 */
void XmLogThreadsInit(void);

/**
 * @brief Shutdown thread support.
 *
 * This function should be called from the main thread before program exit.
 */
void XmLogThreadsShutdown(void);

/**
 * @brief Enable thread support.
 *
 * This function enables thread-safe logging operations.
 */
void XmLogThreadsEnable(void);

/**
 * @brief Set whether colors are disabled.
 *
 * @param[in] disabled #XM_TRUE to disable colors, #XM_FALSE to enable.
 */
void XmLogColorDisableSet(XmBool disabled);

/**
 * @brief Get whether colors are disabled.
 *
 * @return #XM_TRUE if colors are disabled, #XM_FALSE otherwise.
 */
XmBool XmLogColorDisableGet(void);

/**
 * @brief Set whether file information is disabled.
 *
 * @param[in] disabled #XM_TRUE to disable file info, #XM_FALSE to enable.
 */
void XmLogFileDisableSet(XmBool disabled);

/**
 * @brief Get whether file information is disabled.
 *
 * @return #XM_TRUE if file info is disabled, #XM_FALSE otherwise.
 */
XmBool XmLogFileDisableGet(void);

/**
 * @brief Set whether function information is disabled.
 *
 * @param[in] disabled #XM_TRUE to disable function info, #XM_FALSE to enable.
 */
void XmLogFunctionDisableSet(XmBool disabled);

/**
 * @brief Get whether function information is disabled.
 *
 * @return #XM_TRUE if function info is disabled, #XM_FALSE otherwise.
 */
XmBool XmLogFunctionDisableGet(void);

/*============================================================================*
 *                              Print Callbacks                               *
 *============================================================================*/

/**
 * @brief Print callback for stderr output.
 *
 * This is the default print callback that outputs to stderr.
 */
void XmLogPrintCbStdErr(const XmLogDomain *d,
                        XmLogLevel         level,
                        const char        *file,
                        const char        *fnc,
                        int                line,
                        const char        *fmt,
                        void              *data,
                        va_list            args);

/**
 * @brief Print callback for stdout output.
 *
 * This callback outputs log messages to stdout.
 */
void XmLogPrintCbStdOut(const XmLogDomain *d,
                        XmLogLevel         level,
                        const char        *file,
                        const char        *fnc,
                        int                line,
                        const char        *fmt,
                        void              *data,
                        va_list            args);

/**
 * @brief Print callback for file output.
 *
 * @param[in] data Should be a FILE* pointer.
 */
void XmLogPrintCbFile(const XmLogDomain *d,
                      XmLogLevel         level,
                      const char        *file,
                      const char        *fnc,
                      int                line,
                      const char        *fmt,
                      void              *data,
                      va_list            args);

/*============================================================================*
 *                              Core Functions                                *
 *============================================================================*/

/**
 * @brief Print a log message.
 *
 * This is the main logging function that processes and outputs log messages.
 *
 * @param[in] domain The domain identifier.
 * @param[in] level The log level.
 * @param[in] file The source file name.
 * @param[in] fnc The function name.
 * @param[in] line The line number.
 * @param[in] fmt The format string.
 * @param[in] ... Variable arguments for the format string.
 */
void XmLogPrint(int domain, XmLogLevel level, const char *file, const char *fnc, int line, const char *fmt, ...);

/**
 * @brief Get the color code for a log level.
 *
 * @param[in] level The log level.
 *
 * @return The color code string for the level, or empty string if invalid.
 */
const char *XmLogLevelColorGet(int level);

/**
 * @brief Initialize the logging system with default configuration
 * 
 * This function sets up the default print callback based on the
 * XM_DEFAULT_LOG_OUTPUT configuration value set at build time.
 * 
 * @return XM_TRUE if initialization successful, XM_FALSE otherwise
 */
XmBool XmLogInit(void);

/**
 * @brief Set the log output destination at runtime
 * 
 * This function allows changing the log output destination after
 * the logging system has been initialized.
 * 
 * @param[in] output The output destination ("stderr", "stdout", or "file")
 * @param[in] filename Optional filename for file output (ignored for stderr/stdout)
 * 
 * @return XM_TRUE if successful, XM_FALSE if invalid output type
 */
XmBool XmLogSetOutput(const char *output, const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* XM_LOG_H_ */
