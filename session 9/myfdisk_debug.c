#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
typedef struct{
	uint8_t status;
	uint8_t first_CHS[3];
	uint8_t part_type;
	uint8_t last_CHS[3];
	uint32_t  lba;
	uint32_t sector_count;

}partition_table;

typedef struct{
	uint64_t first_lba;
	uint64_t last_lba;

}GPT_partition_table;

#define Bold_on "\e[1m"
#define Bold_off "\e[m"

void printf_partition_entry(int partition_index, partition_table part_entry, char *hard_disk, int sector_offset);
void printf_GPT_partition_entry(int partition_index, GPT_partition_table GPT_part_entry, char *hard_disk);

int main(int argc, char *argv[])
{
	if(argc<1)
	{
		printf("Error no disk provided");
		return -1;
	}

	char buff[512];
	int fd = open(argv[1], O_RDONLY);
	int char_read = read(fd, buff, 512);
	if(char_read==-1)
	{
		printf("Couldn't open the file\n");
		return -1;
	}

	if(buff[510] != 0x55 || (buff[511]&0xff) != 0xaa)
	{
		printf("Not a partition table\n");
		return -1;
	}

	partition_table *part_table =(partition_table*) &buff[446];

	if(part_table[0].part_type == 0xee)
	{
		printf("GPT partition table\n");
		printf(Bold_on "%-10s %-10s %-10s %-10s %-6s\n" Bold_off, "Device", "Start", "End", "Sectors", "Size");

		uint32_t sector_offset = 2;
                uint32_t bytes_offset = lseek(fd, sector_offset*512, SEEK_SET);
		
		if(bytes_offset==-1)
		{
			printf("Error couldn't get the GPT partition table");
			return -1;
		}
		
		char buff[128];
		int nchar;
		for(int i=0; i<128; i++)
		{
			nchar = read(fd, buff, 128);
			
			if(nchar==-1)
			{
				printf("Couldn't read the GPT  partition table");
				return -1;
			}

			GPT_partition_table *gpt_part_table = (GPT_partition_table*) &buff[32];
			if((*gpt_part_table).first_lba==0)break;
			printf_GPT_partition_entry(i, *gpt_part_table, argv[1]);
		}

	}

	else{
		printf(Bold_on "%-10s %-5s %-10s %-10s %-10s %-6s %-10s\n" Bold_off, "Device", "Boot", "Start", "End", "Sectors", "Size", "ID");

		for(int i=0; i<4; i++)
		{	
			if(part_table[i].sector_count==0)continue;	

			printf_partition_entry(i, part_table[i], argv[1], 0);

			if(part_table[i].part_type == 0xf)
			{
				uint32_t sector_offset = part_table[i].lba;
				uint32_t bytes_offset = lseek(fd, sector_offset*512, SEEK_SET);

				for(int j=i+1; ; j++)
				{
					if(bytes_offset==-1)
					{
						printf("Error to get the extended partition table location");
						return -1;
					}

					char ext_buff[512];
					int nchar = read(fd, ext_buff, 512);
					if(nchar==-1)
					{
						printf("Couldn't read the extended partition");
						return -1;
					}

					partition_table *ext_part = (partition_table*) &ext_buff[446];
					if(ext_part[0].sector_count==0)break;
					printf_partition_entry(j, ext_part[0], argv[1], sector_offset);

					if(ext_part[1].sector_count==0)break;

					sector_offset = part_table[i].lba + ext_part[1].lba;
					bytes_offset  = lseek(fd, sector_offset*512, SEEK_SET);
				}

			}

		}
	}
	return 0;
}


void printf_partition_entry(int partition_index, partition_table part_entry, char *hard_disk, int sector_offset)
{
	printf("%-8s%-2d %-5c %-10u %-10u %-10u %-3uM   %-x\n",
			hard_disk,partition_index+1,
			part_entry.status == 0x80 ? '*' : ' ',
			sector_offset + part_entry.lba,
			sector_offset + part_entry.lba + part_entry.sector_count - 1,
			part_entry.sector_count,
			part_entry.sector_count*512 /(1024 * 1024),
			part_entry.part_type );

}

void printf_GPT_partition_entry(int partition_index, GPT_partition_table GPT_part_entry, char *hard_disk)
{
	uint64_t sector_count = GPT_part_entry.last_lba - GPT_part_entry.first_lba + 1;
        printf("%-8s%-2d %-10lu %-10lu %-10lu %-3luM\n",
                        hard_disk,partition_index+1,
                        GPT_part_entry.first_lba,
                        GPT_part_entry.last_lba,
                        sector_count,
                        sector_count*512 /(1024 * 1024));

}



