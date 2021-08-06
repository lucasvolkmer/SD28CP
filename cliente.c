#include <stdio.h>
#include <stdlib.h>
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

void print_game(){
	printf("\n  %c | %c | %c\n", get_value(1), get_value(2), get_value(3));
	printf(" --- --- ---\n");
	printf("  %c | %c | %c\n", get_value(4), get_value(5), get_value(6));
	printf(" --- --- ---\n");
	printf("  %c | %c | %c\n\n", get_value(7), get_value(8), get_value(9));
}

int check_win(){
	// Primeira Horizontal
	if(jogo[0] == jogo[1] && jogo[1] == jogo[2] && jogo[0] != empty){
		if(jogo[0] == o){
			return 1;
		}
		else if(jogo[0] == x){
			return 2;
		}
	}
	// Segunda Horizontal
	if(jogo[3] == jogo[4] && jogo[4] == jogo[5] && jogo[4] != empty){
		if(jogo[3] == o){
			return 1;
		}
		else if(jogo[3] == x){
			return 2;
		}
	}
	// Terceira Horizontal
	if(jogo[6] == jogo[7] && jogo[7] == jogo[8] && jogo[6] != empty){
		if(jogo[6] == o){
			return 1;
		}
		else if(jogo[6] == x){
			return 2;
		}
	}
	// Primeira Vertical
	if(jogo[0] == jogo[3] && jogo[3] == jogo[6] && jogo[0] != empty){
		if(jogo[0] == o){
			return 1;
		}
		else if(jogo[0] == x){
			return 2;
		}
	}
	// Segunda Vertical
	if(jogo[1] == jogo[4] && jogo[4] == jogo[7] && jogo[1] != empty){
		if(jogo[1] == o){
			return 1;
		}
		else if(jogo[1] == x){
			return 2;
		}
	}
	// Terceira Vertical
	if(jogo[2] == jogo[5] && jogo[5] == jogo[8] && jogo[2] != empty){
		if(jogo[2] == o){
			return 1;
		}
		else if(jogo[2] == x){
			return 2;
		}
	}
	// Diagonal Principal
	if(jogo[0] == jogo[4] && jogo[4] == jogo[8] && jogo[0] != empty){
		if(jogo[0] == o){
			return 1;
		}
		else if(jogo[0] == x){
			return 2;
		}
	}
	// Diagonal Secundária
	if(jogo[2] == jogo[4] && jogo[4] == jogo[6] && jogo[2] != empty){
		if(jogo[2] == o){
			return 1;
		}
		else if(jogo[2] == x){
			return 2;
		}
	}
	return 0;
}



int main(void){
	struct sockaddr_in addr_local;
	struct sockaddr_in addr_remoto;
	int socket_desc;
	socklen_t slen = sizeof(addr_remoto);
	char buf[TAMBUFFER];
	int tam_recebido;	
	char *servidor = "127.0.0.1";
	
	char p[2];
	int pos_cli, pos_serv;

	// Passo 1 : Criando um socket UDP
	if ((socket_desc=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror("nao foi possivel criar socket");
		return -1;
	}

	// Associando o socket a todos IPs locais e escolhendo um PORTA QUALQUER
	memset((char *) &addr_local, 0, sizeof(addr_local));
	addr_local.sin_family = AF_INET;
	addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_local.sin_port = htons(0);

	if (bind(socket_desc, (struct sockaddr *)&addr_local, sizeof(addr_local)) < 0) {
		perror("Erro ao fazer bind!");
		return -1;
	}       

	// Definindo addr_remoto como o endereco de destino
	// Convertendo a string 127.0.0.1 para formato binario com inet_aton
	memset((char *) &addr_remoto, 0, sizeof(addr_remoto));
	addr_remoto.sin_family = AF_INET;
	addr_remoto.sin_port = htons(PORTA);
	if (inet_aton(servidor, &addr_remoto.sin_addr)==0) {
		fprintf(stderr, "inet_aton() falhou\n");
		return -1;
	}

	print_game();	
	while(1){
		do{
			printf("Informe em qual posição deseja jogar:");
			scanf(" %s", p);
			pos_cli = ((int)p[0]-48);
			if(jogo[pos_cli-1] != empty){
				printf("Posição ocupada!\n");
			}
		}while(jogo[pos_cli-1] != empty);
			

		// Passo 2: Realizar a comunicação com o servidor
		// Enviando mensagem para o servidor
		jogo[pos_cli-1] = o;
		print_game();	

		// Verifica se o jogo acabou
		if(check_win() == 1){
			printf("\nCliente Ganhou!\n");
			// Envia mensagem ao servidor avisando que o cliente ganhou
			sprintf(buf, "C");
			if (sendto(socket_desc, buf, strlen(buf), 0, (struct sockaddr *)&addr_remoto, slen)==-1) {
				perror("Erro ao enviar pacote!!!");
				return -1;
			}
			close(socket_desc);
			return 0;
		}
		else if(check_win()==2){
			printf("\nServidor Ganhou!\n");
			// Envia mensagem ao servidor avisando que o servidor ganhou
			sprintf(buf, "S");
			if (sendto(socket_desc, buf, strlen(buf), 0, (struct sockaddr *)&addr_remoto, slen)==-1) {
				perror("Erro ao enviar pacote!!!");
				return -1;
			}
			close(socket_desc);
			return 0;
		}

		//printf("Enviando mensagem para o servidor: %s [%d]\n", servidor, PORTA);
		sprintf(buf, p);
		if (sendto(socket_desc, buf, strlen(buf), 0, (struct sockaddr *)&addr_remoto, slen)==-1) {
			perror("Erro ao enviar pacote!!!");
			return -1;
		}
		// Recebendo resposta do servidor
		if ((tam_recebido = recvfrom(socket_desc, buf, TAMBUFFER, 0, (struct sockaddr *)&addr_remoto, &slen)) >=0 ){
			buf[tam_recebido] = '\0';
			pos_serv = buf[0] + 1 - 48;
			printf("\n--------------------------------\n");
			printf("Posição jogada pelo servidor: %d \n", pos_serv);
			jogo[pos_serv-1] = x;
		}
		print_game();

		// Verifica se o jogo acabou
		if(check_win() == 1){
			printf("\nCliente Ganhou!\n");
			close(socket_desc);
			return 0;
		}
		else if(check_win()==2){
			printf("\nServidor Ganhou!\n");
			close(socket_desc);
			return 0;
		}

	}
	

	//Passo 3: Fechar socket
	
}
