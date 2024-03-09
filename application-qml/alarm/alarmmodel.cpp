#include "alarmmodel.h"

#include <QCoroDBus>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>

struct AlarmModelPrivate {
        // QList<AlarmWidget*> alarms;
        QDBusServiceWatcher* watcher;
        QStringList alarms;
        bool serviceAvailable = false;
};

AlarmModel::AlarmModel(QObject* parent) :
    QAbstractListModel(parent), d{new AlarmModelPrivate()} {
    d->watcher = new QDBusServiceWatcher("com.vicr123.the24", QDBusConnection::sessionBus());
    connect(d->watcher, &QDBusServiceWatcher::serviceRegistered, this, &AlarmModel::serviceAvailable);
    connect(d->watcher, &QDBusServiceWatcher::serviceUnregistered, this, &AlarmModel::serviceUnavailable);

    QTimer::singleShot(0, this, [this] {
        if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
            serviceAvailable();
        } else {
            serviceUnavailable();
        }
    });

    QDBusConnection::sessionBus().connect("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AlarmAdded", this, SLOT(addAlarm(QString)));
}

AlarmModel::~AlarmModel() {
    delete d;
}

int AlarmModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return d->alarms.length();
}

QVariant AlarmModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto alarm = d->alarms.at(index.row());
    if (role == ObjectPathRole) {
        return alarm;
    }
    return QVariant();
}

bool AlarmModel::isServiceAvailable() {
    return d->serviceAvailable;
}

QCoro::Task<> AlarmModel::serviceAvailable() {
    this->beginResetModel();
    d->alarms.clear();

    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateAlarms");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);
    auto paths = reply.arguments().constFirst().toStringList();
    for (const auto& path : paths) {
        QDBusConnection::sessionBus().connect("com.vicr123.the24", path, "com.vicr123.the24.Alarm", "Removing", this, SLOT(alarmRemoved()));
        d->alarms.append(path);
    }
    this->endResetModel();

    d->serviceAvailable = true;
    emit serviceAvailableChanged();
}

void AlarmModel::serviceUnavailable() {
    d->serviceAvailable = false;
    emit serviceAvailableChanged();
}

void AlarmModel::addAlarm(QString path) {
    QDBusConnection::sessionBus().connect("com.vicr123.the24", path, "com.vicr123.the24.Alarm", "Removing", this, SLOT(alarmRemoved()));

    this->beginInsertRows({}, d->alarms.length(), d->alarms.length());
    d->alarms.append(path);
    this->endInsertRows();
}

QCoro::Task<> AlarmModel::alarmRemoved() {
    // Get all alarms
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateAlarms");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);
    auto paths = reply.arguments().constFirst().toStringList();

    // Remove any alarms that are no longer in the reply
    for (auto i = 0; i < d->alarms.length(); i++) {
        if (paths.contains(d->alarms.at(i))) continue;

        this->beginRemoveRows({}, i, i);
        d->alarms.removeAt(i);
        this->endRemoveRows();

        i--;
    }
}

QHash<int, QByteArray> AlarmModel::roleNames() const {
    return {
        {ObjectPathRole, "objectPath"}
    };
}
