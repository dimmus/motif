# Logging

libXm contains a small logging facility, the `XmLog` functions declared
in `<Xm/Log.h>`, which applications can use for their own diagnostics.
It is always built in.  Motif itself does not log through it: its
warnings still go through `XtAppWarningMsg`.  The manual page
`XmLog(3)` describes every function.

## Build-time defaults

Two CMake cache variables set the defaults that `XmLogInit()` uses:

| Variable | Values | Default |
|----------|--------|---------|
| `LOG_LEVEL` | `DEBUG`, `INFO`, `WARN`, `ERROR`, `CRITICAL` | `INFO` |
| `LOG_OUTPUT` | `stderr`, `stdout`, `file` | `stderr` |

```sh
cmake -S . -B _build -DLOG_LEVEL=DEBUG -DLOG_OUTPUT=stdout
```

They become the compile definitions `XM_DEFAULT_LOG_LEVEL` and
`XM_DEFAULT_LOG_OUTPUT` of libXm.  With `file`, messages are appended to
`motif.log` in the current directory.

## Use

```c
#include <Xm/Log.h>

int main(void)
{
    int dom;

    if (!XmLogInit())          /* must come first */
        return 1;

    dom = XmLogDomainRegister("myapp", LOG_COLOR_GREEN);
    XmLogDomainRegisteredLevelSet(dom, XM_LOG_LEVEL_DBG);

    XM_LOG_DOM_INFO(dom, "started with %d arguments", 0);
    XM_LOG_WARN("this goes to the global \"XM\" domain");

    XmLogSetOutput("file", "myapp.log");   /* the string is not copied */
    XM_LOG_DOM_ERR(dom, "written to myapp.log");

    XmLogShutdown();
    return 0;
}
```

Messages have one of the levels `XM_LOG_LEVEL_CRITICAL` (0),
`XM_LOG_LEVEL_ERR`, `XM_LOG_LEVEL_WARN`, `XM_LOG_LEVEL_INFO` and
`XM_LOG_LEVEL_DBG` (4).  A message is printed when its level is at most
the level of its domain; new domains start at the default level, which
`XmLogLevelSet()` changes.  A domain at level 0 prints nothing.

Each message is printed as

```
INF<1234>:myapp main.c:12 main() started with 0 arguments
```

with the level in the color of its level and the part from `<pid>` to
`main()` in the color of the domain, if it has one, each followed by a
color reset sequence, whatever the destination.
`XmLogPrintCbSet()` replaces the printing with a callback of your own.

## Limitations

- The functions do no locking and must not be called from several
  threads at once; `XmLogThreadsEnable()` only records which thread is
  the main one.
- `XmLogColorDisableSet()`, `XmLogFileDisableSet()` and
  `XmLogFunctionDisableSet()` have no effect.
- File output opens and closes the file for every message.
