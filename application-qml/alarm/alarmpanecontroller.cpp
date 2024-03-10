#include "alarmpanecontroller.h"

#include <QCoroDBus>
#include <QDBusInterface>

struct AlarmPaneControllerPrivate {
        QDBusInterface* interface = nullptr;
        QString objectPath;

        bool active = false;
        qulonglong offset = 0;
        qlonglong snoozeOffset = -1;

        enum Day {
            None = 0,
            Monday = 0x1,
            Tuesday = 0x2,
            Wednesday = 0x4,
            Thursday = 0x8,
            Friday = 0x10,
            Saturday = 0x20,
            Sunday = 0x40,

            AllDays = Monday | Tuesday | Wednesday | Thursday | Friday | Saturday | Sunday

            // Keep this in sync with Alarm
        };
        typedef QFlags<Day> Days;

        Days repeats = None;
};

AlarmPaneController::AlarmPaneController(QObject* parent) :
    QObject{parent}, d{new AlarmPaneControllerPrivate()} {
}

AlarmPaneController::~AlarmPaneController() {
    delete d;
}

QString AlarmPaneController::objectPath() {
    return d->objectPath;
}

void AlarmPaneController::setObjectPath(QString objectPath) {
    if (d->interface) {
        QDBusConnection::sessionBus().disconnect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "ActiveChanged", this, SLOT(updateActive()));
        QDBusConnection::sessionBus().disconnect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "OffsetChanged", this, SLOT(updateOffset()));
        QDBusConnection::sessionBus().disconnect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "SnoozeOffsetChanged", this, SLOT(updateSnoozeOffset()));
        QDBusConnection::sessionBus().disconnect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "RepeatChanged", this, SLOT(updateRepeat()));
        d->interface->deleteLater();
        d->interface = nullptr;
    }

    d->objectPath = objectPath;

    if (!d->objectPath.isEmpty()) {
        d->interface = new QDBusInterface("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", QDBusConnection::sessionBus(), this);
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "ActiveChanged", this, SLOT(updateActive()));
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "OffsetChanged", this, SLOT(updateOffset()));
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "SnoozeOffsetChanged", this, SLOT(updateSnoozeOffset()));
        QDBusConnection::sessionBus().connect("com.vicr123.the24", objectPath, "com.vicr123.the24.Alarm", "RepeatChanged", this, SLOT(updateRepeat()));

        updateActive();
        updateOffset();
        updateSnoozeOffset();
        updateRepeat();
    }

    emit objectPathChanged();
}

bool AlarmPaneController::active() {
    return d->active;
}

void AlarmPaneController::setActive(bool active) {
    d->interface->asyncCall("SetActive", active);
}

qulonglong AlarmPaneController::offset() {
    return d->offset;
}

void AlarmPaneController::setOffset(qulonglong offset) {
}

QString AlarmPaneController::offsetString() {
    return QTime::fromMSecsSinceStartOfDay(d->offset).toString("hh:mm");
}

qlonglong AlarmPaneController::snoozeOffset() {
    return d->snoozeOffset;
}

QString AlarmPaneController::snoozeOffsetString() {
    if (d->snoozeOffset < 0) {
        return {};
    } else {
        auto snoozeOffset = QTime::fromMSecsSinceStartOfDay(d->snoozeOffset);
        return tr("Snoozed until %1").arg(snoozeOffset.toString("hh:mm"));
    }
}

quint8 AlarmPaneController::repeat() {
    return d->repeats;
}

QString AlarmPaneController::repeatString() {
    QLocale l;

    AlarmPaneControllerPrivate::Days weekdayDays = AlarmPaneControllerPrivate::None;
    for (Qt::DayOfWeek day : l.weekdays()) {
        weekdayDays |= static_cast<AlarmPaneControllerPrivate::Day>(1 << (day - 1));
    }

    auto alldays = d->repeats == AlarmPaneControllerPrivate::AllDays;
    auto weekdays = d->repeats == weekdayDays;
    auto weekends = d->repeats == (~weekdayDays & AlarmPaneControllerPrivate::AllDays);

    if (d->repeats == AlarmPaneControllerPrivate::None) {
        return {};
    } else if (alldays) {
        return tr("Repeats every day");
    } else if (weekdays) {
        return tr("Repeats on Weekdays");
    } else if (weekends) {
        return tr("Repeats on Weekends");
    } else {
        QStringList repeatsList;
        for (int i = l.firstDayOfWeek() - 1; i < l.firstDayOfWeek() + 6; i++) {
            int day = i % 7;
            if (d->repeats & 1 << day) repeatsList.append(l.dayName(day + 1, QLocale::ShortFormat));
        }
        return tr("Repeats on %1").arg(l.createSeparatedList(repeatsList));
    }
}

void AlarmPaneController::remove() {
    d->interface->asyncCall("Remove");
}

void AlarmPaneController::setRepeatDay(quint8 day, bool on) {
    auto repeats = d->repeats;
    repeats = repeats.setFlag(static_cast<AlarmPaneControllerPrivate::Day>(1 << day), on);
    if (!d->interface) {
        // Update the private member and notify
        d->repeats = repeats;
        emit repeatChanged();
    } else {
        d->interface->asyncCall("SetRepeat", static_cast<qulonglong>(repeats));
    }
}

QCoro::Task<> AlarmPaneController::updateActive() {
    auto active = co_await d->interface->asyncCall("Active");
    d->active = active.arguments().constFirst().toBool();
    emit activeChanged();
}

QCoro::Task<> AlarmPaneController::updateOffset() {
    auto offset = co_await d->interface->asyncCall("Offset");
    d->offset = offset.arguments().constFirst().toULongLong();
    emit offsetChanged();
}

QCoro::Task<> AlarmPaneController::updateSnoozeOffset() {
    auto snoozeOffset = co_await d->interface->asyncCall("SnoozeOffset");
    d->snoozeOffset = snoozeOffset.arguments().constFirst().toLongLong();
    emit snoozeOffsetChanged();
}

QCoro::Task<> AlarmPaneController::updateRepeat() {
    auto repeats = co_await d->interface->asyncCall("Repeat");
    d->repeats = static_cast<AlarmPaneControllerPrivate::Days>(repeats.arguments().first().toInt());
    emit repeatChanged();
}
