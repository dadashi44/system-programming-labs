/*
 * Лабораторная работа 1. Файлы и каталоги (средний вариант).
 * Программа рекурсивно удаляет каталог со всем содержимым.
 *
 * Для работы с файлами и каталогами используются только вызовы
 * из раздела 1.2 методички: scandir, unlink, rmdir.
 *
 * Сборка:  cc -Wall -Wextra -o remove remove.c
 * Запуск:  ./remove [каталог]      (по умолчанию удаляется "a")
 */
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Удалить path, чем бы он ни был.
 *
 * Сначала пробуем unlink: он удаляет обычные файлы, жёсткие ссылки
 * и символьные ссылки (саму ссылку, а не то, на что она указывает),
 * поэтому внутрь каталога по символьной ссылке мы никогда не зайдём.
 * Если unlink не сработал, значит перед нами каталог: удаляем всё его
 * содержимое и затем сам каталог через rmdir.
 *
 * Возвращает 0 в случае успеха и -1 в случае ошибки.
 */
static int remove_tree(const char *path)
{
    if (unlink(path) == 0)
        return 0;
    int unlink_errno = errno;

    struct dirent **list;
    int n = scandir(path, &list, NULL, alphasort);
    if (n == -1) {
        /* Не каталог: значит, настоящей была ошибка unlink. */
        if (errno == ENOTDIR)
            errno = unlink_errno;
        fprintf(stderr, "не удалось удалить %s: %s\n", path, strerror(errno));
        return -1;
    }

    int result = 0;
    for (int i = 0; i < n; i++) {
        const char *name = list[i]->d_name;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            char child[PATH_MAX];
            int len = snprintf(child, sizeof(child), "%s/%s", path, name);
            if (len < 0 || (size_t)len >= sizeof(child)) {
                fprintf(stderr, "слишком длинный путь: %s/%s\n", path, name);
                result = -1;
            } else if (remove_tree(child) == -1) {
                result = -1;
            }
        }
        free(list[i]);
    }
    free(list);

    if (result == -1)
        return -1;

    if (rmdir(path) == -1) {
        fprintf(stderr, "rmdir(%s): %s\n", path, strerror(errno));
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "a";
    return remove_tree(path) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
