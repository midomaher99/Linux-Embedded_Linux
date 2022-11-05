#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#define SECTOR_SIZE (512u)

#define MBR_PARTITION_TABLE_SIZE (66u)
#define MBR_PARTITION_TABLE_START (446u)
#define MBR_NUMBER_OF_PARTITIONS (4u)
#define EXTENDED_PARTITION_TYPE (0x5u)
#define GPT_PARTITION_TYPE (0xEEu)
#define GPT_NUMBER_OF_PARTITIONS (128u)
#define GPT_PARTITION_ENTRY_SIZE (128u)

typedef struct
{
	uint8_t status;
	uint8_t first_chs[3];
	uint8_t partition_type;
	uint8_t last_chs[3];
	uint32_t lba;
	uint32_t sector_count;
} MBR_partitionEntry_t;

typedef struct
{
	MBR_partitionEntry_t partition_entry[MBR_NUMBER_OF_PARTITIONS];
	uint16_t boot_signature;
} MBR_partitionTable_t;

typedef struct
{
	uint8_t partition_type[16];
	uint8_t unique_partition_GUID[16];
	uint64_t first_lba;
	uint64_t last_lba;
	uint64_t attribute_flags;
	uint8_t partition_name[72];
} GPT_partitionEntry_t;

void print_preamble();

int parse_MBR(MBR_partitionTable_t *p_MBR_partition_table,
			   char *disk_name, int disk_fd);

int parse_GPT(char *disk_name, int disk_fd);

int main(int argc, char *argv[])
{
	char buffer[MBR_PARTITION_TABLE_SIZE];
	int sdX_fd;

	MBR_partitionTable_t *p_MBR_partition_table = NULL;

	if ((sdX_fd = open(argv[1], O_RDONLY)) == -1)
	{
		printf("Error in opennimng %s\n", argv[1]);
		printf("errno= %d", errno);
		return -1;
	}

	if (lseek(sdX_fd, (off_t)MBR_PARTITION_TABLE_START, SEEK_SET) == -1) // seek to the table start
	{
		printf("Error in seeking to the partition table\n");
		printf("errno= %d", errno);
		return -2;
	}

	if (read(sdX_fd, (void *)buffer, MBR_PARTITION_TABLE_SIZE) == -1)
	{
		printf("Error in reading the partition table from %s\n", argv[1]);
		printf("errno= %d", errno);
		return -3;
	}

	p_MBR_partition_table = (MBR_partitionTable_t *)buffer;

	if(p_MBR_partition_table->boot_signature != 0xaa55)
	{
		printf("the partition table is corrupted (no boot signature)\n");
		return -4;
	}
	

	if (p_MBR_partition_table->partition_entry[0].partition_type == GPT_PARTITION_TYPE)
		parse_GPT(argv[1], sdX_fd);
	else
		parse_MBR(p_MBR_partition_table, argv[1], sdX_fd);

	if (close(sdX_fd) == -1)
	{
		printf("Error in closing %s\n", argv[1]);
		printf("errno= %d", errno);
		return -5;
	}
	return 0;
}

void print_preamble()
{
	printf("\n");
	printf("%-12s %4s %11s %11s %11s %6s %2s %-s\n",
		   "Device",
		   "Boot",
		   "Start",
		   "End",
		   "Sectors",
		   "Size",
		   "ID",
		   "Type");
}

int parse_MBR(	MBR_partitionTable_t *p_MBR_partition_table,
			   	char *disk_name,
			   	int disk_fd)
{

	int extended_index = -1;
	int count;
	char buffer[MBR_PARTITION_TABLE_SIZE]; // to read the EBR of extended partition if exist
	int ret_val =1;

	MBR_partitionTable_t *p_EBR_partition_table = NULL;
	print_preamble();
	for (count = 0; count < MBR_NUMBER_OF_PARTITIONS; count++)
	{
		if (p_MBR_partition_table->partition_entry[count].lba == 0)
			break;

		if (p_MBR_partition_table->partition_entry[count].partition_type ==
			EXTENDED_PARTITION_TYPE)
		{
			extended_index = count;
		}

		printf("%s%-3d %5c %11u %11u %11u %5.1fG %2x\n",
			   disk_name,
			   count + 1,
			   p_MBR_partition_table->partition_entry[count].status ==
					   0x80
				   ? '*'
				   : ' ',
			   p_MBR_partition_table->partition_entry[count].lba,
			   p_MBR_partition_table->partition_entry[count].lba +
				   p_MBR_partition_table->partition_entry[count].sector_count -
				   1,
			   p_MBR_partition_table->partition_entry[count].sector_count,
			   (float)(((long double)p_MBR_partition_table->partition_entry[count].sector_count *
						SECTOR_SIZE) /
					   (1024 * 1024 * 1024)),
			   p_MBR_partition_table->partition_entry[count].partition_type);
	}

	if (extended_index != -1) // extended partition exist
	{
		count = 5; // for partition naming

		off_t first_EBR_offset =
			p_MBR_partition_table->partition_entry[extended_index].lba;
		off_t current_EBR_offset = first_EBR_offset;

		do
		{
			if (lseek(disk_fd, (off_t)MBR_PARTITION_TABLE_START + current_EBR_offset * SECTOR_SIZE, SEEK_SET) == -1) // seek to the table start
			{
				printf("Error in seeking to the extended partition table\n");
				printf("errno= %d", errno);
				ret_val = -1;
				break;
			}
			if(read(disk_fd, (void *)buffer, MBR_PARTITION_TABLE_SIZE) == -1)
			{
				printf("Error in reading the extended partition table\n");
				printf("errno= %d", errno);
				ret_val = -2;
				break;
			}

			p_EBR_partition_table = (MBR_partitionTable_t *)buffer;

			printf("%s%-3d %5c %11lu %11lu %11u %5.1fG %2x\n", disk_name, count, p_EBR_partition_table->partition_entry[0].status == 0x80 ? '*' : ' ', // boot
				   p_EBR_partition_table->partition_entry[0].lba + current_EBR_offset,																  // start
				   p_EBR_partition_table->partition_entry[0].lba + p_EBR_partition_table->partition_entry[0].sector_count - 1 + current_EBR_offset,	  // end
				   p_EBR_partition_table->partition_entry[0].sector_count,																			  // sectors
				   (float)(((long double)p_EBR_partition_table->partition_entry[0].sector_count * 512) / (1024 * 1024 * 1024)),						  // size
				   p_EBR_partition_table->partition_entry[0].partition_type																			  // id
			);

			current_EBR_offset = p_EBR_partition_table->partition_entry[1].lba +first_EBR_offset;
			count++;

		} while (p_EBR_partition_table->partition_entry[1].lba != 0); // second entry not zero
	}
	return ret_val;
}

int parse_GPT(char *disk_name,
			  int disk_fd)
{
	int count;
	char buffer[128];
	GPT_partitionEntry_t *p_GPT_partition_entry;
	off_t partition_entry_offset = 2 * SECTOR_SIZE;
	int ret_val = 1;
	print_preamble();

	for (count = 0; count <= GPT_NUMBER_OF_PARTITIONS; count++)
	{
		if (lseek(disk_fd, partition_entry_offset, SEEK_SET) == -1)
		{
			printf("Error in seeking to the GPT partition table\n");
			printf("errno= %d", errno);
			ret_val = -1;
			break;
		}
		if (read(disk_fd, (void *)buffer, GPT_PARTITION_ENTRY_SIZE) == -1)
		{
			printf("Error in reading the GPT partition table\n");
			printf("errno= %d", errno);
			ret_val = -2;
			break;
		}

		p_GPT_partition_entry = (GPT_partitionEntry_t *)buffer;
		if (p_GPT_partition_entry->first_lba == 0)
			break;
		else
		{
			printf("%s%-3d %5c %11lu %11lu %11u %5.1fG\n",
				   disk_name,
				   count + 1,
				   ' ',																														 // boot
				   p_GPT_partition_entry->first_lba,																						 // start
				   p_GPT_partition_entry->last_lba,																							 // end
				   (unsigned int)(p_GPT_partition_entry->last_lba - p_GPT_partition_entry->first_lba) + 1,													 // sectors
				   (float)((long double)p_GPT_partition_entry->last_lba - p_GPT_partition_entry->first_lba + 1) * 512 / (1024 * 1024 * 1024) // size
			);
		}
		partition_entry_offset += GPT_PARTITION_ENTRY_SIZE;
	}
	return ret_val;
}
