#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

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

	char *new_emp = NULL;
	
	int dbfd;

	while((c = getopt(argc, argv, "nf:a:")) != -1) {
		switch(c) {
			case 'n':
				new_file = true;
				printf("New file to be created\n");
				break;
			case 'f':
				file_path = optarg;
				printf("File to be opened: %s\n", file_path);
				break;
			case 'a':
				if(optarg != NULL) {
					new_emp = optarg;
				}
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

		//create the header
		if((create_db_header(dbfd, &header)) == -1) {
			printf("Unable to create the database header\n");
			close(dbfd);
			free(header);
			return -1;

		}
		//write to the new file
		if((output_file(dbfd, header, NULL)) == -1) {
			printf("Unable to write the database header\n");
			close(dbfd);
			free(header);
			return -1;
		}
		printf("Header written to new file: %d\n", dbfd);
		
	} else {
		printf("Trying to open existing file..\n");
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
		}
		printf("Existing employees: %d\n", header->count);
	}
	printf("File header: %d\n", dbfd);

	//reading the employees first
	struct employee_t *employees = {0};
	read_employees(dbfd, header, &employees); 

	if(new_emp != NULL) {
		header->count++;
		printf("New header count: %d\n", header->count);
			
		employees = realloc(employees, sizeof(struct employee_t)*header->count); 
		if(employees == NULL) {
			perror("realloc");
			return -1;
		}
		add_employee(header, employees, new_emp);
		output_file(dbfd, header, employees);
	}
}
