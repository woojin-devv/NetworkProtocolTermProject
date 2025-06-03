#ifndef L3_QUIZ_H
#define L3_QUIZ_H

#include "mbed.h"

#define MAX_PASSWORD_LEN 20
#define QUIZ_TOTAL_COUNT 4  // index 0 is dummy

// Arrays with dummy at index 0
extern const char* quiz_questions[QUIZ_TOTAL_COUNT];
extern const char* quiz_questions_for_user[QUIZ_TOTAL_COUNT];
extern const char* quiz_answers[QUIZ_TOTAL_COUNT];

// Quiz state
extern int selected_quiz_index;
extern char selected_answer[MAX_PASSWORD_LEN];

// Host functions
void L3_quiz_showMenuToHost(Serial& pc);
bool L3_quiz_select(Serial& pc);

// User functions
void L3_quiz_showSelectedToUser(Serial& pc);
void L3_quiz_receiveAnswerFromUser(Serial& pc);
bool L3_quiz_checkAnswer(const char* user_input);

#endif
