#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>


#define MAX_CHILDREN 4

int fd[2];

void *isPrimo(void* arg) {
	int num = *((int *)arg);
	free(arg);
	int *retval = malloc(sizeof(int)); 
	*retval = 1;

	pthread_t tid = pthread_self();

	if ((num != 2 && !(num%2)) || num<2) {
		*retval = 0;
	} else {
		long int i;
		for (i = 2; i <= num/2 && retval; i++) {
			if (!(num%i)) {
				*retval = 0;
			}
		}
	}

	write(fd[1], &tid, sizeof(tid));
	return (void *)retval;
}

int main() {
	// Estas variaveis sao utilizadas para a leitura da entrada
	char line[1024];
	char const *p;
	int x;
	int n;

	// Numero de threads rodando
	int n_children = 0;

	// Armazena o numero total de numeros primos encontrados na entrada
	int n_primos = 0;

	// Cria um pipe para comunicacao entre threads
	pipe(fd);
	
	// Variaveis para controle de threads
	pthread_t tid;
	void *result;
	
	// Le enetrada e atribui uma thread para tratar cada numero
	fgets(line, 1024, stdin);
	for(p = line; sscanf(p, "%d%n", &x, &n) == 1; p += n) {
		// Copia o valor para o heap
		int *num = malloc(sizeof(int));
		*num = x;
		
		// Passa a copia do valor para a nova thread
		pthread_create(&tid, NULL, isPrimo, (void *)num);
		n_children++;
		if (n_children == MAX_CHILDREN) {
			read(fd[0], &tid, sizeof(tid));
			pthread_join(tid, &result);
			n_primos += *((int *) result);
			free(result);
			n_children--;
		}
	}

	// Espera todas as threads terminarem
	while (n_children) {
		read(fd[0], &tid, sizeof(tid));
		pthread_join(tid, &result);
		n_primos += *((int *) result);
		free(result);
		n_children--;
	}

	printf("%d\n", n_primos);

	return 0;
}
