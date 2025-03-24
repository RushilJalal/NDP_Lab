#include<time.h>
#include <stdio.h>

int main(){
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	printf("%s", asctime(timeinfo));
}
