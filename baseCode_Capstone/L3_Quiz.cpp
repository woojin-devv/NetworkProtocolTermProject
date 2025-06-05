#include "L3_Quiz.h"
#include <string.h>
#include "mbed.h" 


// L3_Quiz.cpp 상단에 추가
extern void L3service_processInputWord(void);

//host에게 보여줄 QUIZ 목록록
const char* quiz_questions[QUIZ_TOTAL_COUNT] = {
    "1. What's host's favorite fruit? (Hint: It's yellow and monkeys love it!)",
    "2. How old is the Host? (Hint: It's a quarter of a century.)",
    "3. Is Host a student in Sookmyung women's university? (Hint: She's currently enrolled in a women's university in Seoul.)",
    "4. What is 7 * 7?",
    "5. What’s the host’s favorite season? (Hint: It’s the season of cherry blossoms and new beginnings.)  "
};

//퀴즈 정답 
const char* quiz_answers[QUIZ_TOTAL_COUNT] = {
    "banana",
    "25",
    "yes"
    "49"
    "spring"
};

int selected_quiz_index = -1;
char selected_answer[MAX_PASSWORD_LEN] = {0};

void L3_quiz_showMenuToHost(Serial& pc) {
    pc.printf("\n======================== Quiz List ========================\n\n");
    for (int i = 0; i < QUIZ_TOTAL_COUNT; i++) {
        pc.printf("%s\n\n", quiz_questions[i]);
    }
    pc.printf("\n===========================================================\n\n");
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

    pc.printf("[Please enter your answer]: ");
    pc.scanf("%19s", user_input);  // 공백 없는 문자열 기준
    pc.getc();

    if (L3_quiz_checkAnswer(user_input)) {
        pc.printf("[Correct answer!] \n");
    } else {
        pc.printf("[Wrong answer The correct answer was]: %s\n", selected_answer);
    }
}

QuizSelectResult L3_quiz_select(Serial& pc) {
    char inputBuffer[16] = {0};

    pc.attach(NULL, Serial::RxIrq);

    pc.printf("[Please select a quiz number (1 ~ %d) or enter 'U' to join as USER] : ", QUIZ_TOTAL_COUNT);

    size_t idx = 0;
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
    }

    pc.printf("\n[You entered]: %s\n", inputBuffer);
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    
    if (strcasecmp(inputBuffer, "u") == 0) {
        pc.printf("[Joining as USER. Sending 'join' message...]\n");
        return QUIZ_JOIN_AS_USER;
    }

    int quiz_choice = atoi(inputBuffer);
    if (quiz_choice < 1 || quiz_choice > QUIZ_TOTAL_COUNT) {
        pc.printf("[Error] Invalid quiz number selected.\n");
        return QUIZ_SELECTION_FAILED;
    }

    selected_quiz_index = quiz_choice - 1;
    strncpy(selected_answer, quiz_answers[selected_quiz_index], MAX_PASSWORD_LEN);
    pc.printf("[Selected quiz] : %s\n\n", quiz_questions[selected_quiz_index]);
    return QUIZ_SELECTED;
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