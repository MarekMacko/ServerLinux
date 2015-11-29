#include "conf_reader.h"

void error(const char* msg, config_t* cfg) {
	printf("Error: %s", msg);
	config_destroy(cfg);
}

int read_settings(const char *config_file_name, serv_sett *ss){
	config_t cfg;
	
	/*Initialization*/
	config_init(&cfg);
	
	/*Read the file. If there is an error, report it and exit*/
	if(!config_read_file(&cfg, config_file_name)){
		printf("%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		error("No file found\n", &cfg);
		return -1;
	} else {
		printf("Read settings successfully\n");
	}
	
	if(config_lookup_int(&cfg, "port", &ss->port)){
		printf("Port = %d\n", ss->port);
	} else {
		error("No 'port' setttings in configuration file\n", &cfg);
		return -1;
	}
	
	if(config_lookup_int(&cfg, "max_clients", &ss->max_clients)){
		printf("Max clients = %d\n", ss->max_clients);
	} else {
		error("No 'max clients' setttings in configuration file\n", &cfg);
		return -1;
	}
	return 0;
}
