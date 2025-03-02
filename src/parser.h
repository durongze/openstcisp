
int parse_hex_line(char *theline, int bytes[], int *addr, int *num, int *code);
/* loads an intel hex file into the global memory[] array */
/* filename is a string of the file to be opened */
int load_hex_file(const char *filename);
const char* get_hex_data_mem();
void dump_memory(uint8_t* memory, unsigned int mem_len);
void dump_hex_file(uint8_t* memory, unsigned int mem_len);