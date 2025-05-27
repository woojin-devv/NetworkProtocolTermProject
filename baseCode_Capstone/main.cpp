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


 // Host 역할
     if (is_host_node(input_destId)) {
        l3_state = WAIT_ANSWER;
    } else {
        l3_state = WAIT_QUIZ;
    }

    // User 역할
    else {
        pc.printf(":: role: USER\n");
        L3_quiz_showSelectedToUser(pc);         // Host가 선택한 퀴즈 보여주기
        L3_quiz_receiveAnswerFromUser(pc);      // User의 답변 받고 채점
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