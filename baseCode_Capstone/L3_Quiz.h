#ifndef L3_QUIZ_H
#define L3_QUIZ_H

#include "mbed.h"

#define MAX_PASSWORD_LEN 20
#define QUIZ_TOTAL_COUNT 5  

// Arrays with dummy at index 0
extern const char* quiz_questions[QUIZ_TOTAL_COUNT];
extern const char* quiz_questions_for_user[QUIZ_TOTAL_COUNT];
extern const char* quiz_answers[QUIZ_TOTAL_COUNT];

// Quiz state
extern int selected_quiz_index;
extern char selected_answer[MAX_PASSWORD_LEN];

// Host functions
void L3_quiz_showMenuToHost(Serial& pc);

// User functions
void L3_quiz_receiveAnswerFromUser(Serial& pc);
bool L3_quiz_checkAnswer(const char* user_input);
bool L3_quiz_isAnswerCorrect(const char* userAnswer); 

typedef enum {
    QUIZ_SELECTED,
    QUIZ_JOIN_AS_USER,
    QUIZ_SELECTION_FAILED
} QuizSelectResult;

QuizSelectResult L3_quiz_select(Serial& pc);


#endif