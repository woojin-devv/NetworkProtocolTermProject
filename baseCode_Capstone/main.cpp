#include "mbed.h"
#include "string.h"
#include "L2_FSMmain.h"
#include "L3_FSMmain.h"

#include "L3_role.h"
#include "L3_Quiz.h"

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
    pc.printf(":: 등록된 유효 노드 ID 목록:\n");

    for (int i = 0; i < MAX_NODE_COUNT; i++) {
        pc.printf("  valid_node_ids[%d] = %d\n", i, valid_node_ids[i]);
    }


    if (is_host_node(input_destId)) {
        pc.printf(":: 역할: HOST 입니다.\n");
        L3_quiz_showMenuToHost(pc);
    if (!L3_quiz_select(pc)) {
        return -1;
    }
        
    } else {
        pc.printf(":: 역할: USER 입니다.\n");
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