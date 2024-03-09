#ifndef TIMEZONESMODEL_H
#define TIMEZONESMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

struct TimezonesModelPrivate;
class QTimeZone;
class TimezonesModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

    public:
        explicit TimezonesModel(QObject* parent = nullptr);
        ~TimezonesModel();

        enum TimezoneRoles {
            TimezoneId = Qt::UserRole,
            TimezoneOffset,
            TimezoneName,
            TimezoneLocation,
            TimezoneMarksNewOffset
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        QModelIndex timezone(QTimeZone timezone);

        Q_SCRIPTABLE void search(QString query);

    private:
        TimezonesModelPrivate* d;
};

#endif // TIMEZONESMODEL_H
