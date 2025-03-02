#include <stdio.h>
#include <string.h>
#include <stdint.h>

uint8_t debug = 0;
uint8_t memory[65536];

int parse_hex_line(char *theline, int bytes[], int *addr, int *num, int *code)
{
	int sum, len, cksum;
	char *ptr;
	
	*num = 0;
	if (theline[0] != ':') return 0;
	if (strlen(theline) < 11) return 0;
	ptr = theline+1;
	if (!sscanf(ptr, "%02x", &len)) return 0;
	ptr += 2;
	if ( strlen(theline) < (size_t)(11 + (len * 2)) ) return 0;
	if (!sscanf(ptr, "%04x", addr)) return 0;
	ptr += 4;
	  /* printf("Line: length=%d Addr=%d\n", len, *addr); */
	if (!sscanf(ptr, "%02x", code)) return 0;
	ptr += 2;
	sum = (len & 255) + ((*addr >> 8) & 255) + (*addr & 255) + (*code & 255);
	while(*num != len) {
		if (!sscanf(ptr, "%02x", &bytes[*num])) return 0;
		ptr += 2;
		sum += bytes[*num] & 255;
		(*num)++;
		if (*num >= 256) return 0;
	}
	if (!sscanf(ptr, "%02x", &cksum)) return 0;
	if ( ((sum & 255) + (cksum & 255)) & 255 ) return 0; /* checksum error */
	return 1;
}

/* loads an intel hex file into the global memory[] array */
/* filename is a string of the file to be opened */
int load_hex_file(const char *filename)
{
	char line[1000];
	FILE *fin;
	int addr, n, status, bytes[256];
	int i, total=0, lineno=1;
	int minaddr=65536, maxaddr=0;

	if (strlen(filename) == 0) {
		return -1;
	}
	fin = fopen(filename, "r");
	if (fin == NULL) {
		printf("   Can't open file '%s' for reading.\n", filename);
		return -1;
	}
	while (!feof(fin) && !ferror(fin)) {
		line[0] = '\0';
		fgets(line, 1000, fin);
		if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
		if (line[strlen(line)-1] == '\r') line[strlen(line)-1] = '\0';
		if (parse_hex_line(line, bytes, &addr, &n, &status)) {
			if (status == 0) {  /* data */
				for(i=0; i<=(n-1); i++) {
					memory[addr] = bytes[i] & 0xFF;
					total++;
					if (addr < minaddr) minaddr = addr;
					if (addr > maxaddr) maxaddr = addr;
					addr++;
				}
			}
			if (status == 1) {  /* end of file */
				fclose(fin);
				printf("   Loaded %d bytes between:", total);
				printf(" %04X to %04X\n", minaddr, maxaddr);
                if (debug)
                {
                    for (int i = minaddr; i <= maxaddr; i++)
                    {
                        printf("%02X ", memory[i]);
                    }
                    printf("\n");
                }
				return maxaddr + 1;
			}
			if (status == 2) {}  /* begin of file */
		} else {
			printf("   Error: '%s', line: %d\n", filename, lineno);
		}
		lineno++;
	}
    return -1;
}

const char* get_hex_data_mem()
{
	return (const char*)memory;
}

void dump_memory(uint8_t*memory, unsigned int mem_len)
{
	int row_idx = 0;
	int row_max = 6;
	int col_idx = 0;
	int col_max = 32;
	row_max = mem_len / col_max + 1;
	printf("mem:%p, len:%lu\n", memory, mem_len);
	for (col_idx = 0; col_idx < col_max; col_idx++)
	{
		printf("%02X ", col_idx);
	}
	printf("\n");
	for (row_idx = 0; row_idx < row_max; row_idx++)
	{
		printf("[ %d:%02X ]", row_idx, row_idx);
		for (col_idx = 0; col_idx < col_max && ( (row_idx * col_max + col_idx) < mem_len); col_idx++)
		{
			printf("%02X ", ((uint8_t*)memory)[row_idx * col_max + col_idx]);
		}
		printf("\n");
	}
}

void dump_hex_file(uint8_t* memory, unsigned int mem_len)
{
	int row_idx = 0;
	int row_max = 6;
	int col_idx = 0;
	int col_max = 16;
	row_max = mem_len / col_max + 1;
	FILE* fp = fopen("test.hex", "wb");
	fprintf(fp, ":%08X\n", mem_len);
	for (row_idx = 0; row_idx < row_max; row_idx++)
	{
		fprintf(fp, ":%02X%04X%02X", col_max, (unsigned short)(col_max * row_idx), (unsigned char)0);
		for (col_idx = 0; col_idx < col_max && ((row_idx * col_max + col_idx) < mem_len); col_idx++)
		{
			fprintf(fp, "%02X", ((uint8_t*)memory)[row_idx * col_max + col_idx]);
		}
		fprintf(fp, "%02X\n", (unsigned char)row_idx);
	}
}