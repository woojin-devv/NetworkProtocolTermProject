// #include "mbed.h"
// #include "string.h"
// #include "L2_FSMmain.h"
// #include "L3_FSMmain.h"

// //serial port interface
// Serial pc(USBTX, USBRX);

// //GLOBAL variables (DO NOT TOUCH!) ------------------------------------------

// //source/destination ID
// uint8_t input_thisId=1;
// uint8_t input_destId=0;

// //FSM operation implementation ------------------------------------------------
// int main(void){
//     int node_valid = 0;  // node 유효성 검사 결과 저장용
//     int quiz_index = -1; // 선택한 퀴즈 번호 저장

//     // 초기화
//     pc.printf("------------------ protocol stack starts! --------------------------\n");

//     // 노드 ID 입력
//     pc.printf(":: ID for this node : ");
//     pc.scanf("%d", &input_thisId);

//     // 유효한 node인지 판단 (예: 1 ~ 5만 유효하다고 가정)
//     if (input_thisId >= 1 && input_thisId <= 5) {
//         node_valid = 1;
//     }

//     // 목적지 ID 입력
//     pc.printf(":: ID for the destination : ");
//     pc.scanf("%d", &input_destId);
//     pc.getc();

//     pc.printf("endnode : %i, dest : %i\n", input_thisId, input_destId);

//     if (node_valid) {
//         // 정상 노드로 인식 → FSM 시작
//         pc.printf("정상 노드로 인식되었습니다. FSM을 실행합니다.\n");

//         // initialize lower layer stacks
//         L2_initFSM(input_thisId);
//         L3_initFSM(input_destId);

//         while (1) {
//             L2_FSMrun();
//             L3_FSMrun();
//         }
//     } else {
//         // 유효하지 않은 노드 → Host로 인식
//         pc.printf("Host로 인식되었습니다.\n");

//         // 퀴즈 선택 화면 출력
//         pc.printf("\n퀴즈를 선택하세요:\n");
//         pc.printf("1. apple\n");
//         pc.printf("2. straw\n");
//         pc.printf("3. candy\n");
//         pc.printf("선택 (1~3): ");
//         pc.scanf("%d", &quiz_index);
//         pc.getc();

//         if (quiz_index >= 1 && quiz_index <= 3) {
//             pc.printf("퀴즈 %d번 선택됨. 상태 전이: WAIT_ANSWER\n", quiz_index);
//             // 상태 저장 및 다음 동작 연결 가능
//             // 예: QUIZ_INDEX = quiz_index;
//             //     상태 전이 처리 등
//         } else {
//             pc.printf("잘못된 입력입니다. 프로그램을 종료합니다.\n");
//         }
//     }

//     return 0;
// }