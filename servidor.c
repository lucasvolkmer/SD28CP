#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define TAMBUFFER 1024
#define PORTA 1234

#define empty 	0
#define o 		1
#define x 		2	

int jogo[9] = {empty,empty,empty,empty,empty,empty,empty,empty,empty};
//int jogo[9] = {o,o,o,o,o,o,o,o,o};
int occup = 0;

char get_value(int pos){
	if(jogo[pos-1] == empty){
		return (pos+48); // Se estiver vazio, retorna a posição (o +48 é para 'converter' para char)
	}
	else if(jogo[pos-1] == o){
		//occup++; // Se estiver ocupado, incrementa o contador e retorna o valor
		return 'o'; // Se for o, retorna o
	}
	else if(jogo[pos-1] == x){
		//occup++; // Se estiver ocupado, incrementa o contador e retorna o valor
		return 'x'; // Se for x, retorna x
	}
}
char get_char(int n){
	return (n+48);
}
void print_game(){
	printf("\n  %c | %c | %c\n", get_value(1), get_value(2), get_value(3));
	printf(" --- --- ---\n");
	printf("  %c | %c | %c\n", get_value(4), get_value(5), get_value(6));
	printf(" --- --- ---\n");
	printf("  %c | %c | %c\n\n", get_value(7), get_value(8), get_value(9));
}
int check_win(){
	return 0;
}



int main(void){
	struct sockaddr_in addr_local;
	struct sockaddr_in addr_remoto;
	int socket_desc;
	socklen_t slen = sizeof(addr_remoto);
	char buf[TAMBUFFER];
	int tam_recebido;

	int pos, jogada_serv;


	// Passo 1 : Criando um socket UDP
	if ((socket_desc=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror("Nao foi possivel criar socket");
		return -1;
	}
	
	// Associando o socket a todos IPs locais e escolhendo um PORTA ESPECIFICA
	memset((char *) &addr_local, 0, sizeof(addr_local));
	addr_local.sin_family = AF_INET;
	addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_local.sin_port = htons(PORTA);

	if (bind(socket_desc, (struct sockaddr *)&addr_local, sizeof(addr_local)) < 0){
		perror("Erro ao fazer bind!");
		return -1;
	}
	
	// Passo 2: Realizar a comunicação com os clientes - fica em loop
	while(1){
		printf("Aguardando cliente ...\n");
		
		// processando pacote recebido
		if ( (tam_recebido = recvfrom(socket_desc, buf, TAMBUFFER, 0, (struct sockaddr *)&addr_remoto, &slen)) > 0){
			buf[tam_recebido]='\0';
			if(buf[0] == 'C'){
				printf("\nFim de jogo, cliente Ganhou!\n");
				close(socket_desc);
				return 0;
			}
			if(buf[0] == 'S'){
				printf("\nFim de jogo, servidor Ganhou!\n");
				close(socket_desc);
				return 0;
			}
				//inet_ntoa(addr_remoto.sin_addr), ntohs(addr_remoto.sin_port), buf);
			printf("Posição escolhida pelo cliente: %s\n\n", buf);
			
			pos = ((int)buf[0]-48); // Pega a posição recebida e faz o cast do valor asci pra inteiro
			jogo[pos-1] = o; // Altera para bolinha na posição escolhida pelo cliente
			
			// Imprimir a mensagem das posições
			print_game();
			// O servidor joga na primeira posição disponível
			for(int i=0; i<9; i++){
				if(jogo[i] == empty){
					jogo[i] = x;
					printf("Posição jogada pelo servidor: %d\n", i+1);
					// Coloca no buffer a posição que o servidor jogou
					sprintf(buf, "%d", i); // Coloca o i no buffer
					//printf("\n%s\n", buf);
					// Respondendo ao addr_remoto
					if ((sendto(socket_desc, buf, strlen(buf), 0, (struct sockaddr *)&addr_remoto, slen)) <0){
						perror("Erro ao enviar resposta!!!");
					}

					i=9;
				}
			}
				
			
		}
		//printf("pos: %d\n", pos);
		//printf("valor: %d", jogo[pos-1]);
		//rintf("valor: %d", jogo[pos-1]);

		/*or(int i=0; i<9; i++){
		printf("%d\n", jogo[i]);
		}*/

		

		
		// Imprimir a mensagem das posições
		print_game();
	}
	//Passo 3: Fechar socket
	close(socket_desc);
	return 0;
}
