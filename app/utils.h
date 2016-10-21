#ifndef APP_UTILS_H
#define	APP_UTILS_H

char *itoa(long long int value, char *str, int base);
void console_put_string(unsigned char colour, const char *string, unsigned int x, unsigned int y);
void console_put_data(int colour, char data, int repeat, unsigned int offset);
size_t strlen(const char *str);

#endif