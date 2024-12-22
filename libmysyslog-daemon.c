/* 
 * Пример работы демона с использованием mysyslog для логирования.
 * Считывает конфигурацию и периодически записывает сообщения в журнал.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "libmysyslog.h"

#define CONFIG_FILE "/etc/mysyslog/mysyslog.cfg"

/* Глобальная переменная для обработки завершающих сигналов */
volatile sig_atomic_t stop = 0;

/* Обработчик сигналов для безошибочного завершения работы демона*/
void
handle_signal (int signal)
{
  stop = 1;
}

/* 
 * Считывает файл конфигурации.
 * Извлекает значения PATH для журнала, формата и драйвера.
 */
void
read_config (char *path, int *format, int *driver)
{
  FILE *file;

  file = fopen (CONFIG_FILE, "r");
  if (file == NULL)
    {
      perror ("fopen");
      exit (EXIT_FAILURE);
    }

  if (fscanf (file, "path=%s\nformat=%d\ndriver=%d\n", path, format, driver) != 3)
    {
      fprintf (stderr, "Неверный формат конфигурационного файла\n");
      fclose (file);
      exit (EXIT_FAILURE);
    }

  fclose (file);
}

/* 
 * Основная функция для инициализации демона, чтения конфигурации
 * и периодической записи сообщений в журнал.
 */
int
main (void)
{
  char path[256];
  int format;
  int driver;

  signal (SIGTERM, handle_signal);
  signal (SIGINT, handle_signal);

  read_config (path, &format, &driver);

  while (!stop)
    {
      mysyslog ("Запись в журнал демона", INFO, driver, format, path);
      sleep (5);
    }

  return 0;
}
