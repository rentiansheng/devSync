#include "so_dlfcn.h"


void
get_function(so_t *so)
{
	so->dp = dlopen(so->path, RTLD_LAZY);

	if(so->dp == NULL) return NULL;

	so->f = dlsym(so->dp, so->function);
}

void 
close_so(so_t *so)
{
	dlclose(so->dp);
}
