#include "L3_Quiz.h"
#include <string.h>
#include "mbed.h" 


// L3_Quiz.cpp 상단에 추가
extern void L3service_processInputWord(void);

//host에게 보여줄 QUIZ 목록록
const char* quiz_questions[QUIZ_TOTAL_COUNT] = {
    "1. What's host's favorite fruit? [banana]",
    "2. How old is the Host? [25]",
    "3. Is Host a student in Sookmyung women's university? [yes]"
};

//USER에게 보여줄 QUIZ 목록록
const char* quiz_questions_for_user[QUIZ_TOTAL_COUNT] = {
    "What's host's favorite fruit?",
    "How old is the Host?",
    "Is Host a student in Sookmyung women's university?"
};

//퀴즈 정답 
const char* quiz_answers[QUIZ_TOTAL_COUNT] = {
    "banana",
    "25",
    "yes"
};

int selected_quiz_index = -1;
char selected_answer[MAX_PASSWORD_LEN] = {0};

void L3_quiz_showMenuToHost(Serial& pc) {
    pc.printf("\n======================== Quiz List ========================\n");
    for (int i = 0; i < QUIZ_TOTAL_COUNT; i++) {
        pc.printf("%s\n", quiz_questions[i]);
    }
    pc.printf("===========================================================\n");
}

void L3_quiz_showSelectedToUser(Serial& pc) {
    if (selected_quiz_index < 0 || selected_quiz_index >= QUIZ_TOTAL_COUNT) {
        pc.printf("[Error] No valid quiz has been selected.\n");
        return;
    }

    pc.printf("\n=========== Quiz ===========\n");
    pc.printf("%s\n", quiz_questions_for_user[selected_quiz_index]);
    pc.printf("============================\n");
}

void L3_quiz_receiveAnswerFromUser(Serial& pc) {
    char user_input[MAX_PASSWORD_LEN] = {0};

    pc.printf(":: Please enter your answer: ");
    pc.scanf("%19s", user_input);  // 공백 없는 문자열 기준
    pc.getc();

    if (L3_quiz_checkAnswer(user_input)) {
        pc.printf(":: Correct answer! \n");
    } else {
        pc.printf(":: Wrong answer The correct answer was: %s\n", selected_answer);
    }
}


bool L3_quiz_select(Serial& pc) {
    char inputBuffer[16] = {0};

    // 인터럽트 detach
    pc.attach(NULL, Serial::RxIrq);

    pc.printf(":: Please select a quiz number (1 ~ %d) or enter 'U' to join as USER: ", QUIZ_TOTAL_COUNT);

    size_t idx = 0;  // idx를 size_t로
    while (1) {
        char c = pc.getc();
        if (c == '\r' || c == '\n') {
            inputBuffer[idx] = '\0';
            break;
        }
        if (idx < sizeof(inputBuffer) - 1) {
            inputBuffer[idx++] = c;
            pc.putc(c);
        }
        // 기존 코드 내부 while 루프 끝난 후 추가
        if (strcasecmp(inputBuffer, "u") == 0) {
            pc.printf(":: Joining as USER. Sending 'join' message...\n");
            strcpy(selected_answer, ""); // 초기화
            selected_quiz_index = -1;    // 초기화
            return false;  // 혹은 특정 플래그 리턴하도록 설계
        }
    }


    pc.printf("\n:: You entered: %s\n", inputBuffer);

    //다시 attach
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    int quiz_choice = atoi(inputBuffer);
    if (quiz_choice < 1 || quiz_choice > QUIZ_TOTAL_COUNT) {
        pc.printf("[Error] Invalid quiz number selected.\n");
        return false;
    }

    selected_quiz_index = quiz_choice - 1;
    strncpy(selected_answer, quiz_answers[selected_quiz_index], MAX_PASSWORD_LEN);
    pc.printf(":: Selected quiz: %s\n", quiz_questions[selected_quiz_index]);
    return true;
}



bool L3_quiz_checkAnswer(const char* user_input) {
    if (selected_quiz_index < 0 || strlen(selected_answer) == 0) return false;
    return strcmp(user_input, selected_answer) == 0;
}



bool L3_quiz_isAnswerCorrect(const char* userInput) {
    if (userInput == NULL || strlen(userInput) == 0) {
        return false;
    }

    // 선택된 퀴즈의 정답 가져오기
    const char* correctAnswer = quiz_answers[selected_quiz_index];
    return strcmp(userInput, correctAnswer) == 0;
}