#include "L3_Quiz.h"
#include <string.h>

const char* quiz_questions[QUIZ_COUNT] = {
    "1. 왕이 넘어지면? [킹콩]",
    "2. 바나나가 웃으면? [바나나킥]",
    "3. 맥주가 죽으면서 남긴말은? [유언비어]"
};

const char* quiz_questions_for_user[QUIZ_COUNT] = {
    "1. 왕이 넘어지면?",
    "2. 바나나가 웃으면?",
    "3. 맥주가 죽으면서 남긴말은?"
};

const char* quiz_answers[QUIZ_COUNT] = {
    "킹콩",
    "바나나킥",
    "유언비어"
};

int selected_quiz_index = -1;
char selected_answer[MAX_PASSWORD_LEN] = {0};

void L3_quiz_showMenuToHost(Serial& pc) {
    pc.printf("\n======================== 퀴즈 목록 ========================\n");
    for (int i = 0; i < QUIZ_COUNT; i++) {
        pc.printf("%s\n", quiz_questions[i]);
    }
    pc.printf("==========================================================\n");
}

bool L3_quiz_select(Serial& pc) {
    int quiz_choice = 0;
    pc.printf(":: 퀴즈 번호를 선택하세요 (1 ~ %d): ", QUIZ_COUNT);
    pc.scanf("%d", &quiz_choice);
    pc.getc();

    if (quiz_choice < 1 || quiz_choice > QUIZ_COUNT) {
        pc.printf("[ERROR] 잘못된 번호입니다.\n");
        return false;
    }

    selected_quiz_index = quiz_choice - 1;
    strncpy(selected_answer, quiz_answers[selected_quiz_index], MAX_PASSWORD_LEN);
    pc.printf(":: 선택된 퀴즈: %s\n", quiz_questions[selected_quiz_index]);
    return true;
}
