#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "[-] Usage: %s INPUT_FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Get file size for later mapping into memory
	struct stat file_stat;
	if (stat(argv[1], &file_stat) == -1)
	{
		perror("stat");
		return EXIT_FAILURE;
	}
	printf("[+] File is %lu bytes\n", file_stat.st_size);
	
	// Open file for reading/writing
	FILE *fd = fopen(argv[1], "r+");
	if (fd == NULL)
	{
		perror("fopen");
		return EXIT_FAILURE;
	}
	printf("[+] Got a file handle\n");

	// Map file into memory
	char *file_map = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fd), 0);
	if (file_map == MAP_FAILED)
	{
		perror("mmap");
		return EXIT_FAILURE;
	}
	printf("[+] File mapped into memory\n");

	// Manipulate endian bit
	Elf64_Ehdr *elf_header = (Elf64_Ehdr*)file_map;
	switch (elf_header->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
			printf("[+] Currently little-endian, flipping to big\n");
			elf_header->e_ident[EI_DATA] = ELFDATA2MSB;
			break;
		case ELFDATA2MSB:
			printf("[+] Currently big-endian, flipping to little\n");
			elf_header->e_ident[EI_DATA] = ELFDATA2LSB;
			break;
		default:  // Set to LSB in event of no previous labelling
			elf_header->e_ident[EI_DATA] = ELFDATA2LSB;
			break;
	}

	// Cleaning up
	printf("[+] Cleaning up\n");
	msync(file_map, file_stat.st_size, MS_SYNC);
	munmap(file_map, file_stat.st_size);
	fclose(fd);

	return EXIT_SUCCESS;
}
