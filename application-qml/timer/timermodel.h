#ifndef TIMERMODEL_H
#define TIMERMODEL_H

#include <QAbstractListModel>
#include <QCoroTask>
#include <QQmlEngine>

struct TimerModelPrivate;
class TimerModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(bool serviceAvailable READ isServiceAvailable NOTIFY serviceAvailableChanged)
        QML_ELEMENT

    public:
        explicit TimerModel(QObject* parent = nullptr);
        ~TimerModel();

        enum Roles {
            ObjectPathRole = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        bool isServiceAvailable();

    private slots:
        void addTimer(QString path);
        QCoro::Task<> timerRemoved();

    signals:
        void serviceAvailableChanged();

    private:
        TimerModelPrivate* d;

        QCoro::Task<> serviceAvailable();
        void serviceUnavailable();
};

#endif // TIMERMODEL_H
