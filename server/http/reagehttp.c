#include <stdio.h>
#include <string.h>
#include "config.h"
#include "base.h"
#include "http_request.h" 

int main(int argc, char *argv[]) {
	struct http_conf conf;
	int i = 1;

	conf.web = NULL;
	conf.web_count = 0;
	conf.mimetype = NULL;
	conf.port = 8484;

	while(i < argc) {
		if(strcasecmp(argv[i],"-p") == 0) {
			i++;
			if(atoi(argv[i]) > 0) {
				conf.port = atoi(argv[i]);
			}
		}
		i++;
	}

	start_accept(&conf);

	return 0;
}
