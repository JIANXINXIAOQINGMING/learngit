#include <stdint.h>
#include <sys/types.h>

void hexdump(const void *_data, size_t size) {
	char lt = 0x2E;

	const uint8_t *data = (const uint8_t *)_data;
	size_t offset = 0;

	//printf("\rbase addr: 0x%08X\n  ", _data);
	while (offset < size) {
		//printf("\r0x%04X  ", (uint32_t)offset);
		printf("\r0x%08X  ", (uint32_t)(long)(data+offset));
		size_t n = size - offset;
		if (n > 16) {
			n = 16;
		}
		for (size_t i = 0; i < 16; ++i) {
			if (i == 8) {
			printf(" ");
			}

			if (offset + i < size) {
				printf("%02X ", data[offset + i]);
			} else {
				printf("   "); 
			}
		}
		printf(" ");
		for (size_t i = 0; i < n; ++i) {
			if ( (data[offset + i]>=32) && (data[offset + i]<=126) ) {//ASCII范围
				printf("%c", data[offset + i]);
			} else {
				printf("%c",lt);
			}
		}
		offset += 16;
		printf("\n");
	}
	printf("\n");
}