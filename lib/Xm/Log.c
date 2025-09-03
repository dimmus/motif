#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Log.h"

// Global domain
int XM_LOG_DOMAIN_GLOBAL = 0;

// Simple domain structure
static XmLogDomain *_log_domains           = NULL;
static unsigned int _log_domains_count     = 0;
static size_t       _log_domains_allocated = 0;

// Print callback - initialized based on XM_DEFAULT_LOG_OUTPUT
static XmLogPrintCb _print_cb       = NULL;  // Will be set by XmLogInit()
static void        *_print_cb_data  = NULL;

// Global log level - set from configure
static XmLogLevel _log_level = XM_DEFAULT_LOG_LEVEL;

// Type definitions for compatibility
typedef pthread_t XmThread;

// Thread management
static XmBool    _threads_enabled = XM_FALSE;
static XmBool    _threads_inited  = XM_FALSE;
static XmThread  _main_thread     = 0;

// Thread macros
#define SELF()           ((XmThread)pthread_self())
#define IS_MAIN(thread)  (thread == _main_thread)
#define IS_OTHER(thread) (thread != _main_thread)
#define INIT()           (_threads_inited = XM_TRUE)
#define CHECK_MAIN()     assert(IS_MAIN(SELF()))
#define SHUTDOWN()       (_threads_inited = XM_FALSE)

// Lock macros
#define LOG_LOCK()   (void)0
#define LOG_UNLOCK() (void)0

// Level names
static const char *_level_names[] = {
   "CRI",
   "ERR",
   "WRN",
   "INF",
   "DBG"
};

const char *
XmLogLevelColorGet(int level)
{
   static const char *colors[] = {
      LOG_COLOR_RED,    // CRITICAL
      LOG_COLOR_RED,    // ERROR
      LOG_COLOR_YELLOW, // WARNING
      LOG_COLOR_GREEN,  // INFO
      LOG_COLOR_BLUE    // DEBUG
   };

   if (level >= 0 && level < XM_LOG_LEVELS)
      return colors[level];

   return "";
}

static unsigned int
XmLogPidGet(void)
{
   return (unsigned int)getpid();
}

static const char *
XmLogDomainStrGet(const char *name, const char *color)
{
   if (color)
   {
      size_t name_len  = strlen(name);
      size_t color_len = strlen(color);
      size_t reset_len = strlen(LOG_COLOR_RESET);

      char *d = malloc(color_len + name_len + reset_len + 1);
      if (!d) return NULL;

      memcpy(d, color, color_len);
      memcpy(d + color_len, name, name_len);
      memcpy(d + color_len + name_len, LOG_COLOR_RESET, reset_len);
      d[color_len + name_len + reset_len] = '\0';

      return d;
   }
   else
   {
      return strdup(name);
   }
}

// Domain creation
static XmLogDomain *
XmLogDomainNew(XmLogDomain *d, const char *name, const char *color)
{
   d->level      = XM_LOG_LEVEL_UNKNOWN;
   d->color      = color;
   d->deleted    = XM_FALSE;
   d->domain_str = XmLogDomainStrGet(name, color);
   d->name       = strdup(name);
   d->namelen    = strlen(name);

   return d;
}

// Domain cleanup
static void
XmLogDomainFree(XmLogDomain *d)
{
  free((char *)d->domain_str);
  free((char *)d->name);
}

// Domain registration
static int
XmLogDomainRegisterUnlocked(const char *name, const char *color)
{
   unsigned int i;

   // Find available slot
   for (i = 0; i < _log_domains_count; i++)
   {
      if (_log_domains[i].deleted)
      {
         XmLogDomainNew(&_log_domains[i], name, color);
         goto finish_register;
      }
   }

   // Need to expand the array
   if (_log_domains_count >= _log_domains_allocated)
   {
      size_t new_size = (!_log_domains) ? 64 : _log_domains_allocated + 8;

      XmLogDomain *tmp = realloc(_log_domains, sizeof(XmLogDomain) * new_size);
      if (!tmp) return -1;

      _log_domains = tmp;
      _log_domains_allocated = new_size;
   }

   // Use the new slot
   XmLogDomainNew(&_log_domains[i], name, color);
   _log_domains_count++;

finish_register:
   // Set default level
   if (_log_domains[i].level == XM_LOG_LEVEL_UNKNOWN)
   {
      _log_domains[i].level = _log_level;
   }

   return i;
}

// Main logging function
static inline void
XmLogPrintUnlocked(int domain, XmLogLevel level,
                  const char *file, const char *fnc,
                  int line, const char *fmt, va_list args)
{
  XmLogDomain *d;

  // Validate domain index
  if ((unsigned int)domain >= _log_domains_count || domain < 0)
  {
    fprintf(stderr, "CRI<%u>:XmLog %s:%d %s() unknown log domain %d\n",
            XmLogPidGet(), file, line, fnc, domain);
    return;
  }

  d = _log_domains + domain;

  // Check if domain was deleted
  if (d->deleted)
  {
    fprintf(stderr, "ERR<%u>:XmLog %s:%d %s() log domain %d was deleted\n",
            XmLogPidGet(), file, line, fnc, domain);
    return;
  }

  // Check if message should be filtered by level
  if ((!d->level) || (level > d->level)) return;

  // Call the print callback
  _print_cb(d, level, file, fnc, line, fmt, _print_cb_data, args);
}

// Public API functions
int
XmLogDomainRegister(const char *name, const char *color)
{
   int r;
   if (!name) return -1;

   LOG_LOCK();
   r = XmLogDomainRegisterUnlocked(name, color);
   LOG_UNLOCK();

   return r;
}

void
XmLogDomainUnregister(int domain)
{
  if (domain < 0) return;

  LOG_LOCK();
  if ((unsigned int)domain < _log_domains_count)
    _log_domains[domain].deleted = XM_TRUE;
  LOG_UNLOCK();
}

void
XmLogDomainLevelSet(const char *domain_name, int level)
{
   if (!domain_name) return;

   LOG_LOCK();
   for (unsigned int i = 0; i < _log_domains_count; i++)
   {
      if (!_log_domains[i].deleted &&
         strcmp(_log_domains[i].name, domain_name) == 0)
      {
         _log_domains[i].level = level;
         break;
      }
   }
   LOG_UNLOCK();
}

int
XmLogDomainLevelGet(const char *domain_name)
{
   if (!domain_name) return XM_LOG_LEVEL_UNKNOWN;

   LOG_LOCK();
   for (unsigned int i = 0; i < _log_domains_count; i++)
   {
      if (!_log_domains[i].deleted &&
         strcmp(_log_domains[i].name, domain_name) == 0)
      {
         int level = _log_domains[i].level;
         LOG_UNLOCK();
         return level;
      }
   }
   LOG_UNLOCK();
   
   return XM_LOG_LEVEL_UNKNOWN;
}

void
XmLogDomainRegisteredLevelSet(int domain, int level)
{
   if (domain < 0 || (unsigned int)domain >= _log_domains_count)
      return;
   
   LOG_LOCK();
   _log_domains[domain].level = level;
   LOG_UNLOCK();
}

int
XmLogDomainRegisteredLevelGet(int domain)
{
   if (domain < 0 || (unsigned int)domain >= _log_domains_count)
      return XM_LOG_LEVEL_UNKNOWN;

   return _log_domains[domain].level;
}

void
XmLogPrintCbSet(XmLogPrintCb cb, void *data)
{
   LOG_LOCK();
   if (cb)
      _print_cb = cb;
   else
      _print_cb = XmLogPrintCbStdErr;
   _print_cb_data = data;
   LOG_UNLOCK();
}

void
XmLogLevelSet(int level)
{
   _log_level = level;
   if (XM_LOG_DOMAIN_GLOBAL >= 0 &&
      (unsigned int)XM_LOG_DOMAIN_GLOBAL < _log_domains_count)
   {
      _log_domains[XM_LOG_DOMAIN_GLOBAL].level = level;
   }
}

int
XmLogLevelGet(void)
{
   return _log_level;
}

XmBool
XmLogThreadsMainCheck(void)
{
   return ((!_threads_enabled) || IS_MAIN(SELF()));
}

void
XmLogThreadsInit(void)
{
   if (_threads_inited) return;
   _main_thread = SELF();
   INIT();
}

void
XmLogThreadsShutdown(void)
{
   if (!_threads_inited) return;
   CHECK_MAIN();
   SHUTDOWN();
   _threads_enabled = XM_FALSE;
}

void
XmLogThreadsEnable(void)
{
   if (_threads_enabled) return;
   if (!_threads_inited) XmLogThreadsInit();
   _threads_enabled = XM_TRUE;
}

void
XmLogColorDisableSet(XmBool disabled)
{
   (void)disabled;
}

XmBool
XmLogColorDisableGet(void)
{
   return XM_FALSE;
}

void
XmLogFileDisableSet(XmBool disabled)
{
   (void)disabled;
}

XmBool
XmLogFileDisableGet(void)
{
   return XM_FALSE;
}

void
XmLogFunctionDisableSet(XmBool disabled)
{
   (void)disabled;
}

XmBool
XmLogFunctionDisableGet(void)
{
   return XM_FALSE;
}

void
XmLogPrintCbStdErr(const XmLogDomain *d, XmLogLevel level,
                  const char *file, const char *fnc, int line,
                  const char *fmt, void *data, va_list args)
{
   // Simple prefix printing
   const char *level_name =
      (level >= 0 && level < XM_LOG_LEVELS) ? _level_names[level] : "UNK";

   // Get colors
   const char *level_color = XmLogLevelColorGet(level);
   const char *domain_color = d->color ? d->color : "";
   const char *reset_color = LOG_COLOR_RESET;

   // Print colored level
   if (level_color) {
      fputs(level_color, stderr);
   }
   fputs(level_name, stderr);
   if (level_color) {
      fputs(reset_color, stderr);
   }

   // Print colored domain
   if (domain_color) {
      fputs(domain_color, stderr);
   }
   fprintf(stderr,
         "<%u>:%s %s:%d %s() ",
         XmLogPidGet(), d->name, file, line, fnc);
   if (domain_color) {
      fputs(reset_color, stderr);
   }

   vfprintf(stderr, fmt, args);
   putc('\n', stderr);

   (void)data;
}

void
XmLogPrintCbStdOut(const XmLogDomain *d, XmLogLevel level,
                   const char *file, const char *fnc, int line,
                   const char *fmt, void *data, va_list args)
{
   // Simple prefix printing
   const char *level_name =
      (level >= 0 && level < XM_LOG_LEVELS) ? _level_names[level] : "UNK";

   // Get colors
   const char *level_color  = XmLogLevelColorGet(level);
   const char *domain_color = d->color ? d->color : "";
   const char *reset_color  = LOG_COLOR_RESET;

   // Print colored level
   if (level_color) fputs(level_color, stdout);

   fputs(level_name, stdout);
   if (level_color) fputs(reset_color, stdout);

   // Print colored domain
   if (domain_color) fputs(domain_color, stdout);

   fprintf(stdout,
         "<%u>:%s %s:%d %s() ",
         XmLogPidGet(), d->name, file, line, fnc);
   if (domain_color) fputs(reset_color, stdout);

   vprintf(fmt, args);
   putchar('\n');

   (void)data;
}

void
XmLogPrintCbFile(const XmLogDomain *d, XmLogLevel level, const char *file,
                 const char *fnc, int line, const char *fmt, void *data,
                 va_list args)
{
   if (!data) return;

   FILE *f = (FILE *)data;

   // Simple prefix printing
   const char *level_name =
      (level >= 0 && level < XM_LOG_LEVELS) ? _level_names[level] : "UNK";

   // Get colors
   const char *level_color  = XmLogLevelColorGet(level);
   const char *domain_color = d->color ? d->color : "";
   const char *reset_color  = LOG_COLOR_RESET;

   // Print colored level
   if (level_color) fputs(level_color, f);

   fputs(level_name, f);
   if (level_color) fputs(reset_color, f);

   // Print colored domain
   if (domain_color) fputs(domain_color, f);

   fprintf(f, 
         "<%u>:%s %s:%d %s() ",
         XmLogPidGet(), d->name, file, line, fnc);
   if (domain_color) fputs(reset_color, f);

   vfprintf(f, fmt, args);
   putc('\n', f);
}


void
XmLogPrint(int domain, XmLogLevel level, const char *file, const char *fnc,
           int line, const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   XmLogPrintUnlocked(domain, level, file, fnc, line, fmt, args);
   va_end(args);
}

XmBool
XmLogInit(void)
{
   // Set default print callback based on configuration
#ifdef XM_DEFAULT_LOG_OUTPUT
   if (strcmp(XM_DEFAULT_LOG_OUTPUT, "stdout") == 0)
      _print_cb = XmLogPrintCbStdOut;
   else if (strcmp(XM_DEFAULT_LOG_OUTPUT, "stderr") == 0)
      _print_cb = XmLogPrintCbStdErr;
   else if (strcmp(XM_DEFAULT_LOG_OUTPUT, "file") == 0)
      _print_cb = XmLogPrintCbFile;
      // For file output, we need to set up a default log file
      _print_cb_data = (void*)"motif.log";
   else
      // Default fallback to stderr
      _print_cb = XmLogPrintCbStdErr;
#else
   // Fallback if XM_DEFAULT_LOG_OUTPUT is not defined
   _print_cb = XmLogPrintCbStdErr;
#endif

   // Register global domain
   XM_LOG_DOMAIN_GLOBAL = XmLogDomainRegister("XM", LOG_COLOR_RED);
   if (XM_LOG_DOMAIN_GLOBAL < 0)
   {
      fprintf(stderr, "Failed to create global logging domain.\n");
      return XM_FALSE;
   }

   return XM_TRUE;
}

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
XmBool
XmLogSetOutput(const char *output, const char *filename)
{
   if (!output) return XM_FALSE;

   if (strcmp(output, "stdout") == 0)
   {
      _print_cb = XmLogPrintCbStdOut;
      _print_cb_data = NULL;
   }
   else if (strcmp(output, "stderr") == 0)
   {
      _print_cb = XmLogPrintCbStdErr;
      _print_cb_data = NULL;
   }
   else if(strcmp(output, "file") == 0)
   {
      _print_cb = XmLogPrintCbFile;
      _print_cb_data = (void*)(filename ? filename : "motif.log");
   }
   else
   {
      return XM_FALSE;  // Invalid output type
   }

   return XM_TRUE;
}

// Shutdown logging system
XmBool
XmLogShutdown(void)
{
   // Clean up domains
   for (unsigned int i = 0; i < _log_domains_count; i++)
      if (!_log_domains[i].deleted) XmLogDomainFree(&_log_domains[i]);

   free(_log_domains);
   
   _log_domains            = NULL;
   _log_domains_count      = 0;
   _log_domains_allocated  = 0;

   return XM_TRUE;
}

