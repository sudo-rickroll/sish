# SISH - Simple Shell

## Overview
This is a simple command-line interpreter that implements a subset of Unix shell functionality as specified in the manual page. It is portable across NetBSD 10.x, Fedora 43, and OmniOS r151044. It has been tested against the examples in the manual page, along with a few additional tests that have been (un)successful.

## Portability

#### Program Name Handling
- **NetBSD**: Uses built-in `setprogname()`/`getprogname()`
- **Other platforms**: Custom implementation in `portability.c`

#### Compilation Flags
- **NetBSD**: `-D_NETBSD_SOURCE`
- **OmniOS**: `-D_XOPEN_SOURCE=700 -D__EXTENSIONS__` with `-lsocket -lnsl`
- **Linux**: `-D_GNU_SOURCE`

## Additional Testing Performed

Beyond the manual page examples:
```bash
make
./sish
sish$ <file  # Behaves like a normal shell does
sish$ cd .. | echo hi
sish$ echo hi | cd ..
sish$ echo $SHELL$HOME # Parses as env variable SHELL$HOME
sish$ ls -l | less          
```
