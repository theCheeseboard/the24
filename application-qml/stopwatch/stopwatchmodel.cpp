#include "stopwatchmodel.h"

#include <QCoroDBus>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QTimer>

struct StopwatchModelPrivate {
        QDBusServiceWatcher* watcher;
        QStringList stopwatches;
        bool serviceAvailable = false;
};

StopwatchModel::StopwatchModel(QObject* parent) :
    QAbstractListModel(parent), d{new StopwatchModelPrivate()} {
    d->watcher = new QDBusServiceWatcher("com.vicr123.the24", QDBusConnection::sessionBus());
    connect(d->watcher, &QDBusServiceWatcher::serviceRegistered, this, &StopwatchModel::serviceAvailable);
    connect(d->watcher, &QDBusServiceWatcher::serviceUnregistered, this, &StopwatchModel::serviceUnavailable);

    QTimer::singleShot(0, this, [this] {
        if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.the24").value()) {
            serviceAvailable();
        } else {
            serviceUnavailable();
        }
    });

    QDBusConnection::sessionBus().connect("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "StopwatchAdded", this, SLOT(addStopwatch(QString)));
}

StopwatchModel::~StopwatchModel() {
    delete d;
}

int StopwatchModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return d->stopwatches.length();
}

QVariant StopwatchModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto stopwatch = d->stopwatches.at(index.row());
    if (role == ObjectPathRole) {
        return stopwatch;
    }
    return QVariant();
}

QHash<int, QByteArray> StopwatchModel::roleNames() const {
    return {
        {ObjectPathRole, "objectPath"}
    };
}

bool StopwatchModel::isServiceAvailable() {
    return d->serviceAvailable;
}

void StopwatchModel::newStopwatch() {
    QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "AddStopwatch");
    QDBusConnection::sessionBus().asyncCall(message);
}

void StopwatchModel::addStopwatch(QString path) {
    QDBusConnection::sessionBus().connect("com.vicr123.the24", path, "com.vicr123.the24.Stopwatch", "Removing", this, SLOT(stopwatchRemoved()));

    this->beginInsertRows({}, d->stopwatches.length(), d->stopwatches.length());
    d->stopwatches.append(path);
    this->endInsertRows();
}

QCoro::Task<> StopwatchModel::stopwatchRemoved() {
    // Get all stopwatches
    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateStopwatches");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);
    auto paths = reply.arguments().constFirst().toStringList();

    // Remove any stopwatches that are no longer in the reply
    for (auto i = 0; i < d->stopwatches.length(); i++) {
        if (paths.contains(d->stopwatches.at(i))) continue;

        this->beginRemoveRows({}, i, i);
        d->stopwatches.removeAt(i);
        this->endRemoveRows();

        i--;
    }
}

QCoro::Task<> StopwatchModel::serviceAvailable() {
    this->beginResetModel();
    d->stopwatches.clear();

    QDBusMessage enumerate = QDBusMessage::createMethodCall("com.vicr123.the24", "/com/vicr123/the24", "com.vicr123.the24", "EnumerateStopwatches");
    auto reply = co_await QDBusConnection::sessionBus().asyncCall(enumerate);
    auto paths = reply.arguments().constFirst().toStringList();
    for (const auto& path : paths) {
        QDBusConnection::sessionBus().connect("com.vicr123.the24", path, "com.vicr123.the24.Stopwatch", "Removing", this, SLOT(stopwatchRemoved()));
        d->stopwatches.append(path);
    }
    this->endResetModel();

    d->serviceAvailable = true;
    emit serviceAvailableChanged();
}

void StopwatchModel::serviceUnavailable() {
    d->serviceAvailable = false;
    emit serviceAvailableChanged();
}
