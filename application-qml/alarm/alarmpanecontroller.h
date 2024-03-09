#ifndef ALARMPANECONTROLLER_H
#define ALARMPANECONTROLLER_H

#include <QCoroTask>
#include <QObject>
#include <QQmlEngine>

struct AlarmPaneControllerPrivate;
class AlarmPaneController : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
        Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
        Q_PROPERTY(qulonglong offset READ offset WRITE setOffset NOTIFY offsetChanged)
        Q_PROPERTY(QString offsetString READ offsetString NOTIFY offsetChanged)
        Q_PROPERTY(qlonglong snoozeOffset READ snoozeOffset NOTIFY snoozeOffsetChanged)
        Q_PROPERTY(QString snoozeOffsetString READ snoozeOffsetString NOTIFY snoozeOffsetChanged)
        Q_PROPERTY(quint8 repeat READ repeat NOTIFY repeatChanged)
        Q_PROPERTY(QString repeatString READ repeatString NOTIFY repeatChanged)
        QML_ELEMENT
    public:
        explicit AlarmPaneController(QObject* parent = nullptr);
        ~AlarmPaneController();

        QString objectPath();
        void setObjectPath(QString objectPath);

        bool active();
        void setActive(bool active);

        qulonglong offset();
        void setOffset(qulonglong offset);
        QString offsetString();

        qlonglong snoozeOffset();
        QString snoozeOffsetString();

        quint8 repeat();
        QString repeatString();

        Q_SCRIPTABLE void remove();

    private slots:
        QCoro::Task<> updateActive();
        QCoro::Task<> updateOffset();
        QCoro::Task<> updateSnoozeOffset();
        QCoro::Task<> updateRepeat();

    signals:
        void objectPathChanged();
        void activeChanged();
        void offsetChanged();
        void snoozeOffsetChanged();
        void repeatChanged();

    private:
        AlarmPaneControllerPrivate* d;
};

#endif // ALARMPANECONTROLLER_H
