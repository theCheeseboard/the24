#include "timerpanecontroller.h"

#include "../common/clockhelpers.h"
#include <QCoroDBus>
#include <QDBusInterface>
#include <QTimer>

struct TimerPaneControllerPrivate {
        QDBusInterface* interface = nullptr;
        QString objectPath;

        QTimer* timer;
        QString state;
        qlonglong msecsLeft;
};

TimerPaneController::TimerPaneController(QObject* parent) :
    QObject{parent}, d{new TimerPaneControllerPrivate} {
    d->timer = new QTimer(this);
    d->timer->setInterval(1000);
    connect(d->timer, &QTimer::timeout, this, [this] {
        d->msecsLeft -= 1000;
        emit displayStringChanged();
    });
}

TimerPaneController::~TimerPaneController() {
    delete d;
}

QString TimerPaneController::objectPath() {
    return d->objectPath;
}

void TimerPaneController::setObjectPath(QString objectPath) {
    if (d->interface) {
        QDBusConnection::sessionBus().disconnect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "StateChanged", this, SLOT(updateState()));
        d->interface->deleteLater();
        d->interface = nullptr;
    }

    d->objectPath = objectPath;

    if (!d->objectPath.isEmpty()) {
        d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", QDBusConnection::sessionBus(), this);
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "StateChanged", this, SLOT(updateState()));
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "LengthChanged", this, SLOT(updateLength()));
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Timer", "NoteChanged", this, SLOT(updateNote()));

        updateState();
        updateLength();
        updateNote();
    }

    emit objectPathChanged();
}

QString TimerPaneController::displayString() {
    return ClockHelpers::msecsToString(d->msecsLeft);
}

QString TimerPaneController::state() {
    return d->state;
}

void TimerPaneController::pauseStart() {
    if (d->state == "Running") {
        d->interface->asyncCall("Pause");
    } else {
        d->interface->asyncCall("Resume");
    }
}

void TimerPaneController::reset() {
    d->interface->asyncCall("Reset");
}

void TimerPaneController::remove() {
    d->interface->asyncCall("Remove");
}

QCoro::Task<> TimerPaneController::updateState() {
    d->timer->stop();
    auto stateReply = co_await d->interface->asyncCall("State");
    d->state = stateReply.arguments().constFirst().toString();

    auto msRemainReply = co_await d->interface->asyncCall("MillisecondsRemaining");
    d->msecsLeft = msRemainReply.arguments().constFirst().toLongLong();

    if (d->state == "Running") {
        d->timer->start();
    }

    emit displayStringChanged();
    emit stateChanged();
}

QCoro::Task<> TimerPaneController::updateLength() {
    co_return;
}

QCoro::Task<> TimerPaneController::updateNote() {
    co_return;
}
