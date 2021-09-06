#include<iostream>
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
#include <arpa/inet.h>
#include <string>
#include <sqlite3.h>

int port;

struct Infosong {
	char id[100];
	char songname[50];
	char songdescription[100];
	char songtype[100];
	char songcomments[10000];
	char songlink[150];
	char songvote[100];
};

Infosong INFO1[10000];//STRUCTURA PENTRU MELODII

int main(int argc, char *argv[])
{
	int OKL = 0 , OKS = 0 ,  OKDB = 1 , OK1 = 0 , OK2 = 0 , OK3 = 0;
	if (argc != 3)
	{
		printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
		return -1;
	}
	while (OK1 == 0 && OKDB == 1)
	{
		int sd;			// descriptorul de socket
		struct sockaddr_in server;	// structura folosita pentru conectare
		/* stabilim portul */
		port = atoi (argv[2]);

		/* cream socketul */
		if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
		{
			std::cerr << "Eroare la socket().";
			return errno;
		}
		/* umplem structura folosita pentru realizarea conexiunii cu serverul */
		/* familia socket-ului */
		server.sin_family = AF_INET;
		/* adresa IP a serverului */
		server.sin_addr.s_addr = inet_addr(argv[1]);
		/* portul de conectare */
		server.sin_port = htons (port);
		/* ne conectam la server */
		if (connect (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
		{
			std::cerr << "[client]Eroare la connect()" << std::endl;
			return errno;
		}
		std::cout << "Bun venit in aplicatia ";
		std::cout << "\033[1;31mTOP MUSIC.\033[0m " << std::endl;
		std::cout << "\033[1;31m1)\033[0m";
		std::cout << "Tastati comanda ";
		std::cout << "\033[1;31msingup\033[0m " ;
		std::cout << "<NAME> <PASSWORD> pentru a va inregistra." << std::endl;
		std::cout << "\033[1;31m2)\033[0m";
		std::cout << "Tastati comanda ";
		std::cout << "\033[1;31mlogin\033[0m " ;
		std::cout << "<NAME> <PASSWORD> pentru a va loga." << std::endl;
		char comanda[100];
		std::cin.get(comanda, 101);
		std::cin.get();

		if (send (sd, &comanda, sizeof(comanda), 0) <= 0)
			// TRIMITEM COMANDA LOGIN SAU SINGUP SPRE SERVER
		{
			std::cout << "[client]Eroare la send() spre server" << std::endl;
			return errno;
		}
		if (recv (sd, &OKS, sizeof(OKS), MSG_WAITALL) < 0)
			// PRIMIM CONFIRMAREA SINGUP SCRIS OK
		{
			std::cerr << "[client]Eroare la read() de la server." << std::endl;
			return errno;
		}
		if (recv (sd, &OKL, sizeof(OKL), MSG_WAITALL) < 0)
			// PRIMIM CONFIRMAREA LOGIN SCRIS OK
		{
			std::cerr << "[client]Eroare la read() de la server." << std::endl;
			return errno;
		}
		if (recv (sd, &OKDB, sizeof(OKDB), MSG_WAITALL) < 0)
			// A REUSIT SA SE INREGISTREZE
		{
			std::cerr << "[client]Eroare la read() de la server." << std::endl;
			return errno;
		}
		if (recv (sd, &OK1, sizeof(OK1), MSG_WAITALL) < 0)
			// A REUSIT SA SE LOGHEZE CU SUCCESS//NUME PAROLA
		{
			std::cerr << "[client]Eroare la read() de la server." << std::endl;
			return errno;
		}
		if (recv (sd, &OK2, sizeof(OK2), MSG_WAITALL) < 0)
			// DACA ESTI ADMINISTRATOR SAU USER OBISNUIT
		{
			std::cerr << "[client]Eroare la read() de la server." << std::endl;
			return errno;
		}
		if (recv (sd, &OK3, sizeof(OK3), MSG_WAITALL) < 0)
			// DACA ARE DREPT DE VOT SAU NU
		{
			std::cerr << "[client]Eroare la read() de la server." << std::endl;
			return errno;
		}
		// PENTRU SIGNUP
		if (OKS == 1)
		{
			if (OKDB == 0) {
				std :: cout << "Ati reusit sa va inregistrati cu succes la aplicatia ";
				std ::cout << "\033[1;31mTOPMUSIC!\033[0m" << std::endl;
				std::cout << "Aveti la dispozitie urmatoarele comenzi:" << std::endl;
				std::cout << "\033[1;31maddsong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mvotesong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowtop\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31maddcomment\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowcomments\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mQuit\033[0m " << std::endl;

			}
			else
			{
				std ::cout << "\033[1;31m!\033[0m";
				std :: cout << "Numele se afla deja in baza de date!" << std::endl;
			}
		}

		// PENTRU LOGIN
		if (OKL == 1)
		{
			if (OK1 == 1 && OK2 == 1 && OK3 == 1) {
				std :: cout << "V-ati logat cu succes la aplicatia ";
				std ::cout << "\033[1;31mTOPMUSIC ca ADMINISTRATOR - aveti drept de vot!\033[0m" << std::endl;
				std::cout << "Aveti la dispozitie urmatoarele comenzi:" << std::endl;
				std::cout << "\033[1;31maddsong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mvotesong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowtop\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31maddcomment\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowcomments\033[0m ";
				std::cout << std::endl;
				std::cout << "\033[1;31mdeletesong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mrestrictionvote\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31msetusertype\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mQuit\033[0m " << std::endl;
			}
			if (OK1 == 1 && OK2 == 0 && OK3 == 1)
			{
				std :: cout << "V-ati logat cu succes la aplicatia ";
				std ::cout << "\033[1;31mTOPMUSIC ca user OBISNUIT - aveti drept de vot!\033[0m" << std::endl;
				std::cout << "Aveti la dispozitie urmatoarele comenzi:" << std::endl;
				std::cout << "\033[1;31maddsong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mvotesong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowtop\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31maddcomment\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowcomments\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mQuit\033[0m " << std::endl;
			}
			if (OK1 == 1 && OK2 == 0 && OK3 == 0)
			{
				std::cout << "V-ati logat cu succes la aplicatia ";
				std::cout << "\033[1;31mTOPMUSIC ca user OBISNUIT - nu aveti drept de vot!\033[0m" << std::endl;
				std::cout << "Aveti la dispozitie urmatoarele comenzi:" << std::endl;
				std::cout << "\033[1;31maddsong\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowtop\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31maddcomment\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mshowcomments\033[0m ";
				std::cout << ",";
				std::cout << " \033[1;31mQuit\033[0m " << std::endl;
			}
			if (OK1 == 0)
			{
				std ::cout << "\033[1;31m!\033[0m";
				std :: cout << "Nume sau parola gresita." << std :: endl;
			}
		}
		if (OKS == 1 || OKL == 1)
		{
			if (OKDB == 0 || OK1 == 1)
			{
				while (1)
				{
					if (OKDB == 0 || (OK1 == 1 && OK2 == 1) || (OK1 == 1 && OK2 == 0))
					{
						char command[50]; // comanda pe care o citim
						int OKadds = 0, OKshow = 0, OKaddc = 0, OKquit = 0; // VERIFICARILE - CE FEL DE COMANDA ESTE
						int OKshowcom = 0 ;
						int OKvotesong = 0;
						int OKdelete = 0 ;
						int OKrestriction = 0;
						int OKsetusertype = 0;
						std :: cin.getline(command, 50);
						if (strcmp(command, "addsong") == 0)
						{
							OKadds = 1;
							int OKMUSIC = 0 ;
							if (send (sd, &OKadds, sizeof(OKadds), 0) <= 0) // TRIMITEM CONFIRMARE SUNTEM IN ADDSONG
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							char sname[50], sdescription[100], stype[100], slink[150];
							std::cout << "\033[1;31mIntroduceti numele piesei :\033[0m ";
							std :: cin.getline(sname, 50);
							std::cout << "\033[1;31mIntroduceti descrierea piesei :\033[0m ";
							std :: cin.getline(sdescription, 100);
							std::cout << "\033[1;31mIntroduceti genurile muzicale ale piesei prin ',' :\033[0m ";
							std :: cin.getline(stype, 100);
							std::cout << "\033[1;31mIntroduceti Link catre melodia pe care ati adaugat-o :\033[0m ";
							std :: cin.getline(slink, 50);
							if (send (sd, &sname, sizeof(sname), 0) <= 0) // TRIMITEM NUME PIESA
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (send (sd, &sdescription, sizeof(sdescription), 0) <= 0) // TRIMITEM DESCRIERE
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (send (sd, &stype, sizeof(stype), 0) <= 0) // TRIMITEM GENUL
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (send (sd, &slink, sizeof(slink), 0) <= 0) // TRIMITEM LINK
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (recv (sd, &OKMUSIC, sizeof(OKMUSIC), MSG_WAITALL) < 0)
								// PRIMIM CONFIRMAREA CA MELODIA S-A PUTUT ADAUGA
							{
								std::cerr << "[client]Eroare la read() de la server." << std::endl;
								return errno;
							}
							// OKMUSIC == 1 INSEAMNA CA MELODIA NU S-A PUTUT ADAUGA
							if (OKMUSIC == 1) {
								std :: cout << "Melodia nu s-a putut adauga in aplicatia ";
								std ::cout << "\033[1;31mTOPMUSIC!(numele melodiei deja exista sau este gresit).\033[0m" << std::endl;
							}
							else
							{
								std :: cout << "\033[1;32mMelodia s-a adaugat cu succes in aplicatia\033[0m";
								std :: cout << ' ';
								std ::cout << "\033[1;31mTOPMUSIC!\033[0m" << std::endl;
							}
						}
						else
						{
							if (send (sd, &OKadds, sizeof(OKadds), 0) <= 0) // TRIMITEM CONFIRMARE CHIAR DACA NU S-A TASTAT ADD SONG
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						//LUCRAM PENTRU COMANDA showtop ALL|GENURI
						if (strcmp(command, "showtop") == 0)
						{
							int OKALL = 0, OKGenuri = 0;
							OKshow = 1;
							if (send (sd, &OKshow, sizeof(OKshow), 0) <= 0) // TRIMITEM CONFIRMARE SUNTEM IN SHOW
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							char showtype[10];
							std::cout << "\033[1;31mIntroduceti tipul (ALL|Genuri) :\033[0m ";
							std :: cin.getline(showtype, 10);
							if (strcmp(showtype, "ALL") == 0)
							{
								OKALL = 1;
								int songnumber = 0;
								if (send (sd, &OKALL, sizeof(OKALL), 0) <= 0) // TRIMITEM CONFIRMARE SUNTEM IN ALL
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &OKGenuri, sizeof(OKGenuri), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NU SUNTEM IN GENURI
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (recv (sd, &songnumber, sizeof(songnumber), MSG_WAITALL) < 0)
									// PRIMIM CATE MELODII SUNT IN DB
								{
									std::cerr << "[client]Eroare la read() de la server." << std::endl;
									return errno;
								}
								if (recv (sd, &INFO1, sizeof(INFO1), MSG_WAITALL) < 0)
									// PRIMIM STRUCTURA CU MELODIILE SORTATE
								{
									std::cerr << "[client]Eroare la read() de la server." << std::endl;
									return errno;
								}
								for (int i = 0 ; i < songnumber ; i++) // AFISAM MELODIILE SORTATE DUPA NR DE VOTURI
								{
									std ::cout << INFO1[i].id ;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songname;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songdescription;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songtype;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songlink;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songvote;
									std ::cout << "\033[1;32m|\033[0m";
									std :: cout << std :: endl;
								}

							}
							else if (strcmp(showtype, "Genuri") == 0)
							{
								char introducetigen[30];
								int sizeMUSIC = 0;
								std::cout << "\033[1;31mIntroduceti genurile prin ',' :\033[0m ";
								std :: cin.getline(introducetigen, 30);
								OKGenuri = 1;
								if (send (sd, &OKALL, sizeof(OKALL), 0) <= 0)
									//TRIMITEM CONFIRMARE CA NU SUNTEM IN ALL
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &OKGenuri, sizeof(OKGenuri), 0) <= 0) // TRIMITEM CONFIRMARE SUNTEM IN GENURI
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &introducetigen, sizeof(introducetigen), 0) <= 0)
									// TRIMITEM GENURILE DUPA CARE TREBUIE SA SORTAM
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (recv (sd, &sizeMUSIC, sizeof(sizeMUSIC), MSG_WAITALL) < 0)
									// PRIMIM CATE MELODII SUNT IN DB
								{
									std::cerr << "[client]Eroare la read() de la server." << std::endl;
									return errno;
								}
								if (recv (sd, &INFO1, sizeof(INFO1), MSG_WAITALL) < 0)
									// PRIMIM STRUCTURA CU MELODIILE SORTATE DUPA GEN
								{
									std::cerr << "[client]Eroare la read() de la server." << std::endl;
									return errno;
								}
								for (int i = 0 ; i < sizeMUSIC ; i++) // AFISAM MELODIILE SORTATE DUPA NR DE VOTURI
								{
									std ::cout << INFO1[i].id ;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songname;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songdescription;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songtype;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songlink;
									std ::cout << "\033[1;32m|\033[0m";
									std ::cout << INFO1[i].songvote;
									std ::cout << "\033[1;32m|\033[0m";
									std :: cout << std :: endl;
								}
								if (sizeMUSIC == 0)
								{
									std ::cout << "\033[1;31m!\033[0m";
									std :: cout << "Genul ales nu exista in baza de date TOPMUSIC." << std::endl;
								}

							}
							else
							{
								if (send (sd, &OKALL, sizeof(OKALL), 0) <= 0)
									//TRIMITEM CONFIRMARE CA NU SUNTEM IN ALL
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &OKGenuri, sizeof(OKGenuri), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NU SUNTEM IN GENURI
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								std ::cout << "\033[1;31m!\033[0m";
								std :: cout << "Comanda introdusa este gresita." << std :: endl;
							}
						}
						else
						{
							if (send (sd, &OKshow, sizeof(OKshow), 0) <= 0) // TRIMITEM CONFIRMARE CA NU SUNTEM IN SHOW
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						//LUCRAM PENTRU COMANDA DE ADAUGAT COMENTARII addcomment
						if (strcmp(command, "addcomment") == 0)
						{
							OKaddc = 1; // INSEAMNA CA CLIENTUL A TASTAT addcomment
							int OKcomvalid = 0 ;
							char introduceticomentariu[500];
							char numelemelodiei[100];
							if (send (sd, &OKaddc, sizeof(OKaddc), 0) <= 0) // TRIMITEM CONFIRMARE CA SUNTEM IN addc
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							std::cout << "\033[1;31mIntroduceti numele melodiei :\033[0m ";
							std :: cin.getline(numelemelodiei, 100);
							std::cout << "\033[1;31mIntroduceti comentariul :\033[0m ";
							std :: cin.getline(introduceticomentariu, 500);
							if (strcmp(introduceticomentariu, "") == 0)
							{
								OKcomvalid = 0;
								if (send (sd, &OKcomvalid, sizeof(OKcomvalid), 0) <= 0)
									// TRIMITEM CA ESTE UN COMENTARIU RAU
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								std ::cout << "\033[1;31m!\033[0m";
								std :: cout << "Nu puteti adauga un comentariu gol." << std::endl;
							}
							//TRIMITEM SPRE SERVER COMENTARIUL SI NUMELE MELODIEI UNDE TREBUIE ADAUGA
							else
							{
								OKcomvalid = 1;
								int OKEXIST  = 0;
								if (send (sd, &OKcomvalid, sizeof(OKcomvalid), 0) <= 0)
									// TRIMITEM DACA ESTE UN COMENTARIU BUN
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &numelemelodiei, sizeof(numelemelodiei), 0) <= 0)
									// TRIMITEM NUMELE MELODIEI LA CARE TREBUIE ADAUGAT COMENTARIUL
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &introduceticomentariu, sizeof(introduceticomentariu), 0) <= 0)
									// TRIMITEM COMENTARIUL CARE TREBUIE ADAUGAT
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (recv (sd, &OKEXIST, sizeof(OKEXIST), MSG_WAITALL) < 0)
									// PRIMIM DACA COMENTARIUL S-A PUTUT ADAUGA SAU NU
								{
									std::cerr << "[client]Eroare la read() de la server." << std::endl;
									return errno;
								}
								if (OKEXIST == 1)
									std :: cout << "Comentariul a fost adaugat cu succes!" << std :: endl;
								else
								{	std ::cout << "\033[1;31m!\033[0m";
									std :: cout << "Comentariul nu a fost adaugat(numele melodiei nu exista in DB)." << std::endl;
								}
							}
						}
						else
						{
							if (send (sd, &OKaddc, sizeof(OKaddc), 0) <= 0) // TRIMITEM CONFIRMARE CA NU SUNTEM IN addc
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						//COMANDA PRIN CARE ARATAM COMENTARIILE LA O MELODIE
						if (strcmp(command, "showcomments") == 0)
						{
							OKshowcom = 1;
							int OKnumvalid = 0;
							char numelemelodiei[100];
							std::cout << "\033[1;31mIntroduceti numele melodiei la care vreti sa afisati comentariile:\033[0m ";
							std :: cin.getline(numelemelodiei, 100);
							if (send (sd, &OKshowcom, sizeof(OKshowcom), 0) <= 0)
								// TRIMITEM CONFIRMARE CA SUNTEM IN SHOWCOMMENTS
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (strcmp(numelemelodiei, "") == 0)
							{
								OKnumvalid = 0;
								std :: cout << "Nu ati scris o melodie." << std::endl;
								if (send (sd, &OKnumvalid, sizeof(OKshowcom), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NUMELE ESTE GRESIT
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
							}
							else
							{
								OKnumvalid = 1;
								int OKEXIST  = 0;
								if (send (sd, &OKnumvalid, sizeof(OKshowcom), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NUMELE ESTE CORECT SCRIS
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (send (sd, &numelemelodiei, sizeof(numelemelodiei), 0) <= 0)
									// TRIMITEM NUMELE MELODIEI PENTRU CARE TREBUIE AFISATE COMENTARIILE
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (recv (sd, &OKEXIST, sizeof(OKEXIST), MSG_WAITALL) < 0)
									// PRIMIM DACA COMENTARIUL S-A PUTUT ADAUGA SAU NU
								{
									std::cerr << "[client]Eroare la read() de la server." << std::endl;
									return errno;
								}
								if (OKEXIST == 1)
								{
									char recivecomments[10000];
									if (recv (sd, &recivecomments, sizeof(recivecomments), MSG_WAITALL) < 0)
										// PRIMIM COMENTARIILE DESPRE MELODIA PE CARE AM DAT-O
									{
										std::cerr << "[client]Eroare la read() de la server." << std::endl;
										return errno;
									}
									std ::cout << "\033[1;32mComentarii pentru melodia\033[0m";
									std :: cout << ' ' << numelemelodiei << ':';
									std::cout << std :: endl;
									std :: cout << recivecomments << std ::endl;
								}
								else
								{	std ::cout << "\033[1;31m!\033[0m";
									std :: cout << "Numele melodiei nu exista in DB." << std::endl;
								}
							}
						}
						else
						{
							if (send (sd, &OKshowcom, sizeof(OKshowcom), 0) <= 0)
								// TRIMITEM CONFIRMARE CA NU SUNTEM IN SHOWCOMMENTS
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						if (OKDB == 0 || (OK1 == 1 && OK2 == 1) || (OK1 == 1 && OK2 == 0 && OK3 == 1))
						{
							if (strcmp(command, "votesong") == 0)
							{
								OKvotesong = 1;
								char numelemelodiei[100];
								int OKnumvalid = 0;
								std::cout << "\033[1;31mIntroduceti numele melodiei pe care vreti sa o votati:\033[0m ";
								std :: cin.getline(numelemelodiei, 100);
								if (send (sd, &OKvotesong, sizeof(OKvotesong), 0) <= 0)
									// TRIMITEM CONFIRMARE CA SUNTEM IN VOTESONG
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								if (strcmp(numelemelodiei, "") == 0)
								{
									OKnumvalid = 0;
									std :: cout << "Nu ati scris o melodie." << std::endl;
									if (send (sd, &OKnumvalid, sizeof(OKshowcom), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE GRESIT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
								}
								else
								{
									OKnumvalid = 1;
									int OKEXIST  = 0;
									if (send (sd, &OKnumvalid, sizeof(OKshowcom), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE CORECT SCRIS
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (send (sd, &numelemelodiei, sizeof(numelemelodiei), 0) <= 0)
										// TRIMITEM NUMELE MELODIEI PE CARE TRB SA O VOTAM
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (recv (sd, &OKEXIST, sizeof(OKEXIST), MSG_WAITALL) < 0)
										// PRIMIM DACA NUMELE EXISTA IN BAZA DE DATE
									{
										std::cerr << "[client]Eroare la read() de la server." << std::endl;
										return errno;
									}
									if (OKEXIST == 1) {
										std ::cout << "\033[1;32mMelodia\033[0m";
										std :: cout << ' ' << numelemelodiei;
										std :: cout << ' ';
										std ::cout << "\033[1;32ma fost votata!\033[0m" << std :: endl;
									}
									else {
										std :: cout << "Melodia nu exista in baza de date ";
										std ::cout << "\033[1;31mTOPMUSIC!\033[0m" << std :: endl;
									}

								}
							}
							else
							{
								if (send (sd, &OKvotesong, sizeof(OKvotesong), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NU SUNTEM IN VOTESONG
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
							}
						}
						else
						{
							if (send (sd, &OKvotesong, sizeof(OKvotesong), 0) <= 0)
								// TRIMITEM CONFIRMARE CA SUNTEM IN VOTESONG
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						if (OK1 == 1 && OK2 == 1) // INSEAMNA CA E LOGAT SI E ADMINISTRATOR
							// FACEM SI COMENZILE DELETE SONG / RESTRICTION VOTE
						{
							if (strcmp(command, "deletesong") == 0)
							{
								OKdelete = 1;
								char numelemelodiei[100];
								int OKnumvalid = 0;
								if (send (sd, &OKdelete, sizeof(OKdelete), 0) <= 0)
									// TRIMITEM CONFIRMARE CA SUNTEM IN COMANDA DELETE
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								std::cout << "\033[1;31mIntroduceti numele melodiei pe care vreti sa o stergeti:\033[0m ";
								std :: cin.getline(numelemelodiei, 100);
								if (strcmp(numelemelodiei, "") == 0)
								{
									OKnumvalid = 0;
									std :: cout << "Nu ati scris o melodie." << std::endl;
									if (send (sd, &OKnumvalid, sizeof(OKnumvalid), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE GRESIT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
								}
								else
								{
									OKnumvalid = 1 ;
									int OKEXIST  = 0;
									if (send (sd, &OKnumvalid, sizeof(OKshowcom), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE CORECT SCRIS
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (send (sd, &numelemelodiei, sizeof(numelemelodiei), 0) <= 0)
										// TRIMITEM NUMELE MELODIEI PE CARE TRB SA O VOTAM
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (recv (sd, &OKEXIST, sizeof(OKEXIST), MSG_WAITALL) < 0)
										// PRIMIM DACA MELODIA EXISTA IN BAZA DE DATE
									{
										std::cerr << "[client]Eroare la read() de la server." << std::endl;
										return errno;
									}
									if (OKEXIST == 1)
									{
										std ::cout << "\033[1;32mMelodia\033[0m";
										std :: cout << ' ' << numelemelodiei;
										std :: cout << ' ';
										std ::cout << "\033[1;32ma fost stersa!\033[0m" << std :: endl;
									}
									else {
										std :: cout << "Melodia nu exista in baza de date ";
										std ::cout << "\033[1;31mTOPMUSIC!\033[0m" << std :: endl;
									}
								}
							}
							else
							{
								if (send (sd, &OKdelete, sizeof(OKdelete), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NU SUNTEM IN DELETESONG
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
							}

							//PENTRU COMANDA RESTRICTION VOTE
							if (strcmp(command, "restrictionvote") == 0)
							{
								OKrestriction = 1;
								char username[300];
								int OKusernamevalid = 0;
								if (send (sd, &OKrestriction, sizeof(OKrestriction), 0) <= 0)
									// TRIMITEM CONFIRMARE CA SUNTEM IN COMANDA RESTRICTIONVOTE
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								std::cout << "\033[1;31mIntroduceti numele utilizatorul:\033[0m ";
								std :: cin.getline(username, 100);
								if (strcmp(username, "") == 0)
								{
									OKusernamevalid = 0;
									std :: cout << "Trebuie sa scrieti neaparat un username." << std::endl;
									if (send (sd, &OKusernamevalid, sizeof(OKusernamevalid), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE GRESIT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
								}
								else
								{
									OKusernamevalid = 1;
									int OKEXIST  = 0;
									if (send (sd, &OKusernamevalid, sizeof(OKusernamevalid), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE CORECT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (send (sd, &username, sizeof(username), 0) <= 0)
										// TRIMITEM NUMELE USERULUI LA CARE TREBUIE SA ANULAM DREPTUL DE VOT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (recv (sd, &OKEXIST, sizeof(OKEXIST), MSG_WAITALL) < 0)
										// PRIMIM DACA NUMELE ESTE IN BAZA DE DATE
									{
										std::cerr << "[client]Eroare la read() de la server." << std::endl;
										return errno;
									}
									if (OKEXIST == 1)
									{
										int ISOKTYPE = 0;
										if (recv (sd, &ISOKTYPE, sizeof(ISOKTYPE), MSG_WAITALL) < 0)
											// PRIMIM DACA USERUL ESTE ADMINISTRATOR SAU NU
										{
											std::cerr << "[client]Eroare la read() de la server." << std::endl;
											return errno;
										}
										if (ISOKTYPE == 1)
										{
											std :: cout << "Nu puteti restrictiona votul utilizatorului ";
											std :: cout << username << ' ';
											std :: cout << "deoarece este ";
											std ::cout << "\033[1;31mAdministrator!\033[0m" << std :: endl;

										}
										else
											std ::cout << "\033[1;32mDreptul de vot a fost setat cu succes!\033[0m" << std :: endl;
									}
									else {
										std :: cout << "Numele nu exista in baza de date ";
										std ::cout << "\033[1;31mTOPMUSIC!\033[0m" << std :: endl;
									}

								}
							}
							else
							{
								if (send (sd, &OKrestriction, sizeof(OKrestriction), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NU SUNTEM IN RESTRICTIONVOTE
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
							}

							//PENTRU COMANDA SETUSERTYPE - ADMINISTRATOR ALTI USERI
							if (strcmp(command, "setusertype") == 0)
							{
								OKsetusertype = 1;
								char username[300];
								int OKusernamevalid = 0;
								if (send (sd, &OKsetusertype, sizeof(OKsetusertype), 0) <= 0)
									// TRIMITEM CONFIRMARE CA SUNTEM IN SETUSERTYPE
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
								std::cout << "\033[1;31mIntroduceti numele utilizatorul:\033[0m ";
								std :: cin.getline(username, 100);
								if (strcmp(username, "") == 0)
								{
									OKusernamevalid = 0;
									std :: cout << "Trebuie sa scrieti neaparat un username." << std::endl;
									if (send (sd, &OKusernamevalid, sizeof(OKusernamevalid), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE GRESIT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
								}
								else
								{
									OKusernamevalid = 1;
									int OKEXIST  = 0;
									if (send (sd, &OKusernamevalid, sizeof(OKusernamevalid), 0) <= 0)
										// TRIMITEM CONFIRMARE CA NUMELE ESTE CORECT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (send (sd, &username, sizeof(username), 0) <= 0)
										// TRIMITEM NUMELE USERULUI LA CARE TREBUIE SA ANULAM DREPTUL DE VOT
									{
										std::cout << "[client]Eroare la send() spre server" << std::endl;
										return errno;
									}
									if (recv (sd, &OKEXIST, sizeof(OKEXIST), MSG_WAITALL) < 0)
										// PRIMIM DACA NUMELE ESTE IN BAZA DE DATE
									{
										std::cerr << "[client]Eroare la read() de la server." << std::endl;
										return errno;
									}
									if (OKEXIST == 1)
									{
										int ISOKTYPE = 0;
										if (recv (sd, &ISOKTYPE, sizeof(ISOKTYPE), MSG_WAITALL) < 0)
											// PRIMIM DACA USERUL ESTE ADMINISTRATOR SAU NU
										{
											std::cerr << "[client]Eroare la read() de la server." << std::endl;
											return errno;
										}
										if (ISOKTYPE == 1)
										{
											std :: cout << "Nu puteti sa setati utilizatorului ";
											std :: cout << username << ' ';
											std :: cout << "deoarece este ";
											std ::cout << "\033[1;31mdeja Administrator!\033[0m" << std :: endl;

										}
										else
										{
											std ::cout << "\033[1;32mUtilizatorul este acum \033[0m";
											std ::cout << "\033[1;32mAdministrator!\033[0m" << std :: endl;

										}
									}
									else {
										std :: cout << "Numele nu exista in baza de date ";
										std ::cout << "\033[1;31mTOPMUSIC!\033[0m" << std :: endl;
									}
								}
							}
							else
							{
								if (send (sd, &OKsetusertype, sizeof(OKsetusertype), 0) <= 0)
									// TRIMITEM CONFIRMARE CA NU SUNTEM IN SETUSERTYPE
								{
									std::cout << "[client]Eroare la send() spre server" << std::endl;
									return errno;
								}
							}

						}
						else
						{
							if (send (sd, &OKdelete, sizeof(OKdelete), 0) <= 0)
								// TRIMITEM CONFIRMARE CA NU SUNTEM IN DELETESONG
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (send (sd, &OKrestriction, sizeof(OKrestriction), 0) <= 0)
								// TRIMITEM CONFIRMARE CA NU SUNTEM IN RESTRICTIONVOTE
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							if (send (sd, &OKsetusertype, sizeof(OKsetusertype), 0) <= 0)
								// TRIMITEM CONFIRMARE CA NU SUNTEM IN SETUSERTYPE
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						if (strcmp(command, "Quit") == 0)
						{
							OKquit = 1;
							std ::cout << "\033[1;32mAti iesit cu succes din aplicatia TOPMUSIC!\033[0m" << std :: endl;
							if (send (sd, &OKquit, sizeof(OKquit), 0) <= 0)
								// TRIMITEM CONFIRMARE CA SUNTEM IN QUIT
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
							break; // AM IESIT DIN CLIENT CU SUCCES
						}
						else
						{
							if (send (sd, &OKquit, sizeof(OKquit), 0) <= 0)
								// TRIMITEM CONFIRMARE CA SUNTEM IN QUIT
							{
								std::cout << "[client]Eroare la send() spre server" << std::endl;
								return errno;
							}
						}
						if (OKadds == 0 && OKshow == 0 && OKaddc == 0 && OKshowcom == 0 && OKvotesong == 0 && OKdelete == 0 && OKrestriction == 0 && OKsetusertype == 0 && OKquit == 0)
						{
							std ::cout << "\033[1;31m!\033[0m";
							std::cout << "Comanda dumnevoastra nu se afla in lista de mai sus." << std::endl;
						}
					}
					std ::cout << "\033[1;35mIntroduceti alta comanda sau 'Quit' pentru a iesi din aplicatie.\033[0m" << std :: endl;
				}
			}
		}
		if (OKS == 0 && OKL == 0)
		{
			std ::cout << "\033[1;31m!\033[0m";
			std :: cout << "Comanda introdusa este gresita,incercati din nou." << std :: endl;
		}
	}
}