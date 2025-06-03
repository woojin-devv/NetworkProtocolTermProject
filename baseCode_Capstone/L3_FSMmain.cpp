#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"

#include "L3_state.h" //state 


// //FSM state -------------------------------------------------
// #define L3STATE_IDLE 0


// //state variables
// static uint8_t main_state = L3STATE_IDLE; //protocol state
// static uint8_t prev_state = main_state;
L3_State l3_state = IDLE;   
static L3_State prev_state = IDLE;

//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t sdu[1030];

//serial port interface
static Serial pc(USBTX, USBRX);
static uint8_t myDestId;

//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
        }
    }
}



void L3_initFSM(uint8_t destId)
{

    myDestId = destId;
    //initialize service layer
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{   
    if (prev_state != l3_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %s to %s\n",
             L3_stateToStr(prev_state), L3_stateToStr(l3_state));
        prev_state = l3_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (l3_state)
    {
        case IDLE: //IDLE state description
            
            if (L3_event_checkEventFlag(L3_event_msgRcvd)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                
                // 버퍼에 복사 후 널 종료 추가
                char printBuf[1030] = {0};
                memcpy(printBuf, dataPtr, size);
                printBuf[size] = '\0'; // 문자열 종료

                debug("\n -------------------------------------------------\n");
                debug("RCVD MSG : %s (length:%i)\n", printBuf, size);
                debug(" -------------------------------------------------\n");
                           
                if (strncmp((char*)dataPtr, "quit", 4) == 0) {
                    pc.printf(":: 'quit' received from peer. Terminating chat...\n");
                    l3_state = TERMINATE;
                    L3_event_clearEventFlag(L3_event_msgRcvd);
                    return;  // TERMINATE 상태로 넘어가기
                }
                pc.printf("Give a word to send : ");
                
                L3_event_clearEventFlag(L3_event_msgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_dataToSend)) //if data needs to be sent (keyboard input)
            {
                 if (strcmp((char*)originalWord, "quit") == 0) {
                    l3_state = TERMINATE;
                    pc.printf(":: Chat terminated by user input.\n");

                    // 초기화
                    wordLen = 0;
                    memset(originalWord, 0, sizeof(originalWord));
                    L3_event_clearEventFlag(L3_event_dataToSend);
                    break;
                }
                //msg header setting
                strcpy((char*)sdu, (char*)originalWord);
                debug("[L3] msg length : %i\n", wordLen);
                L3_LLI_dataReqFunc(sdu, wordLen, myDestId);
                
                //송신 메시지 출력
                pc.printf("[You] %s\n", sdu);


                debug_if(DBGMSG_L3, "[L3] sending msg....\n");
                wordLen = 0;

                pc.printf("Give a word to send : ");

                L3_event_clearEventFlag(L3_event_dataToSend);
            }
            break;

        case CHAT_READY:
            pc.printf(":: [L3] Entering CHAT_READY...\n");
            l3_state = IDLE; // 메시지 송수신 처리를 위해 IDLE로 전이
            break;

        case TERMINATE:
            pc.printf(":: [L3] Entering TERMINATE. Chat session ended.\n");

             // 이벤트 플래그 전부 초기화
            L3_event_clearAllEventFlag();

            // 인터럽트 제거 (더 이상 사용자 입력 X)
            pc.attach(NULL);
            while (1);  // 무한 루프 대기
            break;

        default :
            break;
    }
}