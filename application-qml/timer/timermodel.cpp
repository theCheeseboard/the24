#include "timermodel.h"

#include <QCoroDBus>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QTimer>

struct TimerModelPrivate {
        QDBusServiceWatcher* watcher;
        QStringList timers;
        bool serviceAvailable = false;
};

TimerModel::TimerModel(QObject* parent) :
    QAbstractListModel(parent), d{new TimerModelPrivate()} {
    d->watcher = new QDBusServiceWatcher("com.vicr123.the24", QDBusConnection::sessionBus());
    connect(d->watcher, &QDBusServiceWatcher::serviceRegistered, this, &TimerModel::serviceAvailable);
    connect(d->watcher, &QDBusServiceWatcher::serviceUnregistered, this, &TimerModel::serviceUnavailable);

    QTimer::singleShot(0, this, [this] {
        if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
            serviceAvailable();
        } else {
            serviceUnavailable();
        }
    });

    QDBusConnection::sessionBus().connect("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "TimerAdded", this, SLOT(addTimer(QString)));
}

TimerModel::~TimerModel() {
    delete d;
}

int TimerModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return d->timers.length();
}

QVariant TimerModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto timer = d->timers.at(index.row());
    if (role == ObjectPathRole) {
        return timer;
    }
    return QVariant();
}

QHash<int, QByteArray> TimerModel::roleNames() const {
    return {
        {ObjectPathRole, "objectPath"}
    };
}

bool TimerModel::isServiceAvailable() {
    return d->serviceAvailable;
}

void TimerModel::addTimer(QString path) {
    QDBusConnection::sessionBus().connect("com.vicr123.the24", path, "com.vicr123.the24.Timer", "Removing", this, SLOT(timerRemoved()));

    this->beginInsertRows({}, d->timers.length(), d->timers.length());
    d->timers.append(path);
    this->endInsertRows();
}

QCoro::Task<> TimerModel::timerRemoved() {
    // Get all timers
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateTimers");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);
    auto paths = reply.arguments().constFirst().toStringList();

    // Remove any timers that are no longer in the reply
    for (auto i = 0; i < d->timers.length(); i++) {
        if (paths.contains(d->timers.at(i))) continue;

        this->beginRemoveRows({}, i, i);
        d->timers.removeAt(i);
        this->endRemoveRows();

        i--;
    }
}

QCoro::Task<> TimerModel::serviceAvailable() {
    this->beginResetModel();
    d->timers.clear();

    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateTimers");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);
    auto paths = reply.arguments().constFirst().toStringList();
    for (const auto& path : paths) {
        QDBusConnection::sessionBus().connect("com.vicr123.the24", path, "com.vicr123.the24.Timer", "Removing", this, SLOT(timerRemoved()));
        d->timers.append(path);
    }
    this->endResetModel();

    d->serviceAvailable = true;
    emit serviceAvailableChanged();
}

void TimerModel::serviceUnavailable() {
    d->serviceAvailable = false;
    emit serviceAvailableChanged();
}
