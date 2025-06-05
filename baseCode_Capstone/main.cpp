#include "mbed.h"
#include "string.h"
#include "L2_FSMmain.h"
#include "L3_FSMmain.h"

#include "L3_role.h"
#include "L3_state.h"

Serial pc(USBTX, USBRX);

uint16_t input_thisId = 1;
uint16_t input_destId = 0;

//scanf() 이후에만 attach 

int main(void) {
    pc.printf("------------------ protocol stack starts! --------------------------\n");

    // ID 입력 (attach 전에 반드시 수행)
    pc.printf(":: ID for this node : ");
    pc.scanf("%hu", &input_thisId);

    pc.printf(":: ID for the destination : ");
    pc.scanf("%hu", &input_destId);

    pc.getc();  // '\n' 제거

    if (input_thisId == input_destId) {
        pc.printf("[ERROR] Node ID and Destination ID cannot be the same.\n");
        return -1;
    }

    register_node(input_thisId);

    pc.printf("endnode : %i, dest : %i\n", input_thisId, input_destId);

    //디버깅용 - 등록된 아이디 확인
    // pc.printf(":: Registered ID List:\n");
    // for (int i = 0; i < MAX_NODE_COUNT; i++) {
    //     pc.printf("  valid_node_ids[%d] = %d\n", i, valid_node_ids[i]);
    // }

    // 역할 기반 초기 상태 설정
    if (is_host_node(input_destId)) {
        l3_state = WAIT_ANSWER; // Host
    } else {
        l3_state = WAIT_QUIZ;   // User
    }

    // attach 이후에 FSM 시작
    L2_initFSM(input_thisId);
    L3_initFSM(input_thisId, input_destId);

    // IDLE 유도
    L3_FSMrun();

    while (1) {
        L2_FSMrun();
        L3_FSMrun();
    }
}