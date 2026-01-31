/*
 * leaderboard.c
 * This is a new CGI program to display the high scores.
 */
#include <stdio.h>
#include <stdlib.h>
#include "database.h" // Our database functions

// --- Helper Prototype ---
void print_html_header(const char* title);

#define TOP_TEN 10 // Show top 10 scores

// --- Main Leaderboard Program ---
int main() {
    printf("Content-Type: text/html\n\n");

    sqlite3* db = db_connect();
    if (!db) {
        print_html_header("Error");
        printf("<h1>Database Connection Error</h1></body></html>");
        return 1;
    }

    // Create an array to hold the data
    LeaderboardEntry leaderboard[TOP_TEN];
    
    // Call our new database function
    int score_count = get_leaderboard_data(db, leaderboard, TOP_TEN);

    // --- Print the HTML Page ---
    print_html_header("Leaderboard");
    printf("<div class='quiz-container'>");
    printf("<h1>Top Scores</h1>");
    
    if (score_count == 0) {
        printf("<p>No scores yet! Be the first to play.</p>");
    } else {
        printf("<table class='leaderboard'>");
        printf("<tr><th>Rank</th><th>Username</th><th>High Score</th></tr>");
        
        // Loop through the results and print a table row
        for (int i = 0; i < score_count; i++) {
            printf("<tr>");
            printf("<td>%d</td>", i + 1);
            printf("<td>%s</td>", leaderboard[i].username);
            printf("<td>%d</td>", leaderboard[i].highest_score);
            printf("</tr>");
        }
        
        printf("</table>");
    }
    
    // Add a link to go back to the homepage
    printf("<a href='/index.html' class='link-button'>Back to Login</a>");
    printf("</div></body></html>");

    sqlite3_close(db);
    return 0;
}


// --- HTML Printing Helper ---
void print_html_header(const char* title) {
    printf("<!DOCTYPE html><html lang='en'><head>");
    printf("<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    printf("<title>%s</title>", title);
    printf("<style>");
    printf("body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background-color: #f0f2f5; color: #333; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }\n");
    printf(".quiz-container { background: #fff; padding: 2rem; border-radius: 15px; box-shadow: 0 4px 20px rgba(0,0,0,0.1); width: 100%%; max-width: 600px; text-align: center; }\n");
    printf("h1 { color: #007bff; }\n");
    // New styles for the table
    printf(".leaderboard { width: 100%%; border-collapse: collapse; margin-top: 1.5rem; margin-bottom: 1.5rem; }\n");
    printf(".leaderboard th, .leaderboard td { padding: 0.75rem; border: 1px solid #ddd; }\n");
    printf(".leaderboard th { background-color: #f0f2f5; color: #333; }\n");
    printf(".leaderboard td:first-child { font-weight: bold; color: #007bff; }\n");
    // New style for the "Back" link
    printf(".link-button { display: inline-block; text-decoration: none; margin-top: 1rem; padding: 1rem; border: none; border-radius: 10px; background-color: #6c757d; color: white; font-size: 1rem; cursor: pointer; transition: background-color 0.2s ease; }\n");
    printf(".link-button:hover { background-color: #5a6268; }\n");
    printf("</style>");
    printf("</head><body>\n");
}
