#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    char username[50];
    char password[30];
    int score;
} User;

void sort_users_by_score(User* users, int n){
    User temp;
    for (int i=0; i<n-1; i++){
        for (int j=0; j<n-1-i; j++){
            if (users[i].score < users[i+1].score){
                temp = users[i];
                users[i] = users[i+1];
                users[i+1] = temp;
            }
        }
    }
}

int convert_to_integer(char* string){
    int length = strlen(string);
    int integer = 0, yekan = 0;
    for (int i=length-1; i>=0; i--){
        yekan = string[i] - 48;
        integer = (integer * 10) + yekan;
    }
    return integer;
}

int count_lines(char* file_name){
    FILE* pfile = fopen(file_name, "r");
    int count = 0;
    char c;
    for (c = getc(pfile); c != EOF; c = getc(pfile)){
        if (c == '\n'){
            count++;
        }
    }
    fclose(pfile);
    return count;
}

int login(char* username, char* password, User* users, int n){
    int state = 0;
    for (int i=0; i<n; i++){
        if (strcmp(users[i].username, username) == 0){
            if (strcmp(users[i].password, password) == 0){
                state = 1;
                break;
            }
        }
    }
    return state;
}

int check_username(char* username, User* users, int n){
    int state = 1;
    for (int i=0; i<n; i++){
        if (strcmp(users[i].username, username) == 0){
            state = 0;
            break;
        }
    }
    return state;
}

int check_password(char* password){
    int state = 0;
    int length = strlen(password);
    char c;
    int is_lower = 0, is_upper = 0, is_digit = 0, is_punct = 0;
    for (int i=0; i<length; i++){
        c = password[i];
        is_lower = (islower(c) ? ++is_lower : is_lower);
        is_upper = (isupper(c) ? ++is_upper : is_upper);
        is_digit = (isdigit(c) ? ++is_digit : is_digit);
        is_punct = (ispunct(c) ? ++is_punct : is_punct);
    }
    if (is_lower && is_upper && is_digit && is_punct){
            state = 1;
    }
    return state;
}

int add_user_to_accounts_bank(char* username, char* password, User* users, int n){
    strcpy(users[n].username, username);
    strcpy(users[n].password, password);
    users[n].score = 0;

    int state = 0;
    FILE* file = fopen("accounts_bank.txt", "a");
    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    state = login(username, password, users, n+1);
    
    FILE* pf = fopen("leaderboard.txt", "a");
    fprintf(pf, "%s %d\n", username, 0);
    fclose(pf);

    return state;
}

void add_score(char* username, User* users, int n, int score){
    // updating the users list
    for (int i=0; i<n; i++){
        if (strcmp(users[i].username, username) == 0){
            users[i].score += score;
        }
    }
    // updating the leaderboard file
    FILE* file = fopen("leaderboard.txt", "w");
    for (int i=0; i<n; i++){
        fprintf(file, "%s %d\n", users[i].username, users[i].score);
    }
    fclose(file);
}

void start_the_game(char* word, char* username, User* users, int n){
    int is_loser = 1;

    int length = strlen(word);
    char* word_output = (char*)malloc(length * sizeof(char));
    for (int i=0; i<length; i++){
        word_output[i] = '-';
    }

    int guess_list[26];
    for (int i=0; i<26; i++){
        guess_list[i] = i + 97;     // mirroring the english alphabet
    }

    char* loser_word = (char*)malloc(10 * sizeof(char));
    strcpy(loser_word, "->LOSER");
    int counter = 0, loser_index = 1;       // counter keeps the number of wrong guesses
    int max_try = (length>8 ? 10 : 5);

    while (is_loser == 1 && counter < max_try){
        if (strchr(word_output, '-') == NULL){      // if user has guessed the word
            printf("*%s*\n", word_output);
            is_loser = 0;
            break;
        }

        for (int i=0; i<=loser_index; i++){
            printf("%c", loser_word[i]);
        }

        printf("\n%s", word_output);
        char c;
        printf("%s", "\nEnter your guess: ");
        scanf(" %c", &c);

        if (guess_list[c - 'a'] != 0){
            guess_list[c - 'a'] = 0;
            if (strchr(word, c) == NULL){
                counter++;
                if (counter == max_try){
                    printf("%s\n", loser_word);
                    break;
                }
                printf("Wrong! Try again.\n");
                if (max_try == 10 && (counter%2) == 0){
                    loser_index++;
                } else if (max_try == 5){
                    loser_index++;
                }
            } else {
                for (int i=0; i<length; i++){
                    if (word[i] == c){
                        word[i] = '-';
                        word_output[i] = c;   
                    }
                }
            }
        } else {        // if the user has entered a repeated character
            printf("%s", "Oops! You have already tried that letter.\n");
            printf("%s", "You have tried these letters once: ");
            for (int i=0; i<26; i++){
                if (guess_list[i] == 0){
                    printf("%c ", i+97);
                }
            }
            printf("\n");
        }
    }
    if (is_loser == 1){
        printf("%s", "Game over!");
    } else {
        printf("Congratulations! You won! You gained %d scores.\n", length);
        add_score(username, users, n, length);
    }
    free(word_output);
    free(loser_word);
}

void show_leaderboard(char* file_name, User* users, int n){
    sort_users_by_score(users, n);
    
    printf("%-15s", "Username");
    printf("%s", "          ");
    printf("%5s", "Score\n");
    
    for (int i=0; i<n; i++){
        printf("%-15s", users[i].username);
        printf("%s", "----------");
        printf("%5d\n", users[i].score);
    }
}

char* uncapitalize(char* word){
    char c;
    int length = strlen(word);
    for (int i=0; i<length; i++){
        c = word[i];
        if (c>=65 && c<=90){  // if it's uppercase
            c += 'a' - 'A';
        }
        word[i] = c;
    }
    return word;
}

int check_new_word(char* word){
    int state = 1;
    int length = strlen(word);
    char c;
    for (int i=0; i<length; i++){
        c = word[i];
        if (!(c>=97 && c<=122) || (c>=65 && c<=90)){
            state = -1;
            break;
        }
    }
    if (length <= 5) {
        state = 0;
    }
    return state;
}

int check_word_repetition(char* new_word){
    int state = 1;
    char this_word[20];
    FILE* file = fopen("word_bank.txt", "r");
    while (!(feof(file))){
        fscanf(file, "%s", this_word);
        if (strcmp(this_word, new_word) == 0){
            state = 0;
            break;
        }
    }
    fclose(file);
    return state;
}

int main(){
    // Loading the users' data
    FILE* user_file = fopen("accounts_bank.txt", "r");
    int number_of_users = count_lines("accounts_bank.txt");
    User* users = (User*)malloc(100 * sizeof(User));
    
    char user_info[100];
    char* splited;
    for (int i=0; i<number_of_users; i++){
        fgets(user_info, 100, user_file);
        splited = strtok(user_info, " ");     // username
        strcpy(users[i].username, splited);
        splited = strtok(NULL, " \n");        // password
        strcpy(users[i].password, splited);
    }
    fclose(user_file);
    
    FILE* score_file = fopen("leaderboard.txt", "r");
    for (int i=0; i<number_of_users; i++){
        fgets(user_info, 100, score_file);
        splited = strtok(user_info, " ");   // username -> is useless here
        splited = strtok(NULL, " \n");
        users[i].score = convert_to_integer(splited);
    }
    fclose(score_file);

    // main program starts here
    printf("%s", "Welcome to the Loser Game!\n");
    printf("%s", "You must log in first.\n");
    printf("%s", "1.Login\n2.Signup\n3.Exit\n");

    int valid_action = 0;
    int home_action = 0;
    char username_input[50], password_input[30];
    int login_state = 0;

    while (valid_action == 0){
        scanf("%d", &home_action);
        switch (home_action){
            case 1:
                valid_action = 1;
                while (login_state == 0){
                    printf("%s", "Username: ");
                    scanf("%s", username_input);
                    printf("%s", "Password: ");
                    scanf("%s", password_input);
                    login_state = login(username_input, password_input, users, number_of_users);
                    if (login_state == 1){
                        printf("%s", "You have logged in successfully!\n");
                    } else if (login_state == 0){
                        printf("%s", "Username or password is incorrect. Please try again.\n");
                    }
                }
                break;

            case 2:
                valid_action = 1;
                int signup_state = 0, un_state = 0, pswd_state = 0;
                while (signup_state == 0){
                    while (un_state == 0){
                        printf("%s", "Username: ");
                        scanf("%s", username_input);
                        un_state = check_username(username_input, users, number_of_users);
                        if (un_state == 0){
                            printf("%s", "This username is already taken. Please try again.\n");
                        }
                    }
                    while (pswd_state == 0){
                        printf("%s", "Password(at least 8 characters): ");
                        scanf("%s", password_input);
                        if (strlen(password_input) < 8){
                            printf("Password is too short. Please try again.\n");
                        } else {
                            pswd_state = check_password(password_input);
                            if (pswd_state == 0){
                                printf("Password must include alphanumerics and at least one special character.\n");
                            }
                        }
                    }
                    signup_state = add_user_to_accounts_bank(username_input, password_input, users, number_of_users);
                }
                if (signup_state == 1){
                    printf("You have signed up successfully!\n");
                    number_of_users++;
                    login_state = 1;
                }
                break;

            case 3:
                return 0;

            default:
                printf("%s", "Oops! Invalid input! Please try again: ");
        }
    }

    if (login_state == 1){
        while (1){
            printf("%s", "\n1.Start game\n2.Show leaderboard\n3.New word\n4.Exit\n");
            int action = 0;
            valid_action = 0;
            while (valid_action == 0){
                scanf("%d", &action);
                switch (action){
                    case 1:
                        valid_action = 1;
                        char* file_name = "word_bank.txt";
                        FILE* word_file = fopen(file_name, "r");
                        
                        fseek(word_file, 0, SEEK_END);
                        int size = ftell(word_file);
                        if (size == 0){
                            printf("%s", "Looks like there is no word in the word bank.\n");
                            printf("%s", "First add some words using option 3 and then continue.\n");
                            break;
                        }
                        rewind(word_file);
                        
                        int number_of_lines = count_lines(file_name);
                        long int random = clock();
                        int random_word_number = (random % (number_of_lines - 1 + 1) + 1);  // choosing a random word
                        char** words = (char**)malloc(number_of_lines * sizeof(char*));
                        for (int i=0; i<number_of_lines; i++){
                            words[i] = (char*)malloc(20 * sizeof(char));
                        }

                        for (int i=0; i<number_of_lines; i++){  // loading words from the file
                            fscanf(word_file, "%s", words[i]);
                        }
                        
                        char the_word[20];
                        strcpy(the_word, words[random_word_number - 1]);    // getting a copy of the random word

                        fclose(word_file);
                        for (int i=0; i<number_of_lines; i++){
                            free(words[i]);
                        }
                        free(words);

                        start_the_game(the_word, username_input, users, number_of_users);
                        break;

                    case 2:
                        valid_action = 1;
                        char* lb_file = "leaderboard.txt";
                        show_leaderboard(lb_file, users, number_of_users);
                        break;

                    case 3:
                        valid_action = 1;
                        char* new_word = (char*)malloc(20 * sizeof(char));
                        int word_state = 0, repetition = 0;
                        while (word_state == 0 || repetition == 0){
                            printf("Please type the new word: ");
                            scanf("%s", new_word);
                            word_state = check_new_word(new_word);
                            if (word_state == -1){
                                printf("The word is not valid. Please try again.\n");
                            } else if (word_state == 0) {
                                printf("The word must be at least 6 characters. Please try again.\n");
                            } else {
                                uncapitalize(new_word);
                                repetition = check_word_repetition(new_word);
                                if (repetition == 0){
                                    printf("%s", "Oops! This word already exists in the db.\n");
                                }
                            }
                        }
                        FILE* add_word_file = fopen("word_bank.txt", "a");
                        fprintf(add_word_file, "%s\n", new_word);
                        printf("%s", "The word has been added successfully!");
                        free(new_word);
                        fclose(add_word_file);
                        break;

                    case 4:
                        return 0;
                        
                    default:
                        printf("%s", "Oops! Invalid input! Please try again: ");
                }
            }
        }
    }
    free(users);

    return 0;
}