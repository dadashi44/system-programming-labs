/*
 * Лабораторная работа 1. Файлы и каталоги.
 * Программа создаёт заданный каталог со всем содержимым.
 *
 * Для работы с файлами и каталогами используются только вызовы
 * из раздела 1.2 методички: open, close, read, write, ftruncate,
 * link, symlink, mkdir.
 *
 * Сборка:  cc -Wall -Wextra -o create create.c
 * Запуск:  ./create
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Сообщить об ошибке и завершить программу. */
static void die(const char *what, const char *path)
{
    fprintf(stderr, "%s(%s): ", what, path);
    perror("");
    exit(EXIT_FAILURE);
}

/* Записать в файл ровно count байт (write может записать меньше). */
static void write_all(int fd, const char *buf, size_t count, const char *path)
{
    while (count > 0) {
        ssize_t n = write(fd, buf, count);
        if (n == -1)
            die("write", path);
        buf += n;
        count -= (size_t)n;
    }
}

/* Создать новый файл (или очистить существующий) и открыть на запись. */
static int create_file(const char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1)
        die("open", path);
    return fd;
}

/* Каталог. */
static void make_dir(const char *path)
{
    if (mkdir(path, S_IRWXU) == -1)
        die("mkdir", path);
}

/* Пустой файл. */
static void make_empty(const char *path)
{
    close(create_file(path));
}

/* Файл с заданной строкой. */
static void make_text(const char *path, const char *text)
{
    int fd = create_file(path);
    write_all(fd, text, strlen(text), path);
    close(fd);
}

/* Файл размером size байт, заполненный нулями. */
static void make_zero(const char *path, off_t size)
{
    int fd = create_file(path);
    if (ftruncate(fd, size) == -1)
        die("ftruncate", path);
    close(fd);
}

/* Файл размером size байт со случайными данными. */
static void make_random(const char *path, size_t size)
{
    char buf[4096];
    int src = open("/dev/urandom", O_RDONLY);
    if (src == -1)
        die("open", "/dev/urandom");

    int dst = create_file(path);
    while (size > 0) {
        size_t want = size < sizeof(buf) ? size : sizeof(buf);
        ssize_t n = read(src, buf, want);
        if (n <= 0)
            die("read", "/dev/urandom");
        write_all(dst, buf, (size_t)n, path);
        size -= (size_t)n;
    }
    close(dst);
    close(src);
}

/* Символьная ссылка linkpath, указывающая на target.
 * target записывается в ссылку как есть и отсчитывается
 * от каталога, в котором лежит сама ссылка. */
static void make_symlink(const char *target, const char *linkpath)
{
    if (symlink(target, linkpath) == -1)
        die("symlink", linkpath);
}

/* Жёсткая ссылка newpath на существующий файл oldpath.
 * Оба пути отсчитываются от рабочего каталога программы. */
static void make_hardlink(const char *oldpath, const char *newpath)
{
    if (link(oldpath, newpath) == -1)
        die("link", newpath);
}

int main(void)
{
    /*
     * Структура из примера в методичке:
     *
     * a/b/cat.txt   ("catts")
     *  /c/dog.txt   (пустой)
     *  /d           (ссылка на /c)
     *  /e/f.txt     (жёсткая ссылка на b/cat.txt)
     *    /g.bin     (500 байт, случайный)
     *    /h.bin     (35 байт, заполнен 0)
     */
    make_dir("a");
    make_dir("a/b");
    make_dir("a/c");
    make_dir("a/e");

    make_text("a/b/cat.txt", "catts\n");
    make_empty("a/c/dog.txt");
    make_symlink("c", "a/d");
    make_hardlink("a/b/cat.txt", "a/e/f.txt");
    make_random("a/e/g.bin", 500);
    make_zero("a/e/h.bin", 35);

    return 0;
}
