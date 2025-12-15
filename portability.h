#ifndef PORTABILITY_H
#define PORTABILITY_H

#ifndef __NetBSD__

void
setprogname(const char *name);

const char *
getprogname(void);

#endif

#endif
