#include <string.h>
int main(void){
	const char foo[] ="foo";
	char *bar = strdup(foo);
	free(bar); bar = NULL;
}
