#ifndef L3_QUIZ_H
#define L3_QUIZ_H

#include "mbed.h"

#define MAX_PASSWORD_LEN 20
#define QUIZ_COUNT 3

extern const char* quiz_questions[QUIZ_COUNT];
extern const char* quiz_answers[QUIZ_COUNT];
extern int selected_quiz_index;
extern char selected_answer[MAX_PASSWORD_LEN];

void L3_quiz_showMenuToHost(Serial& pc);
bool L3_quiz_select(Serial& pc);

#endif
