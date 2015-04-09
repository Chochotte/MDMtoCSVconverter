/*************************************************************************************
*				Programme de conversion de fichier MDM (IC-CAP) 					 *
*                    vers le format CSV (géré par EXCEL)                             *
**************************************************************************************
																by Laurent VARIZAT

	INFOS :
		1) Ce programme prend en argument :
				- le fichier à modifier

		2) On doit avoir : /0/
																					 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMA ';'
#define NB_PARAM 8
#define TAILLE_MAX 1000

/* ... */
int main (int argc, char * argv[])
{
    FILE * 	fancien 	= NULL;
    FILE * 	fnouveau 	= NULL;
    char 	a, b, c     = COMMA;
    char * 	chaine 		= (char *) malloc(sizeof(char) * TAILLE_MAX);
	char * 	ptrChaine 	= NULL;
	int 	nb_modif 	= 0; 	// compte le nb de ligne modifiée : 4 pour l'en-tete...
	int 	cpt_ligne	= 0; 	// compte le nombre de ligne apres une ligne X
	float	params[NB_PARAM];

    printf("Execution du programme de conversion de fichier MDM vers CSV ...\n");

    // Récupération du nom de fichier
    if (argc != 2) {
        printf("Veuillez entrer 1 argument : \
               \n - le nom du fichier a transformer \n");
        return 0;
    }

 	// Ouverture des fichiers (l'ancien et le nouveau)
    strcpy(chaine, argv[1]);
    fancien = fopen(chaine, "r");
    if (!fancien) {
        printf("Erreur lors de l'ouverture du fichier %s", chaine);
        exit(1);
    }

    strcat(chaine, ".csv");
    fnouveau = fopen(chaine, "w+");
    if (!fnouveau) {
        printf("Erreur lors de la création du fichier %s", argv[2]);
        exit(1);
    }

    // On parcours le fichier tant qu'on a pas d'erreur
    while (fgets(chaine, TAILLE_MAX, fancien)) {

		sscanf(chaine, " %c%c ", &a, &b); // On récupère les 2 premières lettres de la chaine

		/* MODIFICATION DE L'EN-TETE */
		if (nb_modif < 8 && (a == 'v' || a == 'i')) { 	// si je suis dans l'en-tete
            // On écrit l'en-tete du convertisseur
            fprintf(fnouveau, "%c%c%c", a, b, c);
            nb_modif++;
            if (nb_modif == 8) fprintf(fnouveau, "\n"); // Retour à la ligne
		}

		/* ON MODIFIE LE CORP DU FICHIER */
		else if (nb_modif > 7) {
			// Si notre ligne courante fait partie de la description des stimulis vd, vb, vs ou/et vg
			if ((ptrChaine = strstr(chaine, "ICCAP_VAR")) != NULL) {
				ptrChaine += 9 * sizeof(char); // On se place apres le mot ICCAP_VAR

                    // On récupère les infos importante de la ligne (variable + valeur)
                if (sscanf(ptrChaine, " %c%c %f ", &a, &b, &params[cpt_ligne]) != 3)
                    printf("WARNING : Parametre manquant  !!\n");

                cpt_ligne++;
			}

            else if ((ptrChaine = strstr(chaine, "END_DB")) != NULL) {
                cpt_ligne	= 0;
                fprintf(fnouveau, "\n");
            }

			else if (cpt_ligne > 0 && cpt_ligne < 5) cpt_ligne++;

			else if (cpt_ligne >= 5) {	// On remplit le tableau
				if (sscanf(chaine, " %f %f %f %f %f ", &params[3], &params[4], &params[5], &params[6], &params[7]) != 5)
					printf("WARNING : Les 5 mesures v + 4i n'ont pas été récupéré correctement\n");

                    // On écrit les valeurs mesurée dans le nouveau fichier
				fprintf(fnouveau, "%f%c%f%c%f%c%f%c%e%c%e%c%e%c%e\n", params[0], c, params[1], c, params[2], c, params[3], c, params[4], c, params[5], c, params[6], c, params[7]);
			}
		}
    }

    fclose(fancien);
    fclose(fnouveau);

	printf("SUCCESS\n\n");

    return EXIT_SUCCESS;
}
