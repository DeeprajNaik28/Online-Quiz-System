/*
 * auth.c
 * This is a new CGI program to handle user login and registration.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h" // Our database functions

// --- Helper Prototypes (copied from quiz.c) ---
void print_html_header(const char* title);
void get_form_data(char* data);
char* get_param_value(char* data, const char* param_name);
void urldecode(char* str);
int hex_to_int(char c);

// --- Main Auth Program ---
int main() {
    printf("Content-Type: text/html\n\n");

    sqlite3* db = db_connect();
    if (!db) {
        print_html_header("Error");
        printf("<h1>Database Connection Error</h1></body></html>");
        return 1;
    }

    // Get all form data from index.html
    char form_data[1024] = {0};
    get_form_data(form_data);

    // Get the parameters
    char* action = get_param_value(form_data, "action");
    char* username = get_param_value(form_data, "username");
    char* password = get_param_value(form_data, "password");

    // URL-decode them (usernames might have + or %20)
    urldecode(username);
    urldecode(password);

    if (action && username && password) {
        if (strcmp(action, "register") == 0) {
            // --- Handle Registration ---
            if (register_user(db, username, password)) {
                print_html_header("Registration Successful");
                printf("<div class='quiz-container'>");
                printf("<h1>Success!</h1>");
                printf("<p>Your account '%s' has been created.</p>", username);
                printf("<a href='/index.html'>Click here to login</a>");
                printf("</div></body></html>");
            } else {
                print_html_header("Registration Failed");
                printf("<div class='quiz-container'>");
                printf("<h1>Error</h1>");
                printf("<p>Registration failed. That username might already be taken.</p>");
                printf("<a href='/index.html'>Try again</a>");
                printf("</div></body></html>");
            }
        } else if (strcmp(action, "login") == 0) {
            // --- Handle Login ---
            int user_id = login_user(db, username, password);
            if (user_id > 0) {
                // --- LOGIN SUCCESS ---
                print_html_header("Login Successful");
                printf("<div class='quiz-container'>");
                printf("<h1>Welcome, %s!</h1>", username);
                printf("<p>You are logged in. Ready to play?</p>");
                // This form passes the user_id to the quiz
                printf("<form action='quiz.cgi' method='POST'>");
                printf("<input type='hidden' name='user_id' value='%d'>", user_id);
                printf("<button type='submit'>Start Quiz</button>");
                printf("</form>");
                printf("</div></body></html>");
            } else {
                // --- LOGIN FAILED ---
                print_html_header("Login Failed");
                printf("<div class='quiz-container'>");
                printf("<h1>Error</h1>");
                printf("<p>Invalid username or password.</p>");
                printf("<a href='/index.html'>Try again</a>");
                printf("</div></body></html>");
            }
        }
    } else {
        print_html_header("Error");
        printf("<h1>Missing Information</h1>");
        printf("<p>Please fill out all fields.</p>");
        printf("<a href='/index.html'>Try again</a>");
        printf("</div></body></html>");
    }

    // Clean up
    sqlite3_close(db);
    free(action);
    free(username);
    free(password);
    return 0;
}


// --- Helper Functions (Copied from quiz.c) ---
void print_html_header(const char* title) {
    printf("<!DOCTYPE html><html lang='en'><head>");
    printf("<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>%s</title>", title);
    printf("<style>");
    printf("body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background-color: #f0f2f5; color: #333; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }\n");
    printf(".quiz-container { background: #fff; padding: 2rem; border-radius: 15px; box-shadow: 0 4px 20px rgba(0,0,0,0.1); width: 100%%; max-width: 600px; text-align: center; }\n");
    printf("h1 { color: #007bff; }\n");
    printf("a { color: #007bff; text-decoration: none; font-weight: bold; }\n");
    printf("button { display: block; width: 100%%; padding: 1rem; border: none; border-radius: 10px; background-color: #007bff; color: white; font-size: 1rem; cursor: pointer; transition: background-color 0.2s ease; }\n");
    printf("button:hover { background-color: #0056b3; }\n");
    printf("</style>");
    printf("</head><body>\n");
}

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