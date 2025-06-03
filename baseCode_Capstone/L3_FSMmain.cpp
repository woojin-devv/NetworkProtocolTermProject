#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"
#include "L3_Quiz.h"
#include "L3_2min_timer.h"
#include "L3_state.h"

L3_State l3_state = IDLE;
static L3_State prev_state = IDLE;

static uint8_t originalWord[1030];
static uint8_t wordLen = 0;
static uint8_t sdu[1030];

static Serial pc(USBTX, USBRX);
static uint8_t myDestId;

static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend)) {
        if (c == '\n' || c == '\r') {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3, "word is ready! ::: %s\n", originalWord);
        } else {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE - 1) {
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
    pc.attach(&L3service_processInputWord, Serial::RxIrq);
    pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{
    if (prev_state != l3_state) {
        debug_if(DBGMSG_L3, "[L3] State transition from %s to %s\n",
                 L3_stateToStr(prev_state), L3_stateToStr(l3_state));
        prev_state = l3_state;
    }

    switch (l3_state) {
        case WAIT_ANSWER: {
            pc.printf(":: role: HOST\n");
            L3_quiz_showMenuToHost(pc);
            if (!L3_quiz_select(pc)) {
                pc.printf("[ERROR] Quiz selection failed. Terminating.\n");
                l3_state = TERMINATE;
                break;
            }
            l3_state = CHAT_READY;
            break;
        }
        case WAIT_QUIZ: {
            pc.printf(":: role: USER\n");
            L3_quiz_showSelectedToUser(pc);

            const int maxAttempts = 3;
            Timer quizTimer;
            quizTimer.start();

            for (int attempt = 1; attempt <= maxAttempts; attempt++) {
                float elapsed = quizTimer.read();
                float remaining = 120.0f - elapsed;

                if (remaining <= 0.0f) {
                    pc.printf("[TIMEOUT] 2 minutes are over. Terminating.\n");
                    l3_state = TERMINATE;
                    break;
                }

                pc.printf("\n[Attempt %d/%d] Time left: %d seconds\n", attempt, maxAttempts, (int)remaining);
                pc.printf("Answer: ");

                char answerBuffer[MAX_PASSWORD_LEN] = {0};
                bool gotInput = getInputWithinTime(pc, answerBuffer, sizeof(answerBuffer), remaining);

                if (!gotInput) {
                    pc.printf("Incorrect answer. (No input detected)\n");
                    continue;
                }

                if (L3_quiz_isAnswerCorrect(answerBuffer)) {
                    pc.printf("Correct! Moving to CHAT_READY state.\n");
                    l3_state = CHAT_READY;
                    break;
                } else {
                    pc.printf("Incorrect answer.\n");
                }

                if (attempt == maxAttempts) {
                    pc.printf("You've used all attempts. Terminating session.\n");
                    l3_state = TERMINATE;
                }
            }
            break;
        }
        case IDLE: {
            if (L3_event_checkEventFlag(L3_event_msgRcvd)) {
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                char printBuf[1030] = {0};
                memcpy(printBuf, dataPtr, size);
                printBuf[size] = '\0';

                debug("\n -------------------------------------------------\n");
                debug("RCVD MSG : %s (length:%i)\n", printBuf, size);
                debug(" -------------------------------------------------\n");

                if (strncmp((char*)dataPtr, "quit", 4) == 0) {
                    pc.printf(":: 'quit' received from peer. Terminating chat...\n");
                    l3_state = TERMINATE;
                    L3_event_clearEventFlag(L3_event_msgRcvd);
                    return;
                }
                pc.printf("[Peer] %s\n", printBuf);
                pc.printf("Give a word to send : ");

                L3_event_clearEventFlag(L3_event_msgRcvd);
            } else if (L3_event_checkEventFlag(L3_event_dataToSend)) {
                if (strcmp((char*)originalWord, "quit") == 0) {
                    l3_state = TERMINATE;
                    pc.printf(":: Chat terminated by user input.\n");
                    wordLen = 0;
                    memset(originalWord, 0, sizeof(originalWord));
                    L3_event_clearEventFlag(L3_event_dataToSend);
                    break;
                }

                strcpy((char*)sdu, (char*)originalWord);
                debug("[L3] msg length : %i\n", wordLen);
                L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

                pc.printf("[You] %s\n", sdu);

                wordLen = 0;
                pc.printf("Give a word to send : ");
                L3_event_clearEventFlag(L3_event_dataToSend);
            }
            break;
        }
        case CHAT_READY: {
            pc.printf(":: [L3] Entering CHAT_READY...\n");
            l3_state = IDLE;
            break;
        }
        case TERMINATE: {
            pc.printf(":: [L3] Entering TERMINATE. Chat session ended.\n");
            L3_event_clearAllEventFlag();
            pc.attach(NULL);
            while (1);
            break;
        }
        default:
            break;
    }
}
