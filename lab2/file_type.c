//int stat(const char *restrict pathname, struct stat *restrict buf); -
// возвращает структуру с информацией о файле, указанном в ар- гументе pathname

//int fstat(int fd, struct stat *buf); -
//возвращает информацию об открытом файле, который определяется дескриптором fd

//int lstat(const char *restrict pathname, struct stat restrict buf); -
//похожа на функцию stat, но когда ей передается имя символической ссылки, она возвращает сведения о самой символической ссылке, а не о файле, на который она ссылается

//int fstatat(int fd, const char *restrict pathname, struct stat *restrict buf, int flag); -
//дает возможность получить информацию о файле, представ- ленном строкой относительного пути pathname, начинающегося в открытом ка- талоге, представленном дескриптором fd

//Макросы для определения типа файла из <sys/stat.h>
//S_ISREG() - Обычный файл
//S_ISDIR() - Каталог
//S_ISCHR() - Специальный файл символьного устройства
//S_ISBLK() - Специальный файл блочного устройств
//S_ISFIFO() - Канал (именованный или неименованный)
//S_ISLNK() - Символьная ссылка
//S_ISSOCK - Сокет

//Программа, представленная ниже, выводит тип файла для каждого аргумента командной строки
// По Стивенсу Раго (стр. 142)
#include "../apue.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <unistd.h>
//#include <limits.h>
//#include <string.h>
//#include <dirent.h>
//#include <sys/types.h>
//#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int i;
    struct stat buf;
    char *ptr;
   
    for (i = 1; i < argc; i++)
    {
       printf("%s: ", argv[i]);
       if (lstat(argv[i], &buf) < 0)
       {
           printf("Ошибка вызова функции lstat\n");
           continue;
           
       }
       if (S_ISREG(buf.st_mode))
           ptr = "Обычный файл";
       else if (S_ISDIR(buf.st_mode))
           ptr = "Каталог";
       else if (S_ISCHR(buf.st_mode))
           ptr = "Файл символьного устройства";
       else if (S_ISBLK(buf.st_mode))
           ptr = "Файл блочного устройства";
       else if (S_ISFIFO(buf.st_mode))
           ptr = "Fifo";
       else if (S_ISLNK(buf.st_mode))
           ptr = "Символическая ссылка";
       else if (S_ISSOCK(buf.st_mode))
           ptr = "Сокет";
       else
           ptr = "** Неизвестный тип файла **";
       printf("%s\n", ptr);
   }
    exit(0);
}
