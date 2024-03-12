#include "stopwatchpanecontroller.h"

#include "../common/clockhelpers.h"
#include <QCoroDBus>
#include <QDBusInterface>
#include <QElapsedTimer>

struct StopwatchPaneControllerPrivate {
        QDBusInterface* interface = nullptr;
        QString objectPath;
        QElapsedTimer elapsed;
        QTimer* timer;

        bool paused;
        qlonglong initialElapsed;
};

StopwatchPaneController::StopwatchPaneController(QObject* parent) :
    QObject{parent}, d{new StopwatchPaneControllerPrivate()} {
    d->timer = new QTimer(this);
    d->timer->setInterval(10);
    connect(d->timer, &QTimer::timeout, this, &StopwatchPaneController::displayStringChanged);
}

StopwatchPaneController::~StopwatchPaneController() {
    delete d;
}

QString StopwatchPaneController::objectPath() {
    return d->objectPath;
}

void StopwatchPaneController::setObjectPath(QString objectPath) {
    if (d->interface) {
        QDBusConnection::sessionBus().disconnect("com.vicr123.the24", objectPath, "com.vicr123.the24.Stopwatch", "StateChanged", this, SLOT(updateState()));
        d->interface->deleteLater();
        d->interface = nullptr;
    }

    d->objectPath = objectPath;

    if (!d->objectPath.isEmpty()) {
        d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Stopwatch", QDBusConnection::sessionBus(), this);
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Stopwatch", "StateChanged", this, SLOT(updateState()));

        updateState();
    }

    emit objectPathChanged();
}

bool StopwatchPaneController::paused() {
    return d->paused;
}

void StopwatchPaneController::setPaused(bool paused) {
    if (paused) {
        d->interface->asyncCall("Stop");
    } else {
        d->interface->asyncCall("Start");
    }
}

QString StopwatchPaneController::displayString() {
    qulonglong time = d->initialElapsed;
    if (!d->paused) {
        time += d->elapsed.elapsed();
    }
    return ClockHelpers::msecsToStringWithMsecs(time);
}

void StopwatchPaneController::reset() {
    d->interface->asyncCall("Reset");
}

void StopwatchPaneController::remove() {
    d->interface->asyncCall("Remove");
}

QCoro::Task<> StopwatchPaneController::updateState() {
    auto elapsedReply = co_await d->interface->asyncCall("Elapsed");
    d->initialElapsed = elapsedReply.arguments().constFirst().toLongLong();
    d->elapsed.start();

    auto pausedReply = co_await d->interface->asyncCall("Paused");
    d->paused = pausedReply.arguments().constFirst().toBool();

    if (d->paused) {
        d->timer->stop();
    } else {
        d->timer->start();
    }

    emit pausedChanged();
    emit displayStringChanged();
}
