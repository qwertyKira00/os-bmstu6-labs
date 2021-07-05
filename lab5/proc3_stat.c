#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#define ALPHABET "Abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_SIZE 26
#define FILENAME "proc3_alphabet.txt"

void print_stat(char *fd, char *operation, struct stat statbuf)
{
	printf("%s_%s:\ninode: %d\nsize: %d\nI/O_block_size: %d\nLast modification of file data: %s\n", 
		fd, operation, (int)statbuf.st_ino, 
		(int)statbuf.st_size, (int)statbuf.st_blksize, ctime(&statbuf.st_mtime));

}

int main()
{
	struct stat statbuf;

	FILE *fd1 = fopen(FILENAME, "w");
	stat(FILENAME, &statbuf);
	print_stat("fd1", "fopen", statbuf);

	FILE *fd2 = fopen(FILENAME, "w");
	stat(FILENAME, &statbuf);
	print_stat("fd2", "fopen", statbuf);	

	for (int i = 0; i < ALPHABET_SIZE; i++)
	{
		if (ALPHABET[i] % 2)					//acegikmoqsuwy
			fprintf(fd1, "%c", ALPHABET[i]);
		else
			fprintf(fd2, "%c", ALPHABET[i]);
	}

	//Функция fclose отделяет указанный поток 
	//от связанного с ним файла.
	//Если поток использовался для вывода данных, то все данные, 
	//содержащиеся в буфере, сначала записываются с помощью fflush.  
	fclose(fd1);
	stat(FILENAME, &statbuf);
	print_stat("fd1", "fclose", statbuf);
	

	fclose(fd2);
	stat(FILENAME, &statbuf);
	print_stat("fd2", "fclose", statbuf);

	return 0;
}
