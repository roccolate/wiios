void ringlog_write(const char *msg);
void os_log_write(const char *msg) { ringlog_write(msg); }
