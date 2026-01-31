/*
 * database.h
 * UPDATED for Leaderboard
 */
#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

// Struct for holding a single question
typedef struct {
    int id;
    char question_text[256];
    char options[3][100];
    int correct_option;
} Question;

// --- *** NEW LEADERBOARD STRUCT *** ---
typedef struct {
    char username[100];
    int highest_score;
} LeaderboardEntry;


// --- Quiz Functions ---
sqlite3* db_connect();
int get_question_by_id(sqlite3* db, int id, Question* question_out);
int get_random_question_ids(sqlite3* db, int count, int* ids_out);

// --- User/Score Functions ---
int register_user(sqlite3* db, const char* username, const char* password);
int login_user(sqlite3* db, const char* username, const char* password);
void save_score(sqlite3* db, int user_id, int score);

// --- *** NEW LEADERBOARD FUNCTION *** ---
/**
 * @brief Gets the top 10 high scores from the database.
 * @return The number of scores fetched.
 */
int get_leaderboard_data(sqlite3* db, LeaderboardEntry* entries_out, int max_entries);

#endif // DATABASE_H

