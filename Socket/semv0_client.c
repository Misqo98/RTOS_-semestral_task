#include <sys/socket.h>  
#include <netinet/in.h>  
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h> 
#include <arpa/inet.h> 
#include <unistd.h> 


int **create(int n)
{
	int **p, i;
	p = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		p[i] = (int *)malloc(n * sizeof(int));
	return p;
}

int **read1(char subor[50],int *num ) 
{
	FILE *fr;
	int **table;
	int sx = 0, sy = 0, min;
	//char subor[] = {"Vstup.txt"};
	if ((fr = fopen(subor, "r")) == NULL) {
		printf("Subor sa nepodarilo otvorit.\n");
		return 0;
	}

	fscanf(fr, "%d\n", &*(num));
	table = create(*(num));


	for (int x = 0; x < (*num); x++)

	{

		for (int y = 0; y < (*num); y++)

		{
			fscanf(fr, "%d", &table[x][y]);
		}

	}

	return table;

}

void Vypis(int **pole, int n) {
	for (int x = 0; x < n; x++)
	{
		for (int y = 0; y < n; y++)

		{
			printf("%d ", pole[x][y]);
		}
		printf("\n");
	}
	printf("\n");
}

main() 
{  
	char subor[] = {"Vstup.txt"};
	int **table;
	int n;
	table = read1(subor,&n);
	Vypis(table, n);
	

    // vytvorenie socketu
    int sock_desc = socket(AF_INET, SOCK_STREAM, 0); 
    if (sock_desc == -1)
    {
        printf("cannot create socket!\n");
        return 0;
    }


    // nastavenie socketu
    struct sockaddr_in client;  
    memset(&client, 0, sizeof(client));  
    client.sin_family = AF_INET;  
    client.sin_addr.s_addr = inet_addr("127.0.0.1");  
    client.sin_port = htons(7777);  

    // pripojenie socketu
    if (connect(sock_desc, (struct sockaddr*)&client, sizeof(client)) != 0)
    {
        printf("cannot connect to server!\n");
        close(sock_desc);
    }

    /*int Second_sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if (Second_sock_desc == -1)
    {
        printf("cannot create socket!\n");
        return 0;
    }

    // nastavenie socketu
    struct sockaddr_in server;  
    memset(&server, 0, sizeof(server));  
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;  
    server.sin_port = htons(17777);  
    if (bind(Second_sock_desc, (struct sockaddr*)&server, sizeof(server)) != 0)
    {
        printf("cannot bind socket!\n");
        close(Second_sock_desc);  
        return 0;
    }
    	
    if (listen(Second_sock_desc, 20) != 0)
    {
        printf("cannot listen on socket!\n");
        close(Second_sock_desc);  
        return 0;
    }

    struct sockaddr_in Second_client;  
    memset(&Second_client, 0, sizeof(Second_client));  
    socklen_t Second_len = sizeof(Second_client); 
    int temp_sock_desc = accept(Second_sock_desc, (struct sockaddr*)&Second_client, &Second_len);  
    if (temp_sock_desc == -1)
    {
        printf("cannot accept client!\n");
        close(Second_sock_desc);  
        return 0;
    }*/


    char buf[100];
    char c = '\n';
    char *p_buf;
    int k, len; 
    

    while(1) 
    {      
        gets(buf);

        len = strlen(buf);
        p_buf = buf;

        while (len > 0)
        {
            k = send(sock_desc, p_buf, len, 0);  // posielanie dat    
            if (k == -1)
            {
                printf("cannot write to server!\n");
                break;
            }

            p_buf += k;
            len -= k;
        }

        k = send(sock_desc, &c, 1, 0);      
        if (k == -1)
        {
            printf("cannot write to server!\n");
            break;
        }

        if (strcmp(buf, "exit") == 0)          
            break;  
	k = recv(sock_desc, buf, 100, 0);      
        if (recv == -1)
        {
            printf("\ncannot read from client!\n");
            break;
        }

        if (recv == 0)
        {
            printf("\nclient disconnected.\n");
            break;
        }

        if (k > 0)          
            printf("%*.*s", k, k, buf);  

        if (strcmp(buf, "exit") == 0)         
            break;     

    }
    

    close(sock_desc);  // zatvorenie socketu
    printf("client disconnected\n");

    return 0;  
} 
