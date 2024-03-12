#ifndef STOPWATCHMODEL_H
#define STOPWATCHMODEL_H

#include <QAbstractListModel>
#include <QCoroTask>
#include <QQmlEngine>

struct StopwatchModelPrivate;
class StopwatchModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(bool serviceAvailable READ isServiceAvailable NOTIFY serviceAvailableChanged)
        QML_ELEMENT

    public:
        explicit StopwatchModel(QObject* parent = nullptr);
        ~StopwatchModel();

        enum Roles {
            ObjectPathRole = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        bool isServiceAvailable();

        Q_SCRIPTABLE void newStopwatch();

    private slots:
        void addStopwatch(QString path);
        QCoro::Task<> stopwatchRemoved();

    signals:
        void serviceAvailableChanged();

    private:
        struct StopwatchModelPrivate* d;

        QCoro::Task<> serviceAvailable();
        void serviceUnavailable();
};

#endif // STOPWATCHMODEL_H
