#include <stdio.h>
#include <string.h>
#include "config.h"
#include "base.h"
#include "http_request.h"
#include "modules/ds_daemon.h"


static int ds_version;
static int ds_help;
static int ds_port = 8484;
static char *user;

int ds_get_options(int argc, char *argv[]);


int main(int argc, char *argv[])
{
	struct http_conf conf;
	int isMasterProcess = 1;
	conf.web = NULL;
	conf.web_count = 0;
	conf.mimetype = NULL;


	ds_get_options(argc, argv);

	if(ds_version) {
		printf("devSync version: "_Version DS_LINEEND);
		return OK;
    }
	if(ds_help) {
		printf("devSync version: "_Version DS_LINEEND);
		printf("Usage: devsync [-vh]" DS_LINEEND
				"[-p port]" DS_LINEEND
				"Options:" DS_LINEEND
				"   -v            : show version and exit" DS_LINEEND
				"   -h            : this help" DS_LINEEND
		);

		return OK;
	}


	conf.port = ds_port;
	conf.user = user;

	ds_daemon(&conf);

	start_accept(&conf);




	return 0;
}



int ds_get_options(int argc, char *argv[])
{
	int i ;
	char *p;

	for(i = 1; i < argc; i++) {
		p = (char *)argv[i];

		if(*p++ != '-') {
			printf("invalid option: \"%s\"\n", argv[i]);
		}
		while(*p) {
			switch(*p++) {
				case 'p':
					if(atoi(argv[++i]) > 0) {
						ds_port = atoi(argv[i]);
					}
					break;
				case 'v':
					ds_version = 1;
				break;
				case 'h':
					ds_version = 1;
					ds_help = 1;
				break;
				case 'u':
					user = argv[++i];
				break;
			}
		}
	}

	return OK;
}

