#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <link.h>
static void dump_link_map(struct link_map* map) {
  struct link_map* curr = map;

  while(curr) {
    fprintf(stderr, "[link_map] dso: %s, loaded at: %p, _DYNAMIC: %p\n",
	    curr->l_name, (void*)curr->l_addr, curr->l_ld);
    curr = curr->l_next;
  }
}
int main(int argc, char **argv)
{
	void *handle;
	void *handle2;
	void *handle3;
	int (*test)(int);
	char *error;

	handle = dlmopen(LM_ID_NEWLM, "dlmopen-simple.so", RTLD_LAZY);
	if (!handle)
	{
		fputs(dlerror(), stderr);
		exit(1);
	}
	handle2 = dlopen("dlmopen-simple2.so", RTLD_LAZY);
	if (!handle2)
	{
		fputs(dlerror(), stderr);
		exit(1);
	}
	handle3 = dlmopen(LM_ID_NEWLM, "dlmopen-simple3.so", RTLD_LAZY);
	if (!handle3)
	{
		fputs(dlerror(), stderr);
		exit(1);
	}
	test = dlsym(handle, "test_libdl");
	if ((error = dlerror()) != NULL)
	{
		fputs(error, stderr);
		exit(1);
	}
	dump_link_map(handle);
	dump_link_map(handle2);
	printf("%d\n", test(5));
	dlclose(handle);
	return 0;
}
