#include <stdio.h>
#include "config.h"
#include "base.h"
#include "http_request.h" 

int main(int argc, char *argv[]) {
	struct http_conf conf;

	conf.web = NULL;
	conf.web_count = 0;
	conf.mimetype = NULL;
	conf.port = 8484;
	start_accept(&conf);

	return 0;
}
