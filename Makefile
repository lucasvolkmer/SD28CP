all:: udp

udp:	cliente_udp.c servidor_udp.c 
	gcc -o cliente_udp cliente_udp.c
	gcc -o servidor_udp servidor_udp.c

clean:
	rm cliente_udp servidor_udp 
