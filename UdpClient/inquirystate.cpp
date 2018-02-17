#include "inquirystate.h"

InquiryState::InquiryState() :
    timerId_(0)
{

}

void InquiryState::startTimerRequest()
{
    timerId_ = startTimer(2000);
}

void InquiryState::stopTimerRequest()
{
    killTimer(timerId_);
}

void InquiryState::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerId_) {          //!< если время ожидания ответа на запрос вышло
        stopTimerRequest();
        emit responseReceived(false);
    }
}
