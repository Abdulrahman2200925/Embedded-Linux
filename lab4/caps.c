#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>




int main(int argc, char *argv[]){



if((argc !=2) || ((strcmp(argv[1], "1") != 0) && (strcmp(argv[1], "0") != 0) )){
    printf("invalid arguments\n");
}
  
else {
const char *path="/sys/class/leds/input6::capslock/brightness";
/*open the desired file to get the fd number*/
int fd= open(path, O_RDWR);

if (strcmp(argv[1], "1") == 0){
write( fd,"1", 1);
}
else if (strcmp(argv[1], "0") == 0) {
write( fd,"0", 1);
}
 close( fd);

}
return 0;
}