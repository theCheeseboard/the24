#ifndef WORLDCLOCKCONTROLLER_H
#define WORLDCLOCKCONTROLLER_H

#include <QAbstractListModel>
#include <QObject>
#include <QQmlEngine>

struct WorldClockControllerPrivate;
class WorldClock;
class WorldClockController : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

    public:
        explicit WorldClockController(QObject* parent = nullptr);
        ~WorldClockController();

        enum WorldClockControllerRoles {
            TimezoneRole = Qt::UserRole,
            RemovableRole
        };

        // QObjectList worldClocks();
        int rowCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

        Q_SCRIPTABLE void addClock(QString timezone);
        Q_SCRIPTABLE void removeClock(QString timezone);

    private:
        WorldClockControllerPrivate* d;

        void updateClocks();
};

#endif // WORLDCLOCKCONTROLLER_H
