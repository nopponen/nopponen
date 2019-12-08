/* Projektiin on käytetty apuja TIMistä löytyvistä funktioista tietenkin tehtävään liittyvillä modifikaatioilla. Apufunktiot ovat lähes kokonaan itsetehtyjä. Nauttikaa. */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct student {
	char *student_name;
	int opiskelija_numero;
	int points[6];
};

struct student *add_student(struct student *arr, int op_nro, char *name) {
    int i;
    for (i = 0; arr[i].student_name != NULL; i++);

    struct student *newarray = realloc(arr, sizeof(struct student) * (i + 2));

    if (newarray == NULL) {
    	printf("Allocating memory did not work.\n");
        return NULL;
    }

    char *name_set = malloc((strlen(name))*sizeof(char)+1);			
    strcpy(name_set, name);									//Kopioidaan student_name omaksi muuttujaksi.

    newarray[i].student_name = name_set;
    newarray[i].opiskelija_numero = op_nro;
    memset(newarray[i].points, 0, sizeof(int) * 6);									
    
    newarray[i+1].student_name = NULL;			// Lisätään tyhjä loppuun
    return newarray;
}

int count_sum(int *list) {		//summataan pisteet.

	int sum = 0;
	for (int i = 0; i < 6; i++ ) {
		sum = sum + list[i];
	}

	return sum;
}

void print_array(struct student *arr){			//Printataan funktion elementit

	int sum;
    for (int i = 0; arr[i].student_name != NULL; i++) {
        printf("Name: %s\nStudent Number: %d\n", arr[i].student_name, arr[i].opiskelija_numero);
        printf("Points: ");
        for (int j = 0; j < 6; j++) {												
            printf("%d  ", arr[i].points[j]);
            sum = count_sum(arr[i].points);
        }
        printf("\nTotal points: %d\n\n", sum);

    }
}

int get_student_number(const char *text, int start) {	//Etsitään oppilasta numerolla.
	
	int std_nro = 0;
	int student_number;
			
	for (int i = start; text[i] != ' '; i++) {			//Opiskelijan pituuden etsiminen
		std_nro++;
	}

	if (strlen(text)+2 < std_nro) {		//Opiskelijaa ei löydy.
		return 0;																	
	}

	char student_num[6*sizeof(int)] = "";
	strncpy(student_num, text + 2, std_nro);

	int ret = sscanf(student_num, "%d", &student_number);
	if (ret != 1) {			//Opnro ei löydy.
		return 0;
	}
	return student_number;
}

int get_digits(int number) {	//lasketaan numeroiden määrä
	int rets = number/10;
	int digits = 1;
	while (rets != 0) {
		digits++;
		rets = rets/10;
	}
	return digits;
}


int find_student(struct student *arr, int stdnum) { 	//etsitään opiskelija numeroa listasta (Arraysta).
	int i;
	int f = 0;

	for (i = 0; arr[i].student_name != NULL; i++) {
		if (arr[i].opiskelija_numero == stdnum) {
			f++;
			break;
		}
	}

	if (f == 0) {
		return -1;
	}
	else {
		return i;	//Millä indeksillä löytyy
	}
}

const struct student *reorder(struct student *arr) {
    //Järjestetään jono (arr).

	int i, j;
	struct student a;

	for (i = 0; arr[i].student_name != NULL; i++) {
		for (j = i+1; arr[j].student_name != NULL; j++) {
			int sum1 = count_sum(arr[i].points);
			int sum2 = count_sum(arr[j].points);
			if (sum2 > sum1) {
				a = arr[i];
				arr[i] = arr[j];
				arr[j] = a;
			}
		}
	}
	return arr;
}

void write_into_file(struct student *arr, const char *filename) {

	FILE *filu = fopen(filename, "w");

	if (!filu) {
		fprintf(stderr, "Could not open file\n");
		exit(EXIT_FAILURE);
	}

	int sum;
	for (int i = 0; arr[i].student_name != NULL; i++) {
		fprintf(filu, "%s %d ", arr[i].student_name, arr[i].opiskelija_numero);
		for (int j = 0; j < 6; j++) {
            fprintf(filu, "%d  ", arr[i].points[j]);
            sum = count_sum(arr[i].points);
        }
        fprintf(filu,"%d\n", sum);
	}
	fclose(filu);
}

struct student *override(struct student *arr, const char *filename) {  //Ladataan infomraatio tiedostosta ja käsitellään sitä.

	FILE *filu = fopen(filename, "r");

    
	if (!filu) {
		fprintf(stderr, "Could not open file\n");
		return NULL;
	}

    
	char rivi[50*sizeof(char)];
	char name[20*sizeof(char)];
	int opnumero;
	int sum;
	int pisteita[6] = {0};
	int n;

    
	for (n = 0; fgets(rivi, 50, filu); n++) {		//Luetaan niin kauan kuin informaatiota riveillä riittää.
		arr = realloc(arr, (n+2)*sizeof(struct student));
		sscanf(rivi, "%s %d %d %d %d %d %d %d %d", name, &opnumero, &pisteita[0], &pisteita[1] ,&pisteita[2], &pisteita[3], &pisteita[4], &pisteita[5], &sum);
		for (int i = 0; i < 6; i++) {
			arr[n].points[i] = pisteita[i];
		}
		char *name_set = malloc(20*sizeof(char));		//Asetetaan muuttujiksi
		strcpy(name_set, name);
		arr[n].student_name = name_set;
		arr[n].opiskelija_numero = opnumero;
	}
	
	arr[n+1].student_name = NULL;
	return arr;
}

int main() {

	int onnistui = 0;
	struct student *arr = malloc(sizeof(struct student));
	arr[0].student_name = NULL;
	int student_number, digits;
	size_t name_length;
	char filename[20*sizeof(char)] = "";

	while (onnistui == 0) {
		printf("Give text (press enter when done)\n");
		char text[30];
		fgets(text, sizeof(text), stdin);
		char command = toupper(text[0]);

		switch(command) {

			case 'A':		//lisätään opiskelija

			student_number = get_student_number(text, 2);

			if (student_number == 0) {
				printf("give real number\n");
				break;
			}

			else {
				name_length = 0;
				digits = get_digits(student_number);	//Opiskelijan numero nimen perusteella
				for (int i = 3 + digits; text[i] != '\n'; i++ ) {
					name_length++; 
				}
                
				if (name_length > 20) {
					printf("The given name is too long (20 characters).\n");
					break;
				}

				char name[20*sizeof(char)] = "";
				strncpy(name, text + 3 + digits, name_length);
				arr = add_student(arr,student_number, name);
			}			
			break;


			case 'U':		//Pisteiden päivitys

			student_number = get_student_number(text, 2);
			digits = get_digits(student_number);

			const char *dig = text + 3 + digits;		//Tehtävä nro
			int kierros = atoi(dig);

			char pist[2*sizeof(char)];
			strncpy(pist, text + 5 + digits, 2);
			int pisteet = atoi(pist);

			int index = find_student(arr, student_number);
			
			if (student_number == 0) {					//Varmistetaan, että pisteet ovat oikeassa järkässä ja tehtävissä.
				printf("give real number\n");
				break;
			}

			else if (index == -1) {
				printf("The number you stated cannot be found.\n");
				break;			
			}

			else if (kierros == 0 || kierros > 6) {
				printf("The round number you gave is invalid.\n");
				break;
			}

			else if (pisteet == 0) {
				printf("Give a proper amount of points (more than zero or an integer).\n");
				break;
			}
			else {
				arr[index].points[kierros-1] = pisteet;

			}
			break;



			case 'L': 	//Printataan

			reorder(arr);
			print_array(arr);
			break;



			case 'W':	//Kirjoitetaan filuksi
			
			name_length = 0;
			for (int i = 2; text[i] != '\n'; i++ ) {
				name_length++; 
			}
			
			strncpy(filename, text + 2, name_length);
			write_into_file(arr, filename);
			break;


			case 'O': 	//Overridetaan filusta

			name_length = 0;
			for (int i = 2; text[i] != '\n'; i++ ) {
				name_length++; 
			}
			char filename[20*sizeof(char)] = "";
			strncpy(filename, text + 2, name_length);
			filename[name_length+1] = '\0';
			

			struct student *replace = override(arr, filename); 
			if (replace == NULL) {
				printf("File not found. Do not quit process.\n");
			}

			else {
				arr = replace;
			}
			break;


			case 'Q':		//lopetus
			onnistui++;
			break;

			default:
			printf("Invalid input! Give a proper first command!\n");

		}

	}
	for (int i = 0; arr[i].student_name != NULL; i++) { 		//Vapautetaan muisti
		free(arr[i].student_name);
	}
	free(arr);
	return 0;
}
