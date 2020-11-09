
#include <sys/printk.h>

int tfm_log_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);

	return 0;
}