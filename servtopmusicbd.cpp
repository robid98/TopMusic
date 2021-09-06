#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <sqlite3.h>

//VARIABILE GLOBALE PENTRU A SALVA CE ESTE IN BAZA DE DATE
char name[10000][300];
int cname;

struct Infouser { // STRUCTURA PENTRU A TINE INFORMATIILE DESPRE USERI - INFO[0] : primul user etc..
	char num[300];
	char pass[30];
	char usert[30];
	char uservote[5];
};

struct Infosong {
	char id[100];
	char songname[50];
	char songdescription[100];
	char songtype[100];
	char songcomments[10000];
	char songlink[150];
	char songvote[100];
};
Infouser INFO[10000]; // STRUCTURA PENTRU USERI

Infosong INFO1[10000];//STRUCTURA PENTRU MELODII

int sizeINFO ; // CATI USERI AVEM IN DB
int sizeMUSIC ; // CATE MELODII AVEM IN BAZA DE DATE
char linenumber[5] ; // CATE LINII AVEM IN BAZA DE DATE - MUSIC


//FUNCTII CALLBACK SQLITE3

//FUNCTIE PENTRU A PUNE IN STRUCT INFORMATIILE PENTRU USERI
static int callback(void *data, int argc, char **argv, char **azColName) {
	int i;

	for (i = 0; i < argc; i++) {
		if (i == 0)
			strcpy(INFO[sizeINFO].num, argv[i]);
		if (i == 1)
			strcpy(INFO[sizeINFO].pass, argv[i]);
		if (i == 2)
			strcpy(INFO[sizeINFO].usert, argv[i]);
		if (i == 3) {
			strcpy(INFO[sizeINFO].uservote , argv[i]);
			sizeINFO++;
		}
	}
	return 0;
}

//FUNCTIE PENTRU A PUNE IN STRUCT INFORMATIILE PENTRU MELODII
static int callbacksong(void *data, int argc, char **argv, char **azColName) {
	int i;

	for (i = 0; i < argc; i++) {
		if (i == 0)
			strcpy(INFO1[sizeMUSIC].id, argv[i]);
		if (i == 1)
			strcpy(INFO1[sizeMUSIC].songname, argv[i]);
		if (i == 2)
			strcpy(INFO1[sizeMUSIC].songdescription, argv[i]);
		if (i == 3)
			strcpy(INFO1[sizeMUSIC].songtype , argv[i]);
		if (i == 4)
			strcpy(INFO1[sizeMUSIC].songcomments, argv[i]);
		if (i == 5)
			strcpy(INFO1[sizeMUSIC].songlink, argv[i]);
		if (i == 6) {
			strcpy(INFO1[sizeMUSIC].songvote, argv[i]);
			sizeMUSIC++;
		}
	}
	return 0;
}

//FUNCTIE PENTRU A PUNE IN LINENUMBER CATE LINII AVEM IN BAZA DE DATE
static int callbacknumber(void *data, int argc, char **argv, char **azColName) {
	int i;
	for (int i = 0 ; i < argc ; i++)
	{
		if (i == 0)
			strcpy(linenumber, argv[i]);
	}
}

//FUNCTIE PENTRU A TRANSFORMA UN CHAR * IN INT
int toint(char *number) // CHAR IN INT , FUNCTIE
{
	int sol = 0 ;
	for (int i = 0 ; i < strlen(number) ; i++)
		sol = sol * 10 + (number[i] - '0');
	return sol;
}


void criptare(char userpassword[])
{
	char xorKey = 'P';
	int lungime = strlen(userpassword);
	for (int i = 0; i < lungime; i++)
		userpassword[i] = userpassword[i] ^ xorKey;
}
/* portul folosit */
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main ()
{
	//PENTRU BAZA DE DATE SQLITE3
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	const char *sql;
	const char *data ;
	//Deschidem baza de date 'user.db'
	rc = sqlite3_open("user.db", &db);
	if ( rc ) {
		std::cout << "Nu s-a putut deschide baza de date" << std::endl;
		return (0);
	}
	else
		std::cout << "Baza de date a  fost deschisa cu succes" << std::endl;


	struct sockaddr_in server;	// structura folosita de server
	struct sockaddr_in from;

	int sd;		       //descriptorul de socket
	char comanda[100]; //comanda primita de la client

	/* crearea unui socket */
	if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "[server]Eroare la socket().";
		return errno;
	}
	/* pregatirea structurilor de date */
	bzero (&server, sizeof (server));
	bzero (&from, sizeof (from));
	/* umplem structura folosita de server */
	/* stabilirea familiei de socket-uri */
	server.sin_family = AF_INET;
	/* acceptam orice adresa */
	server.sin_addr.s_addr = htonl (INADDR_ANY);
	/* utilizam un port utilizator */
	server.sin_port = htons (PORT);
	/* atasam socketul */
	if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
	{
		std::cerr << "[server]Eroare la bind()." << std::endl;
		return errno;
	}
	/* punem serverul sa asculte daca vin clienti sa se conecteze */
	if (listen (sd, 1000) == -1)
	{
		std::cerr << "[server]Eroare la listen()." << std::endl;
		return errno;
	}
	while (1)
	{
		int client;
		socklen_t length = sizeof (from);
		std::cout << "\033[1;31m[server]\033[0m " ;
		std::cout << "Asteptam la portul..." << PORT << std::endl;

		/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
		client = accept (sd, (struct sockaddr *) &from, &length);
		std::cout << "\033[1;31m!\033[0m " ;
		std::cout << "Cineva s-a conectat la aplicatie" << std::endl;
		/* eroare la acceptarea conexiunii de la un client */
		if (client < 0)
		{
			std::cout << "[server]Eroare la accept()." << std::endl;
			continue;
		}
		int pid;
		if ((pid = fork()) == -1)
		{
			close(client);
			continue;
		}
		else if (pid > 0) // In parinte
		{
			close(client);
			while (waitpid(-1, NULL, WNOHANG));
			continue;
		}
		else if (pid == 0 ) // Copil
		{
			close(sd);
			bzero (comanda, 100);

			// PRIMIM COMANDA (LOGIN) SAU (SINGUP) CA SA O VERIFICAM
			if (recv(client, comanda, sizeof(comanda), MSG_WAITALL) <= 0) // PRIMIM COMANDA LOGIN SAU SINGUP
			{
				std::cerr << "[server]Eroare la recv() de la client";
				close (client);	/* inchidem conexiunea cu clientul */
				continue;		/* continuam sa ascultam */
			}
			int OKL = 0 , OKS = 0 , count = 0 ; // OKL - LOGIN OKS - SINGUP COUNT - PENTRU STRTOK
			int OKDB = 0 ; //DACA NUMELE SE AFLA DEJA IN BAZA DE DATE
			int OK1 = 0 , savepos = 0; // DACA REUSESTE SA SE LOGHEZE CU SUCCES. ; savepos = salvam pozitia
			int OK2 = 0 ; // DACA ESTI ADMINISTRATOR SAU USER OBISNUIT OK2 = 1 / ADM OK2 = 0 / OBS
			int OK3 = 0 ; // DACA ARE DREPT DE VOT SAU NU.
			int OKadds = 0, OKshow = 0, OKaddc = 0 , OKshowcom = 0 ; // PENTRU COMENZILE CARE POT FI FOLOSITE DE TOTI USERII
			int OKvotesong = 0;
			int OKdelete = 0;
			int OKrestriction = 0;
			int OKsetusertype = 0;
			int OKquit = 0 ; // SE IESE DIN BAZA DE DATE
			int OKMUSIC = 0; // DACA NUMELE MELODIEI EXISTA IN BAZA DE DATE
			char *p , LS[10] , USER[30] , PASSWORD[40]; // LS - LOGIN / SINGUP USER - NUME PASSWORD - PAROLA
			memset(USER, 0, sizeof(USER));
			memset(PASSWORD, 0, sizeof(PASSWORD));
			p = strtok(comanda, " ");
			while (p)
			{
				count ++ ;
				if (count == 1)
					strcpy(LS, p);
				if (count == 2)
					strcpy(USER, p);
				if (count == 3)
					strcpy(PASSWORD, p);
				p = strtok(NULL, " ");
			}
			if (strcmp(LS, "login") == 0) {
				OKL = 1;

			}
			if (strcmp(LS, "singup") == 0) {
				OKS = 1;
				if (strcmp(USER, "") == 0)
					OKS = 0;
				if (strcmp(PASSWORD, "") == 0)
					OKS = 0;
			}
			criptare(PASSWORD); // CRIPTAM PAROLA.
			if (send(client, &OKS, sizeof(OKS), 0) <= 0)
				// TRIMITEM LA CLIENT DACA COMANDA LOGIN ESTE SCRISA BINE
			{
				std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
				continue;
			}
			if (send(client, &OKL, sizeof(OKL), 0) <= 0)
				// TRIMITEM LA CLIENT DACA COMANDA SINGUP ESTE SCRISA BINE
			{
				std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
				continue;
			}
			if (OKS == 1) // DACA CLIENTUL A TASTAT SINGUP
			{
				sql = "SELECT * from USERNAME";
				rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
				for (int i = 0 ; i < sizeINFO ; i++)
				{
					if (strcmp(USER, INFO[i].num) == 0)
						OKDB = 1;
				}
				if (OKDB == 0) // INSEAMNA CA USERNAMENUL NU A FOST GASIT IN DB - SE POATE INREGISTRA
				{
					sql = sqlite3_mprintf("insert into USERNAME values ('%s','%s','%s','%d');", USER, PASSWORD, "Obisnuit", 1);
					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
				}
			}
			if (OKS == 0)
				OKDB = 1 ;
			if (send(client, &OKDB, sizeof(OKDB), 0) <= 0)
				// TRIMITEM LA CLIENT DACA A REUSIT SA SE INREGISTREZE SAU NU
			{
				std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
				continue;
			}
			if (OKL == 1) // DACA CLIENTUL A TASTAT LOGIN
			{
				sql = "SELECT * from USERNAME";
				rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
				for (int i = 0 ; i < sizeINFO ; i++)
				{
					if (strcmp(INFO[i].num, USER) == 0)
					{
						savepos = i;
						OK1 = 1;
						break;
					}

				}
				if (OK1 == 1) // DACA NUMELE A FOST GASIT IN BAZA DE DATE
				{
					if (strcmp(INFO[savepos].pass, PASSWORD) == 0)
						OK1 = 1; // DACA PAROLA ESTE CORECTA
					else OK1 = 0; // => PAROLA NU ESTE CORECTA

					if (OK1 == 1) // DACA A REUSIT SA SE LOGHEZE CU SUCCES / NUME - PAROLA
					{
						if (strcmp(INFO[savepos].usert, "Administrator") == 0)
							OK2 = 1; // Este administrator
						else
							OK2 = 0; // Este user obisnuit
						if (strcmp(INFO[savepos].uservote, "1") == 0)
							OK3 = 1; // Are drept de vot
						else
							OK3 = 0; // Nu are drept de vot
					}
				}
			}
			if (send(client, &OK1, sizeof(OK1), 0) <= 0)
				// TRIMITEM LA CLIENT DACA LOGAREA A FOST CU SUCCES - NUME PAROLA VERIFICATE
			{
				std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
				continue;
			}
			if (send(client, &OK2, sizeof(OK2), 0) <= 0)
				// TRIMITEM LA CLIENT DACA ESTE ADMINISTRATOR SAU USER OBISNUIT
			{
				std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
				continue;
			}
			if (send(client, &OK3, sizeof(OK3), 0) <= 0)
				// TRIMITEM LA CLIENT DACA ARE DREPT DE VOT SAU NU
			{
				std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
				continue;
			}
			while (1)
			{
				//INCEPEM PROCESAREA COMENZILOR PENTRU TOP-UL MUSICAL
				sizeMUSIC = 0;
				memset(linenumber,0,sizeof(linenumber));
				if (OKS == 1 || OKL == 1)
				{
					if (OKDB == 0 || (OK1 == 1 && OK2 == 1) || (OK1 == 1 && OK2 == 0))
					{
						// PRIMIM CONFIRMAREA CA CLIENTUL A SCRIS //ADDSONG SI PROCESAM
						if (recv(client, &OKadds , sizeof(OKadds), MSG_WAITALL) <= 0)
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKadds == 1) // DACA UTILIZATORUL A TASTAT ADDSONG
						{
							char sname[50], sdescription[100], stype[100], slink[150];
							memset(sname, 0, sizeof(sname));
							memset(sdescription, 0, sizeof(sname));
							memset(stype, 0, sizeof(sname));
							memset(slink, 0, sizeof(sname));
							if (recv(client, &sname , sizeof(sname), MSG_WAITALL) <= 0) // PRIMIM NUME MELODIE
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (recv(client, &sdescription , sizeof(sdescription), MSG_WAITALL) <= 0) // PRIMIM DESCRIERE
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (recv(client, &stype , sizeof(stype), MSG_WAITALL) <= 0) // PRIMIM GEN
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (recv(client, &slink, sizeof(slink), MSG_WAITALL) <= 0) // PRIMIM LINK
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							sql = "SELECT COUNT(*) FROM MUSIC";
							rc = sqlite3_exec(db, sql, callbacknumber, (void*)data, &zErrMsg);
							int songid = toint(linenumber) + 1;
							char songidtoadd[100]; // VARIABILA PENTRU A ADAUGA ID - MELODIEI IN BAZA DE DATE
							char comm;
							sprintf(songidtoadd, "%d", songid);
							sql = "SELECT * FROM MUSIC";
							rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
							for (int i = 0 ; i < sizeMUSIC ; i++)
							{
								if (strcmp(INFO1[i].songname, sname) == 0)
								{
									OKMUSIC = 1;
									break;
								}
							}
							if (strcmp(sname, "") == 0 || strcmp(sdescription, "") == 0 || strcmp(stype, "") == 0 || strcmp(slink, "") == 0)
								OKMUSIC = 1;

							if (send(client, &OKMUSIC, sizeof(OKMUSIC), 0) <= 0)
								// TRIMITEM LA CLIENT DACA S-A PUTUT ADAUGA MELODIA
							{
								std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
								continue;
							}
							if (OKMUSIC == 0)
							{
								sql = sqlite3_mprintf("insert into MUSIC values ('%s','%s','%s','%s','%s','%s','%s');", songidtoadd, sname, sdescription, stype, comm, slink, "0");
								rc = sqlite3_exec(db, sql, callbacksong, 0, &zErrMsg);
							}
						}
						if (recv(client, &OKshow , sizeof(OKshow), MSG_WAITALL) <= 0)
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}


						if (OKshow == 1) // DACA UTILIZATORUL A TASTAT COMANDA Show
						{
							int OKALL = 0;
							int OKGenuri = 0;
							if (recv(client, &OKALL, sizeof(OKALL), MSG_WAITALL) <= 0)
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (recv(client, &OKGenuri , sizeof(OKGenuri), MSG_WAITALL) <= 0)
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKGenuri == 1)
							{
								char introducetigen[30];
								if (recv(client, &introducetigen , sizeof(introducetigen), MSG_WAITALL) <= 0)
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = sqlite3_mprintf("SELECT * FROM MUSIC WHERE GEN LIKE '%s' ORDER BY NRVOT DESC", introducetigen);
								rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								if (send(client, &sizeMUSIC, sizeof(sizeMUSIC), 0) <= 0)
									// TRIMITEM LA CLIENT CATE MELODII SUNT IN BAZA DE DATE
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (send(client, &INFO1, sizeof(INFO1), 0) <= 0)
									// TRIMITEM LA CLIENT STRUCTURA CU MELODIILE
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
							}
							if (OKALL == 1)
							{
								sql = "SELECT COUNT(*) FROM MUSIC";
								
								rc = sqlite3_exec(db, sql, callbacknumber, (void*)data, &zErrMsg);
								int songnumber = toint(linenumber);
								if (send(client, &songnumber, sizeof(songnumber), 0) <= 0)
									// TRIMITEM LA CLIENT CATE MELODII SUNT IN BAZA DE DATE
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								sql = "SELECT * FROM MUSIC ORDER BY NRVOT DESC";
								rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								if (send(client, &INFO1, sizeof(INFO1), 0) <= 0)
									// TRIMITEM LA CLIENT STRUCTURA CU MELODIILE
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
							}
						}

						if (recv(client, &OKaddc , sizeof(OKaddc), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT addc - SA ADAUGE COMENTARIU
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKaddc == 1)
						{
							int OKcomvalid = 0;
							if (recv(client, &OKcomvalid , sizeof(OKcomvalid), MSG_WAITALL) <= 0)
								// DACA UTILIZATORUL A TASTAT addc - SA ADAUGE COMENTARIU
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKcomvalid == 1)
								// INSEAMNA CA IL PUTEM ADAUGA IN BAZA DE DATE
							{
								char introduceticomentariu[500];
								char numelemelodiei[100];
								int OKEXIST = 0 ;
								int position = 0 ;
								if (recv(client, &numelemelodiei , sizeof(numelemelodiei), MSG_WAITALL) <= 0)
									// PRIMIM NUMELE MELODIEI
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								if (recv(client, &introduceticomentariu , sizeof(introduceticomentariu), MSG_WAITALL) <= 0)
									// PRIMIM COMENTARIUL
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = "SELECT * FROM MUSIC";
								rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								for (int i = 0 ; i < sizeMUSIC ; i++)
								{
									if (strcmp(INFO1[i].songname, numelemelodiei) == 0)
									{
										position = i ;
										OKEXIST = 1;
										break;
									}
								}
								if (send(client, &OKEXIST, sizeof(OKEXIST), 0) <= 0)
									// TRIMITEM LA CLIENT DACA NUMELE MELODIEI A FOST GASIT IN DB
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (OKEXIST == 1)
								{
									strcat(INFO1[position].songcomments, "\n");
									strcat(INFO1[position].songcomments, introduceticomentariu);
									sql = sqlite3_mprintf("UPDATE MUSIC SET COMENTARII = '%s' WHERE SNAME LIKE '%s'", INFO1[position].songcomments, numelemelodiei);
									rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								}
							}
						}
						if (recv(client, &OKshowcom , sizeof(OKshowcom), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT showcomments
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKshowcom == 1) // DACA SUNTEM IN COMANDA SHOWCOMMENTS
						{
							int OKnumvalid = 0;
							if (recv(client, &OKnumvalid , sizeof(OKnumvalid), MSG_WAITALL) <= 0)
								// DACA UTILIZATORUL A TASTAT UN NUME VALID
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKnumvalid == 1)
							{
								char numelemelodiei[100];
								int OKEXIST = 0 ;
								int position = 0;
								if (recv(client, &numelemelodiei , sizeof(numelemelodiei), MSG_WAITALL) <= 0)
									// PRIMIM NUMELE MELODIEI
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = "SELECT * FROM MUSIC";
								rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								for (int i = 0 ; i < sizeMUSIC ; i++)
								{
									if (strcmp(INFO1[i].songname, numelemelodiei) == 0)
									{
										position = i ;
										OKEXIST = 1;
										break;
									}
								}
								if (send(client, &OKEXIST, sizeof(OKEXIST), 0) <= 0)
									// TRIMITEM LA CLIENT DACA NUMELE MELODIEI A FOST GASIT IN DB
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (OKEXIST == 1)
								{
									if (send(client, &INFO1[position].songcomments, sizeof(INFO1[position].songcomments), 0) <= 0)
										// TRIMITEM LA CLIENT DACA NUMELE MELODIEI A FOST GASIT IN DB
									{
										std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
										continue;
									}
								}
							}
						}

						//PENTRU COMANDA VOTESONG - DOAR CEI CARE AU DREPT SA FACA ASTA
						if (recv(client, &OKvotesong , sizeof(OKvotesong), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT votesong
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKvotesong == 1)
						{
							int OKnumvalid = 0;
							if (recv(client, &OKnumvalid , sizeof(OKnumvalid), MSG_WAITALL) <= 0)
								// DACA UTILIZATORUL A TASTAT UN NUME VALID
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKnumvalid == 1) {
								char numelemelodiei[100];
								int OKEXIST = 0 ;
								int position = 0;
								if (recv(client, &numelemelodiei , sizeof(numelemelodiei), MSG_WAITALL) <= 0)
									// PRIMIM NUMELE MELODIEI PE CARE TREBUIE SA O VOTAM
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = "SELECT * FROM MUSIC";
								rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								for (int i = 0 ; i < sizeMUSIC ; i++)
								{
									if (strcmp(INFO1[i].songname, numelemelodiei) == 0)
									{
										position = i ;
										OKEXIST = 1;
										break;
									}
								}
								if (send(client, &OKEXIST, sizeof(OKEXIST), 0) <= 0)
									// TRIMITEM LA CLIENT DACA NUMELE MELODIEI A FOST GASIT IN DB
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (OKEXIST == 1)
								{
									int votare = toint(INFO1[position].songvote);
									votare ++;
									char addnewvote[1000];
									sprintf(addnewvote, "%d", votare);
									sql = sqlite3_mprintf("UPDATE MUSIC SET NRVOT = '%s' WHERE SNAME LIKE '%s'", addnewvote, numelemelodiei);
									rc = sqlite3_exec(db, sql, NULL, (void*)data, &zErrMsg);
								}
							}
						}
						//PENTRU COMANDA DELETESONG - DOAR CEI CARE AU DREPT SA FACA ASTA
						if (recv(client, &OKdelete , sizeof(OKdelete), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT deletesong
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKdelete == 1)
						{
							int OKnumvalid = 0;
							if (recv(client, &OKnumvalid , sizeof(OKnumvalid), MSG_WAITALL) <= 0)
								// DACA UTILIZATORUL A TASTAT UN NUME VALID
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKnumvalid == 1)
							{
								char numelemelodiei[100];
								int OKEXIST = 0 ;
								int position = 0;
								if (recv(client, &numelemelodiei , sizeof(numelemelodiei), MSG_WAITALL) <= 0)
									// PRIMIM NUMELE MELODIEI PE CARE TREBUIE SA O VOTAM
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = "SELECT * FROM MUSIC";
								rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
								for (int i = 0 ; i < sizeMUSIC ; i++)
								{
									if (strcmp(INFO1[i].songname, numelemelodiei) == 0)
									{
										position = i ;
										OKEXIST = 1;
										break;
									}
								}
								if (send(client, &OKEXIST, sizeof(OKEXIST), 0) <= 0)
									// TRIMITEM LA CLIENT DACA NUMELE MELODIEI A FOST GASIT IN DB
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (OKEXIST == 1)
								{
									sql = sqlite3_mprintf("DELETE FROM MUSIC WHERE SNAME LIKE '%s'", numelemelodiei);
									rc = sqlite3_exec(db, sql, callbacksong, (void*)data, &zErrMsg);
									for (int i = position + 1 ; i < sizeMUSIC ; i++)
										//CELE CARE AVEAU ID MAI MARE CA MELODIA STEARSA ACUM O SA AIBA ID-1
										//DEOARECE , ID-URILE TREBUIE SA FIE TOT TIMPUL IN ORDINE.
									{
										int intid = toint(INFO1[i].id);
										intid = intid - 1;
										char charid[100];
										sprintf(charid, "%d", intid);
										sql = sqlite3_mprintf("UPDATE MUSIC SET ID = '%s' WHERE SNAME LIKE '%s'", charid, INFO1[i].songname);
										rc = sqlite3_exec(db, sql, NULL, (void*)data, &zErrMsg);
									}
								}
							}
						}
						//PENTRU COMANDA RESTRICTIONVOTE
						if (recv(client, &OKrestriction , sizeof(OKrestriction), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT restrictionvote
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKrestriction == 1)
						{
							int OKusernamevalid = 0;
							if (recv(client, &OKusernamevalid , sizeof(OKusernamevalid), MSG_WAITALL) <= 0)
								// DACA UTILIZATORUL A TASTAT UN NUME VALID
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKusernamevalid == 1)
							{
								char numeleutilizatorului[300];
								int OKEXIST = 0 ;
								int position = 0;
								if (recv(client, &numeleutilizatorului , sizeof(numeleutilizatorului), MSG_WAITALL) <= 0)
									//  PRIMIM NUMELE UTILIZATORULUI
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = "SELECT * from USERNAME";
								rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
								for (int i = 0 ; i < sizeINFO ; i++)
								{
									if (strcmp(INFO[i].num, numeleutilizatorului) == 0)
									{
										position = i ;
										OKEXIST = 1;
										break;
									}
								}
								if (send(client, &OKEXIST, sizeof(OKEXIST), 0) <= 0)
									// TRIMITEM LA CLIENT DACA NUMELE UTILIZATORULUI A FOST GASIT IN DB
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (OKEXIST == 1)
								{
									int ISOKTYPE = 0;
									//DACA ESTE ADMINISTRATOR NU PUTEM SA II RESTRICTIONAM DREPTUL DE A VOTA
									if (strcmp(INFO[position].usert, "Administrator") == 0)
										ISOKTYPE = 1 ;
									else
										ISOKTYPE = 0;
									if (send(client, &ISOKTYPE, sizeof(ISOKTYPE), 0) <= 0)
										// TRIMITEM LA CLIENT DACA USERUL ESTE ADMINISTRATOR SAU NU
									{
										std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
										continue;
									}
									if (ISOKTYPE == 0)
									{
										sql = sqlite3_mprintf("UPDATE USERNAME SET VOTE = '%s' WHERE NAME LIKE '%s'", "0", numeleutilizatorului);
										rc = sqlite3_exec(db, sql, NULL, (void*)data, &zErrMsg);
									}
								}

							}
						}

						//PENTRU COMANDA SETUSERTYPE
						if (recv(client, &OKsetusertype , sizeof(OKsetusertype), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT setusertype
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if (OKsetusertype == 1)
						{	int OKusernamevalid = 0;
							if (recv(client, &OKusernamevalid , sizeof(OKusernamevalid), MSG_WAITALL) <= 0)
								// DACA UTILIZATORUL A TASTAT UN NUME VALID
							{
								std::cerr << "[server]Eroare la recv() de la client";
								close (client);	/* inchidem conexiunea cu clientul */
								continue;		/* continuam sa ascultam */
							}
							if (OKusernamevalid == 1)
							{
								char numeleutilizatorului[300];
								int OKEXIST = 0 ;
								int position = 0;
								if (recv(client, &numeleutilizatorului , sizeof(numeleutilizatorului), MSG_WAITALL) <= 0)
									//  PRIMIM NUMELE UTILIZATORULUI
								{
									std::cerr << "[server]Eroare la recv() de la client";
									close (client);	/* inchidem conexiunea cu clientul */
									continue;		/* continuam sa ascultam */
								}
								sql = "SELECT * from USERNAME";
								rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
								for (int i = 0 ; i < sizeINFO ; i++)
								{
									if (strcmp(INFO[i].num, numeleutilizatorului) == 0)
									{
										position = i ;
										OKEXIST = 1;
										break;
									}
								}
								if (send(client, &OKEXIST, sizeof(OKEXIST), 0) <= 0)
									// TRIMITEM LA CLIENT  DACA NUMELE USERULUI A FOST GASIT IN DB
								{
									std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
									continue;
								}
								if (OKEXIST == 1)
								{
									int ISOKTYPE = 0;
									//DACA ESTE ADMINISTRATOR NU PUTEM SA IL SETAM ADMINISTRATOR
									if (strcmp(INFO[position].usert, "Administrator") == 0)
										ISOKTYPE = 1 ;
									else
										ISOKTYPE = 0;
									if (send(client, &ISOKTYPE, sizeof(ISOKTYPE), 0) <= 0)
										// TRIMITEM LA CLIENT DACA USERUL ESTE ADMINISTRATOR SAU NU
									{
										std :: cerr << "[server]Eroare la send() catre client" << std :: endl;
										continue;
									}
									if (ISOKTYPE == 0)
									{
										sql = sqlite3_mprintf("UPDATE USERNAME SET USERTYPE = '%s' WHERE NAME LIKE '%s'", "Administrator", numeleutilizatorului);
										rc = sqlite3_exec(db, sql, NULL, (void*)data, &zErrMsg);
										sql = sqlite3_mprintf("UPDATE USERNAME SET VOTE = '%s' WHERE NAME LIKE '%s'", "1", numeleutilizatorului);
										rc = sqlite3_exec(db, sql, NULL, (void*)data, &zErrMsg);

									}
								}
							}
						}
						//PENTRU COMANDA QUIT
						if (recv(client, &OKquit , sizeof(OKquit), MSG_WAITALL) <= 0)
							// DACA UTILIZATORUL A TASTAT QUIT
						{
							std::cerr << "[server]Eroare la recv() de la client";
							close (client);	/* inchidem conexiunea cu clientul */
							continue;		/* continuam sa ascultam */
						}
						if(OKquit == 1)
						{
							break;
						}
					}
				}
			}
			close (client);
			exit(0);
		}
	}
}
