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
static uint8_t myId;
void L3_quiz_showSelectedToUser(Serial &pc); //

void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3, "word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE - 1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
        }
    }
}

void L3_initFSM(uint8_t thisId, uint8_t destId)
{
    myId = thisId;
    myDestId = destId;
    pc.attach(&L3service_processInputWord, Serial::RxIrq);
    // pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{
    if (prev_state != l3_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %s to %s\n",
                 L3_stateToStr(prev_state), L3_stateToStr(l3_state));
        prev_state = l3_state;
    }

    switch (l3_state)
    {
    case WAIT_ANSWER:
    {
        L3_quiz_showMenuToHost(pc);
        QuizSelectResult result = L3_quiz_select(pc);

        if (result == QUIZ_SELECTION_FAILED)
        {
            pc.printf("[ERROR] Quiz selection failed. Terminating.\n\n");
            l3_state = TERMINATE;
            break;
        }

        if (result == QUIZ_JOIN_AS_USER)
        {
            const char *joinMsg = "join";
            L3_LLI_dataReqFunc((uint8_t *)joinMsg, strlen(joinMsg) + 1, myDestId);
            pc.printf("[You] join (sent)\n\n");
            pc.printf("[Switching to IDLE state.]\n\n");
            l3_state = IDLE;
            break;
        }

        // 정상 퀴즈 선택
        char quizIndexMsg[8];
        sprintf(quizIndexMsg, "quiz%d", selected_quiz_index);
        L3_LLI_dataReqFunc((uint8_t *)quizIndexMsg, strlen(quizIndexMsg) + 1, myDestId);
        pc.printf("[Quiz index %d sent to user.]\n\n", selected_quiz_index);
        l3_state = CHAT_READY;
        break;
    }

    case WAIT_QUIZ:
    {
        // pc.printf(":: role: USER\n");
        L3_quiz_showSelectedToUser(pc);

        pc.attach(NULL, Serial::RxIrq); // 시리얼 인터럽트 해제

        const int maxAttempts = 3;
        Timer quizTimer;
        quizTimer.start();

        for (int attempt = 1; attempt <= maxAttempts; attempt++)
        {
            float elapsed = quizTimer.read();
            float remaining = 120.0f - elapsed;

            if (remaining <= 0.0f)
            {
                pc.printf("[TIMEOUT] 2 minutes are over. Terminating.\n\n");
                l3_state = TERMINATE;
                break;
            }

            pc.printf("\n[Attempt %d/%d (Time left: %d seconds\n\n)]", attempt, maxAttempts, (int)remaining);
            pc.printf("Answer: ");

            char answerBuffer[MAX_PASSWORD_LEN] = {0};
            bool gotInput = getInputWithinTime(pc, answerBuffer, sizeof(answerBuffer), remaining);

            if (!gotInput)
            {
                pc.printf("\n[Incorrect answer. (No input detected)]\n\n");
                continue;
            }

            if (L3_quiz_isAnswerCorrect(answerBuffer))
            {
                pc.printf("\n[Correct! Moving to CHAT_READY state.]\n\n");
                l3_state = CHAT_READY;
                break;
            }
            else
            {
                pc.printf("\n[Incorrect answer.]\n\n");
            }

            if (attempt == maxAttempts)
            {
                pc.printf("[You've used all attempts. Terminating session.]\n\n");
                l3_state = TERMINATE;
            }
        }

        pc.attach(&L3service_processInputWord, Serial::RxIrq); // 다시 attach
        break;
    }
    case IDLE:
    {
        if (L3_event_checkEventFlag(L3_event_msgRcvd))
        {
            uint8_t *dataPtr = L3_LLI_getMsgPtr();
            uint8_t size = L3_LLI_getSize();

            char printBuf[1030] = {0};
            memcpy(printBuf, dataPtr, size);
            printBuf[size] = '\0';

            // debug("\n -------------------------------------------------\n");
            // debug("RCVD MSG : %s (length:%i)\n", printBuf, size);
            // debug(" -------------------------------------------------\n");

            if (strncmp((char *)dataPtr, "quiz", 4) == 0)
            {
                selected_quiz_index = dataPtr[4] - '0';
                if (selected_quiz_index >= 0 && selected_quiz_index < QUIZ_TOTAL_COUNT)
                {
                    strncpy(selected_answer, quiz_answers[selected_quiz_index], MAX_PASSWORD_LEN);
                    pc.printf("[Quiz Received] Index: %d\n\n", selected_quiz_index);
                    l3_state = WAIT_QUIZ;
                }
                else
                {
                    pc.printf("[Error] Invalid quiz index received.\n\n");
                    l3_state = TERMINATE;
                }
                L3_event_clearEventFlag(L3_event_msgRcvd);
                return;
            }

            if (strncmp((char *)dataPtr, "quit", 4) == 0)
            {
                pc.printf("['quit' received from peer. Terminating chat...]\n\n");
                l3_state = TERMINATE;
                L3_event_clearEventFlag(L3_event_msgRcvd);
                return;
            }
            pc.printf("[Peer] %s\n\n", printBuf);
            // pc.printf("Give a word to send : ");

            L3_event_clearEventFlag(L3_event_msgRcvd);
        }
        else if (L3_event_checkEventFlag(L3_event_dataToSend))
        {
            if (strcmp((char *)originalWord, "quit") == 0)
            {
                l3_state = TERMINATE;
                pc.printf("[Chat terminated by user input.]\n\n");
                wordLen = 0;
                memset(originalWord, 0, sizeof(originalWord));
                L3_event_clearEventFlag(L3_event_dataToSend);
                break;
            }

            strcpy((char *)sdu, (char *)originalWord);
            // debug("[L3] msg length : %i\n", wordLen);
            L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

            pc.printf("[You] %s\n\n", sdu);
            wordLen = 0;
            // pc.printf("Give a word to send : ");
            L3_event_clearEventFlag(L3_event_dataToSend);
        }
        break;
    }
    case CHAT_READY:
    {
        pc.printf("[Entering CHAT_READY...]\n\n");
        l3_state = IDLE;
        break;
    }
    case TERMINATE:
    {
        pc.printf("[Entering TERMINATE. Chat session ended.]\n\n");
        L3_event_clearAllEventFlag();
        pc.attach(NULL);
        while (1)
            ;
        break;
    }
    default:
        break;
    }
}