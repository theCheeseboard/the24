#ifndef WORLDCLOCK_H
#define WORLDCLOCK_H

#include <QObject>
#include <QQmlEngine>
#include <QTimeZone>

struct WorldClockPrivate;
class WorldClock : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString timezone READ timezone WRITE setTimezone NOTIFY currentTimezoneChanged)
        Q_PROPERTY(QDateTime currentDateTime READ currentDateTime NOTIFY currentDateTimeChanged)
        Q_PROPERTY(QString currentDateTimeString READ currentDateTimeString NOTIFY currentDateTimeChanged)
        Q_PROPERTY(QString dateDifferenceString READ dateDifferenceString NOTIFY currentDateTimeChanged)
        Q_PROPERTY(QString informationString READ informationString NOTIFY informationStringChanged)
        Q_PROPERTY(bool removable READ removable CONSTANT)
        QML_NAMED_ELEMENT(WorldClockItemController)

    public:
        explicit WorldClock(QObject* parent = nullptr);
        ~WorldClock();

        QString timezone();
        void setTimezone(QString timezone);

        QDateTime currentDateTime();
        QString currentDateTimeString();
        QString dateDifferenceString();
        QString informationString();

        bool removable();

    signals:
        void currentDateTimeChanged();
        void informationStringChanged();
        void currentTimezoneChanged();

    private:
        WorldClockPrivate* d;
};

#endif // WORLDCLOCK_H
