#include "mbed.h"
#include "string.h"
#include "L2_FSMmain.h"
#include "L3_FSMmain.h"



//serial port interface
Serial pc(USBTX, USBRX);

//GLOBAL variables (DO NOT TOUCH!) ------------------------------------------

//source/destination ID
uint8_t input_thisId=1;
uint8_t input_destId=0;

//FSM operation implementation ------------------------------------------------
#include <string>
#include <cstring>

// 퀴즈 정답 DB
const char* QUIZ_QUESTIONS[] = {
    "", // index 0 dummy
    "What's host's favorite fruit?",
    "How old is the Host?",
    "Is Host a student in Sookmyung women's university?"
};

const char* QUIZ_ANSWERS[] = {
    "",     // index 0 dummy
    "banana",
    "25",
    "yes"
};



int QUIZ_INDEX = 0;
enum STATE { IDLE, WAIT_ANSWER, CHAT_READY, TERMINATE };
STATE current_state = IDLE;

int main(void) {
    int node_valid = 0;
    char user_answer[100];

    pc.printf("------------------ protocol stack starts! --------------------------\n");

    // 노드 입력
    pc.printf(":: ID for this node : ");
    pc.scanf("%d", &input_thisId);

    pc.printf(":: ID for the destination : ");
    pc.scanf("%d", &input_destId);
    pc.getc();

    pc.printf("endnode : %d, dest : %d\n", input_thisId, input_destId);

    // 1-5만 유효하다고 가정
    if (input_destId >= 1 && input_destId <= 5) {
        node_valid = 1;
    }

    if (node_valid) {
        // destination node가 1-5일 경우

        pc.printf("answer: ");
        pc.scanf("%s", user_answer);
        pc.getc();


        L2_initFSM(input_thisId);
        L3_initFSM(input_destId);

        while (1) {
            L2_FSMrun();
            L3_FSMrun();
        }
    } else {
        // destination node 가 지정된 숫자가 아닐 경우
        // Host 흐름
        current_state = WAIT_ANSWER;

        // 퀴즈 선택
        pc.printf("\nRecognized as a Host.\n ------- Select the Quiz Number ------- \n");
        pc.printf("1. What's host's favorite fruit ?\n2. How old is the Host ?\n3. Is Host a student in Sookmyung women's university ? \nChoose the Quiz Number (1~3): ");
        pc.scanf("%d", &QUIZ_INDEX);
        pc.getc();




        if (QUIZ_INDEX >= 1 && QUIZ_INDEX <= 3) {
    pc.printf("You selected: %s\n", QUIZ_QUESTIONS[QUIZ_INDEX]);
} else {
    pc.printf("Invalid quiz number selected.\n");
}

        // 상태 전이: WAIT_ANSWER
        pc.printf("Selected number: %d. Change the state: WAIT_ANSWER\n", QUIZ_INDEX);

        // 정답 비교
        if (strcmp(user_answer, QUIZ_ANSWERS[QUIZ_INDEX]) == 0) {
            // 정답
            pc.printf("Correct! ACK(Answer_Received) Send\n");
            current_state = CHAT_READY;
            pc.printf("State change: CHAT_READY\n");

            // 이후 채팅 준비 상태 로직 연결 가능
        } else {
            // 오답
            pc.printf("Wrong. State change: TERMINATE\n");
            current_state = TERMINATE;
        }
    }

    return 0;
}
