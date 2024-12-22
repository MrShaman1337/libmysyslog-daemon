### Описание программы `mysyslog-daemon`

`mysyslog-daemon` — это демон для периодической записи логов с использованием библиотеки `libmysyslog`. Демон считывает конфигурацию из файла, обрабатывает сигналы для корректного завершения и записывает логи в заданном формате.

## Описание

Программа предназначена для записи логов в фоновом режиме. Она использует настройки, указанные в конфигурационном файле, такие как путь к файлу лога, формат (текстовый или JSON) и драйвер. Демон корректно обрабатывает сигналы завершения (например, SIGTERM и SIGINT), что позволяет ему безопасно завершать работу.

### Прототип основных функций

```c
void handle_signal (int signal);
void read_config (char *path, int *format, int *driver);
```

### Основные функции

- `handle_signal` (void):  
  Обрабатывает сигналы завершения (например, SIGTERM, SIGINT) и устанавливает глобальный флаг для выхода из основного цикла.

- `read_config` (void):  
  Считывает конфигурацию из файла и заполняет параметры: путь к файлу, формат и драйвер.

### Формат конфигурационного файла

Файл конфигурации должен быть в следующем формате:

```
path=/var/log/myapp.log
format=1
driver=2
```

- `path`: путь к файлу лога.
- `format`: формат лога (0 — текстовый, 1 — JSON).
- `driver`: идентификатор драйвера.

### Возвращаемое значение

Программа завершает работу с кодом:

- `0`, если работа завершена успешно.
- Ненулевое значение, если произошла ошибка (например, ошибка чтения конфигурационного файла).

---

### Пример использования

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "libmysyslog.h"

#define CONFIG_FILE "/etc/mysyslog/mysyslog.cfg"

volatile sig_atomic_t stop = 0;

void handle_signal (int signal)
{
  stop = 1;
}

void read_config (char *path, int *format, int *driver)
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
      fprintf (stderr, "Invalid configuration file format\n");
      fclose (file);
      exit (EXIT_FAILURE);
    }

  fclose (file);
}

int main (void)
{
  char path[256];
  int format;
  int driver;

  signal (SIGTERM, handle_signal);
  signal (SIGINT, handle_signal);

  read_config (path, &format, &driver);

  while (!stop)
    {
      mysyslog ("Daemon log entry", INFO, driver, format, path);
      sleep (5);
    }

  return 0;
}
```


---

С помощью `mysyslog-daemon` можно организовать системное логирование в фоновом режиме, что особенно полезно для долгоживущих приложений и служб.

