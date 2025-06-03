#include "mbed.h"
#include "string.h"
#include "L2_FSMmain.h"
#include "L3_FSMmain.h"

#include "L3_role.h"
#include "L3_Quiz.h"
#include "L3_state.h"


//serial port interface
Serial pc(USBTX, USBRX);

//GLOBAL variables (DO NOT TOUCH!) ------------------------------------------

//source/destination ID
uint16_t input_thisId=1;
uint16_t input_destId=0;

//FSM operation implementation ------------------------------------------------
int main(void){


    //initialization
    pc.printf("------------------ protocol stack starts! --------------------------\n");


        //source & destination ID setting
    pc.printf(":: ID for this node : ");
    pc.scanf("%hu", &input_thisId);

    pc.printf(":: ID for the destination : ");
    pc.scanf("%hu", &input_destId);
    pc.getc();

    if (input_thisId == input_destId) {
        pc.printf("[ERROR] Node ID and Destination ID cannot be the same.\n");
        return -1;
    }

    register_node(input_thisId);  // 유효 노드 등록, Host의 destID는 존재하지 않음

    pc.printf("endnode : %i, dest : %i\n", input_thisId, input_destId);
    pc.printf(":: Registered ID List:\n");

    for (int i = 0; i < MAX_NODE_COUNT; i++) {
        pc.printf("  valid_node_ids[%d] = %d\n", i, valid_node_ids[i]);
    }

     if (is_host_node(input_destId)) {  // Host -> Quiz에 따라 정답을 기다림
        l3_state = WAIT_ANSWER;
    } else {
        l3_state = WAIT_QUIZ; //User -> Quiz 정답을 기다림림
    }

    // 역할 분기 처리
    switch (l3_state) {
        case WAIT_ANSWER: // Host
            pc.printf(":: role: HOST\n");
            L3_quiz_showMenuToHost(pc);
            if (!L3_quiz_select(pc)) {
                return -1;
            }
             // 퀴즈 선택 완료 후 상태 전이
            l3_state = CHAT_READY;
            break;

        case WAIT_QUIZ: // User
            pc.printf(":: role: USER\n");
            L3_quiz_showSelectedToUser(pc);
            L3_quiz_receiveAnswerFromUser(pc);
            break;

        case CHAT_READY: // Host & User 공통 진입
            pc.printf(":: state: CHAT_READY\n");
            if (is_host_node(input_destId)) {
                pc.printf("-> [CHAT MODE] Host is ready to chat\n");
                // Host 전용 채팅 준비 로직 (예: 입력 대기 등)
            } else {
                pc.printf("-> [CHAT MODE] User is ready to chat\n");
                // User 전용 채팅 준비 로직
            }
            break;
            
        case TERMINATE:
            pc.printf(":: Terminating...\n");
            break;


        default:
            pc.printf("[ERROR] Invalid state for role initialization: %s\n", L3_stateToStr(l3_state));
            return -1;
    }

    //initialize lower layer stacks
    L2_initFSM(input_thisId);
    L3_initFSM(input_destId);
    
    while(1)
    {
        L2_FSMrun();
        L3_FSMrun();
    }
}