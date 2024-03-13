#ifndef TIMERPANECONTROLLER_H
#define TIMERPANECONTROLLER_H

#include <QCoroTask>
#include <QObject>
#include <QQmlEngine>

struct TimerPaneControllerPrivate;
class TimerPaneController : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged);
        Q_PROPERTY(QString displayString READ displayString NOTIFY displayStringChanged);
        Q_PROPERTY(QString state READ state NOTIFY stateChanged)
        QML_ELEMENT
    public:
        explicit TimerPaneController(QObject* parent = nullptr);
        ~TimerPaneController();

        QString objectPath();
        void setObjectPath(QString objectPath);

        QString displayString();

        QString state();
        Q_SCRIPTABLE void pauseStart();
        Q_SCRIPTABLE void reset();

        Q_SCRIPTABLE void remove();

    signals:
        void objectPathChanged();
        void displayStringChanged();
        void stateChanged();

    private slots:
        QCoro::Task<> updateState();
        QCoro::Task<> updateLength();
        QCoro::Task<> updateNote();

    private:
        TimerPaneControllerPrivate* d;
};

#endif // TIMERPANECONTROLLER_H
