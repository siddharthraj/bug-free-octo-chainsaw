#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -f <filename>\n", argv[0]);
	printf("\t-n\t - Create a new file\n");
	
	return;
}

int main(int argc, char *argv[]) { 
	bool new_file = false;
	char *file_path = NULL;
	int c;

	struct dbheader_t *header = NULL;
	
	int dbfd;

	while((c = getopt(argc, argv, "nf:a")) != -1) {
		switch(c) {
			case 'n':
				new_file = true;
				break;
			case 'f':
				file_path = optarg;
				break;
			case 'a':
				printf("Add a new record\n");
				break;
			default:
				return -1;

		}
	}

	if(file_path == NULL) {
		printf("Database filename has to be provided\n");
		print_usage(argv);
		return 0;
	}

	if(new_file) {
		dbfd = create_db_file(file_path);
		if(dbfd == STATUS_ERROR) {
			printf("Unable to create the database file\n");
			free(header);
			return -1;
		}

		//validate the header
		header = calloc(1, sizeof(struct dbheader_t));
		if(header == NULL) {
			perror("calloc");
			return -1;
		}
		header->version = 1;
		header->filesize = sizeof(struct dbheader_t);
		header->count = 0;
		header->magic = HEADER_MAGIC;
		if((output_file(dbfd, header, NULL)) == -1) {
			printf("Unable to write the database header\n");
			close(dbfd);
			free(header);
			return -1;
		}
		
	} else {
		dbfd = open_db_file(file_path);
		if(dbfd == STATUS_ERROR) {
			printf("Unable to open the database file\n");
			free(header);
			return -1;
		}
		
		if((validate_db_header(dbfd, &header)) == -1) {
			printf("Unable to validate the db header\n");
			close(dbfd);
			free(header);
			return -1;
		} else {
			printf("Database header is Ok!\n");
			return 0;
		}
	}
}
