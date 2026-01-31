/*
 * quiz.c (Updated for Auth & Leaderboard Link)
 * THIS VERSION HAS THE TEXT COLOR FIX
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h" // Include our new database header

#define TOTAL_QUESTIONS_IN_QUIZ 5

// --- Function Prototypes (UPDATED) ---
void print_html_header(const char* title);
void print_question_form(const Question* q, int question_number, int score, const char* q_ids_str, int user_id);
void print_result_page(int score, int user_id); // This will be updated
void get_form_data(char* data);
char* get_param_value(char* data, const char* param_name);
void urldecode(char* str);
int hex_to_int(char c);

// --- Main Program (Unchanged) ---
int main() {
    printf("Content-Type: text/html\n\n");
    sqlite3* db = db_connect();
    if (!db) {
        print_html_header("Error");
        printf("<h1>Database Connection Error</h1></body></html>");
        return 1;
    }
    char form_data[1024] = {0};
    get_form_data(form_data);
    char* user_id_str = get_param_value(form_data, "user_id");
    char* q_idx_str = get_param_value(form_data, "q_idx");
    char* score_str = get_param_value(form_data, "score");
    char* user_answer_str = get_param_value(form_data, "answer");
    char* q_ids_str = get_param_value(form_data, "q_ids");
    urldecode(q_ids_str); 
    int q_idx = q_idx_str ? atoi(q_idx_str) : 0;
    int score = score_str ? atoi(score_str) : 0;
    int user_id = user_id_str ? atoi(user_id_str) : 0;
    if (user_id == 0) {
        print_html_header("Error");
        printf("<div class='quiz-container'>");
        printf("<h1>Error: Not Logged In</h1>");
        printf("<p>You must be logged in to play the quiz.</p>");
        printf("<a style='color: #007bff; text-decoration: none; font-weight: bold;' href='/index.html'>Go to Login Page</a>");
        printf("</div></body></html>");
        sqlite3_close(db);
        free(user_id_str);
        free(q_idx_str);
        free(score_str);
        free(user_answer_str);
        free(q_ids_str);
        return 1;
    }
    int question_ids[TOTAL_QUESTIONS_IN_QUIZ] = {0};
    char new_q_ids_str[100] = "";
    if (q_ids_str) {
        char* id_token = strtok(q_ids_str, ",");
        for (int i = 0; i < TOTAL_QUESTIONS_IN_QUIZ; i++) {
            if (id_token) {
                question_ids[i] = atoi(id_token);
                id_token = strtok(NULL, ",");
            } else {
                question_ids[i] = 0;
            }
        }
        if (user_answer_str) {
            Question prev_question;
            if (q_idx > 0 && get_question_by_id(db, question_ids[q_idx - 1], &prev_question)) {
                if (atoi(user_answer_str) == prev_question.correct_option) {
                    score++;
                }
            }
        }
    } else {
        int ids_fetched = get_random_question_ids(db, TOTAL_QUESTIONS_IN_QUIZ, question_ids);
        if (ids_fetched < TOTAL_QUESTIONS_IN_QUIZ) {
            print_html_header("Error");
            printf("<div class='quiz-container'>");
            printf("<h1>Not enough questions in the database!</h1><p>Add more questions to the 'questions' table.</p>");
            printf("</div></body></html>");
            sqlite3_close(db);
            free(user_id_str);
            free(q_idx_str);
            free(score_str);
            free(user_answer_str);
            free(q_ids_str);
            return 1;
        }
    }
    for (int i = 0; i < TOTAL_QUESTIONS_IN_QUIZ; i++) {
        char temp[10];
        sprintf(temp, "%d,", question_ids[i]);
        strcat(new_q_ids_str, temp);
    }
    if(strlen(new_q_ids_str) > 0) {
        new_q_ids_str[strlen(new_q_ids_str) - 1] = '\0';
    }
    if (q_idx < TOTAL_QUESTIONS_IN_QUIZ) {
        Question current_question;
        if (get_question_by_id(db, question_ids[q_idx], &current_question)) {
            print_question_form(&current_question, q_idx + 1, score, new_q_ids_str, user_id);
        } else {
             print_html_header("Error");
             printf("<div class='quiz-container'>");
             printf("<h1>Error fetching question ID %d!</h1>", question_ids[q_idx]);
             printf("</div></body></html>");
        }
    } else {
        save_score(db, user_id, score);
        print_result_page(score, user_id);
    }
    sqlite3_close(db);
    free(user_id_str);
    free(q_idx_str);
    free(score_str);
    free(user_answer_str);
    free(q_ids_str);
    return 0;
}

// --- HTML Printing Functions (UPDATED) ---
void print_html_header(const char* title) {
    printf("<!DOCTYPE html><html lang='en'><head>");
    printf("<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>%s</title>", title);
    printf("<style>");
    printf("body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background-color: #f0f2f5; color: #333; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }\n");
    printf(".quiz-container { background: #e7e5e5ff; padding: 2rem; border-radius: 15px; box-shadow: 0 4px 20px rgba(0,0,0,0.1); width: 100%%; max-width: 600px; text-align: center; }\n");
    printf("h1 { color: #007bff; }\n");
    printf(".question { font-size: 1.2rem; margin-bottom: 1.5rem; }\n");
    
    /* --- *** THIS IS THE FIX *** --- */
    /* I added 'color: #007bff;' and 'font-weight: bold;' to make the text visible */
    printf(".options button { display: block; width: 100%%; padding: 1rem; margin: 0.5rem 0; border: 2px solid #ddd; border-radius: 10px; background-color: #ffffff; font-size: 1rem; cursor: pointer; transition: all 0.2s ease; color: #007bff; font-weight: bold; }\n");
    
    printf(".options button:hover { background-color: #b3c5d7ff; border-color: #007bff; }\n");
    printf(".score { font-size: 1.1rem; margin-top: 1rem; }\n");
    printf("button { display: block; width: 100%%; padding: 1rem; border: none; border-radius: 10px; background-color: #007bff; color: white; font-size: 1rem; cursor: pointer; transition: background-color 0.2s ease; }\n");
    printf("button:hover { background-color: #0056b3; }\n");
    printf(".link-button { display: block; text-decoration: none; width: 95%%; padding: 1rem; margin: 1rem auto 0 auto; border: none; border-radius: 10px; background-color: #6c757d; color: white; font-size: 1rem; cursor: pointer; transition: background-color 0.2s ease; }\n");
    printf(".link-button:hover { background-color: #5a6268; }\n");
    printf("</style>");
    printf("</head><body>\n");
}

// This function is unchanged
void print_question_form(const Question* q, int question_number, int score, const char* q_ids_str, int user_id) {
    print_html_header("C Quiz Challenge");
    printf("<div class='quiz-container'>\n");
    printf("<h1>Question %d/%d</h1>\n", question_number, TOTAL_QUESTIONS_IN_QUIZ);
    printf("<p class='question'>%s</p>\n", q->question_text);
    printf("<div class='options'>\n");
    for (int i = 0; i < 3; i++) {
        printf("<form action='quiz.cgi' method='POST' style='margin:0;'>\n");
        printf("<input type='hidden' name='q_idx' value='%d'>\n", question_number);
        printf("<input type='hidden' name='score' value='%d'>\n", score);
        printf("<input type='hidden' name='answer' value='%d'>\n", i);
        printf("<input type='hidden' name='q_ids' value='%s'>\n", q_ids_str);
        printf("<input type='hidden' name='user_id' value='%d'>\n", user_id);
        printf("<button type='submit'>%s</button>\n", q->options[i]);
        printf("</form>\n");
    }
    printf("</div>\n");
    printf("<p class='score'>Current Score: %d</p>\n", score);
    printf("</div>\n");
    printf("</body></html>\n");
}

/**
 * --- UPDATED: Added Leaderboard Link ---
 */
void print_result_page(int score, int user_id) {
    print_html_header("Quiz Results");
    printf("<div class='quiz-container'>\n");
    printf("<h1>Quiz Complete!</h1>\n");
    printf("<h2>Your Final Score: %d / %d</h2>\n", score, TOTAL_QUESTIONS_IN_QUIZ);
    printf("<p>Your score has been saved!</p>\n");
    
    // This form lets the user play again as themselves
    printf("<form action='quiz.cgi' method='POST' style='margin-top: 1rem;'>\n");
    printf("<input type='hidden' name='user_id' value='%d'>\n", user_id);
    printf("<button type='submit'>Play Again</button>\n");
    printf("</form>\n");
    
    // --- *** NEW LEADERBOARD LINK *** ---
    // This links to your new leaderboard.cgi program
    printf("<a href='/cgi-bin/leaderboard.cgi' class='link-button'>View Leaderboard</a>\n");
    
    printf("</div>\n");
    printf("</body></html>\n");
}


// --- Form Parsing Functions (Unchanged) ---
void get_form_data(char* data) {
    char* content_length_str = getenv("CONTENT_LENGTH");
    if (content_length_str) {
        int content_length = atoi(content_length_str);
        if (content_length > 0 && content_length < 1024) {
            fread(data, 1, content_length, stdin);
            data[content_length] = '\0';
        }
    }
}

int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

void urldecode(char* str) {
    if (!str) return;
    char* read = str;
    char* write = str;
    while (*read) {
        if (*read == '%' && *(read+1) && *(read+2)) {
            char hex_high = *(read+1);
            char hex_low = *(read+2);
            int val = (hex_to_int(hex_high) * 16) + hex_to_int(hex_low);
            *write = (char)val;
            read += 2; 
        } else if (*read == '+') {
             *write = ' '; 
        } else {
            *write = *read; 
        }
        read++;
        write++;
    }
    *write = '\0'; 
}

char* get_param_value(char* data, const char* param_name) {
    if (!data) return NULL;
    char* needle = malloc(strlen(param_name) + 2);
    if (!needle) return NULL; 
    sprintf(needle, "%s=", param_name);
    char* found = strstr(data, needle); 
    free(needle);
    if (!found) return NULL; 
    char* start = found + strlen(param_name) + 1;
    char* end = strchr(start, '&');
    int length;
    if (end) { length = end - start; } else { length = strlen(start); }
    if (length <= 0) return NULL; 
    char* value = malloc(length + 1);
    if (!value) return NULL; 
    strncpy(value, start, length);
    value[length] = '\0'; 
    return value; 
}