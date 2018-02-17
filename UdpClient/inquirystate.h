#ifndef INQUIRYSTATE_H
#define INQUIRYSTATE_H

#include <QObject>
#include <QTimerEvent>

class InquiryState : public QObject
{

    Q_OBJECT

public:
    InquiryState();

    void startTimerRequest();
    void stopTimerRequest();

private:
    int timerId_;

protected:
    /**
     * @brief timerEvent метод обработки события наступившего по времени
     * @param event событие
     */
    virtual void timerEvent(QTimerEvent *event);

signals:
    void responseReceived(bool response);
};

#endif // INQUIRYSTATE_H
