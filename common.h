#pragma once

#include <sys/types.h>

ssize_t send_all(int s, char *buf, size_t len);
ssize_t receive_all(int s, char *buf, size_t len);
