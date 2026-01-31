/*
 * database.c
 * UPDATED for Leaderboard
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

// !!! This is your correct path !!!
#define DB_PATH "C:/Users/User/OneDrive/Desktop/Quiz/database/quiz.db"


/**
 * --- *** NEW HELPER FUNCTION *** ---
 * @brief A simple hash function for passwords.
 * NOT cryptographically secure, but good for a class project.
 */
unsigned long hash_password(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

/**
 * --- *** UPDATED FUNCTION *** ---
 * @brief Opens a connection to the SQLite database.
 * Now opens in Read-Write-Create mode.
 */
sqlite3* db_connect() {
    sqlite3* db;
    // UPDATED: Changed from SQLITE_OPEN_READONLY
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if (sqlite3_open_v2(DB_PATH, &db, flags, NULL) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    return db;
}

// --- Quiz Functions (Unchanged) ---
int get_question_by_id(sqlite3* db, int id, Question* question_out) {
    sqlite3_stmt* stmt;
    char sql[200];
    sprintf(sql, "SELECT question_text, option_a, option_b, option_c, correct_option_idx FROM questions WHERE question_id = ?;");
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB Error: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, id);
    int result = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        question_out->id = id;
        strncpy(question_out->question_text, (const char*)sqlite3_column_text(stmt, 0), 255);
        question_out->question_text[255] = '\0';
        strncpy(question_out->options[0], (const char*)sqlite3_column_text(stmt, 1), 99);
        question_out->options[0][99] = '\0';
        strncpy(question_out->options[1], (const char*)sqlite3_column_text(stmt, 2), 99);
        question_out->options[1][99] = '\0';
        strncpy(question_out->options[2], (const char*)sqlite3_column_text(stmt, 3), 99);
        question_out->options[2][99] = '\0';
        question_out->correct_option = sqlite3_column_int(stmt, 4);
        result = 1;
    }
    sqlite3_finalize(stmt);
    return result;
}

int get_random_question_ids(sqlite3* db, int count, int* ids_out) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT question_id FROM questions ORDER BY RANDOM() LIMIT ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB Error: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, count);
    int ids_fetched = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && ids_fetched < count) {
        ids_out[ids_fetched] = sqlite3_column_int(stmt, 0);
        ids_fetched++;
    }
    sqlite3_finalize(stmt);
    return ids_fetched;
}

// --- *** USER/SCORE FUNCTIONS *** ---

/**
 * @brief Registers a new user. Hashes the password.
 */
int register_user(sqlite3* db, const char* username, const char* password) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB Error: %s\n", sqlite3_errmsg(db));
        return 0; // SQL error
    }

    unsigned long hashed_pass = hash_password(password);
    char hashed_pass_str[21]; // 64-bit unsigned long can be up to 20 digits
    sprintf(hashed_pass_str, "%lu", hashed_pass);

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_pass_str, -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result == SQLITE_DONE) {
        return 1; // Success
    } else {
        return 0; // Fail (likely username already exists)
    }
}

/**
 * @brief Logs in a user. Hashes password and compares.
 */
int login_user(sqlite3* db, const char* username, const char* password) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT user_id FROM users WHERE username = ? AND password = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB Error: %s\n", sqlite3_errmsg(db));
        return 0; // SQL error
    }

    unsigned long hashed_pass = hash_password(password);
    char hashed_pass_str[21];
    sprintf(hashed_pass_str, "%lu", hashed_pass);

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_pass_str, -1, SQLITE_STATIC);

    int user_id = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0); // Found user, get ID
    }
    
    sqlite3_finalize(stmt);
    return user_id; // Returns 0 if no match
}

/**
 * @brief Saves a user's score.
 */
void save_score(sqlite3* db, int user_id, int score) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO scores (user_id, score) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB Error: %s\n", sqlite3_errmsg(db));
        return; // SQL error
    }
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, score);
    sqlite3_step(stmt); // Run the insert
    sqlite3_finalize(stmt);
}

/*
 * --- *** NEW LEADERBOARD FUNCTION *** ---
 * This is the new function you are adding.
 */
int get_leaderboard_data(sqlite3* db, LeaderboardEntry* entries_out, int max_entries) {
    sqlite3_stmt* stmt;
    
    // This is your exact SQL query!
    const char* sql = "SELECT users.username, MAX(scores.score) AS highest_score "
                      "FROM scores "
                      "JOIN users ON scores.user_id = users.user_id "
                      "GROUP BY users.username "
                      "ORDER BY highest_score DESC "
                      "LIMIT ?;"; // We limit it to max_entries (e.g., 10)

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB Error: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, max_entries); // Bind the LIMIT value

    int entries_fetched = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && entries_fetched < max_entries) {
        // Copy the username
        strncpy(entries_out[entries_fetched].username, 
                (const char*)sqlite3_column_text(stmt, 0), 
                99);
        entries_out[entries_fetched].username[99] = '\0'; // Ensure null-termination

        // Get the high score
        entries_out[entries_fetched].highest_score = sqlite3_column_int(stmt, 1);
        
        entries_fetched++;
    }
    
    sqlite3_finalize(stmt);
    return entries_fetched;
}

