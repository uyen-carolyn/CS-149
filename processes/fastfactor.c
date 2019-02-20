#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
void factor_this(long long n) {
    int nn = n/2;
    for (int i = 0; i < 2; i++) {
        
    int pid = fork(); 
//    int pid1 = fork(); 
    if(pid < 0) {
        perror("Fork failed");
        exit(1); 
    }
    else if (pid == 0) {
        printf("%lld: ", n);
        for (long long i = 1; i <= nn; i++) {
            if(n%i == 0) {  
                printf("%lld ", i); 
            }
        }
        printf("%lld\n", n); break;
//        printf("This is a child\n"); 
//        exit(0); 
    }
    else {
 //       printf("This is a parent\n");
 //       exit(0); 
        wait(NULL); 
 //       printf("This is the end\n");
   
        return; 
    }
    nn = 2 * nn + 1; 
    }
//    printf("%lld: ", n); 
//	for (long long i = 1; i <= n/2; i++) {
//		if(n%i == 0) {
//			printf("%lld ", i); 
//		}
//	}
//	printf("%lld\n", n); 
}

int main(int argc, char **argv) {
	long long n, new_n;
    char *end, *convert; 
//	printf("Enter a positive integer ");
	if (argc > 1) {
        for (int a = 1; a < argc; a++) {
            convert = argv[a];
			new_n = strtoll(convert, &end, 10); 
//            printf("%s\n", argv[a]);
            factor_this(new_n); 
//            printf("%lld\n", new_n); 
        }
//		 printf("coming soon\n");
	}
	else { 
		while (scanf("%lld", &n) > 0) {
			factor_this(n); 
//		printf("%d\n", n);
		}
	 }
	return 0; 
}
