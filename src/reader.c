#include<stdio.h>
#include<stdlib.h>
#include<libconfig.h>

struct server_settings;

int read_settings(const char *config_file_name, struct server_settings *serv_sett){
	config_t cfg;
    config_setting_t *settings;
  
    /*Initialization*/
    config_init(&cfg);
	/*Read the file. If there is an error, report it and exit*/
	if(!config_read_file(&cfg, config_file_name)){
		printf("%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return -1;
	} else {
		printf("Read settings successfully\n");
	}   
		     
	if(config_lookup_int(&cfg, "port", &serv_sett->port)){
		printf("Port = %d\n", serv_sett->port);
	} else {
		printf("No 'port' setttings in configuration file\n");
		return -2;
	}   
	     
	if(config_lookup_int(&cfg, "max_clients", &serv_sett->max_clients)){
		printf("Max clients = %d\n", serv_sett->max_clients);
	} else {
	    printf("No 'max clients' setttings in configuration file\n");
    	return -2;
	}   
	return 0;
}
