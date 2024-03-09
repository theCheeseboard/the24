#ifndef ALARMMODEL_H
#define ALARMMODEL_H

#include <QAbstractListModel>
#include <QCoroTask>
#include <QQmlEngine>

struct AlarmModelPrivate;
class AlarmModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(bool serviceAvailable READ isServiceAvailable NOTIFY serviceAvailableChanged)
        QML_ELEMENT

    public:
        explicit AlarmModel(QObject* parent = nullptr);
        ~AlarmModel();

        enum Roles {
            ObjectPathRole = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        bool isServiceAvailable();

    private slots:
        void addAlarm(QString path);
        QCoro::Task<> alarmRemoved();

    signals:
        void serviceAvailableChanged();

    private:
        AlarmModelPrivate* d;

        QCoro::Task<> serviceAvailable();
        void serviceUnavailable();
};

#endif // ALARMMODEL_H
