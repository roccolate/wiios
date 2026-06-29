#ifndef WIIOS_RINGLOG_H
#define WIIOS_RINGLOG_H

#include <stddef.h>

void ringlog_write(const char *msg);
const char *ringlog_data(void);
size_t ringlog_size(void);
void ringlog_dump_stdout(void);

#endif
