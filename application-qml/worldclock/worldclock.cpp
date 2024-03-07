#include "worldclock.h"
#include <QTimer>
#include <libcontemporary_global.h>

struct WorldClockPrivate {
        QTimeZone tz;
        QTimer* ticker;
        bool removable;
};

WorldClock::WorldClock(QObject* parent) :
    QObject{parent}, d{new WorldClockPrivate()} {
    d->ticker = new QTimer(this);
    d->ticker->setInterval(50);
    connect(d->ticker, &QTimer::timeout, this, &WorldClock::currentDateTimeChanged);
    d->ticker->start();
}

WorldClock::~WorldClock() {
    delete d;
}

QString WorldClock::timezone() {
    return d->tz.id();
}

void WorldClock::setTimezone(QString timezone) {
    d->tz = QTimeZone(timezone.toUtf8());
    emit currentTimezoneChanged();
    emit currentDateTimeChanged();
    emit informationStringChanged();
}

QDateTime WorldClock::currentDateTime() {
    auto date = QDateTime::currentDateTimeUtc();
    int utcOffset = d->tz.offsetFromUtc(date);
    date = date.addSecs(d->tz.offsetFromUtc(date));
    return date;
}

QString WorldClock::currentDateTimeString() {
    return QLocale().toString(this->currentDateTime().time(), QLocale::ShortFormat);
}

QString WorldClock::dateDifferenceString() {
    auto date = QDateTime::currentDateTimeUtc();
    int utcOffset = d->tz.offsetFromUtc(date);
    date = date.addSecs(d->tz.offsetFromUtc(date));
    int dayDifference = QDateTime::currentDateTime().daysTo(date);
    if (dayDifference == 0) {
        return {};
    } else if (dayDifference < 0) {
        return QStringLiteral("-%1").arg(dayDifference * -1);
    } else {
        return QStringLiteral("+%1").arg(dayDifference);
    }
}

QString WorldClock::informationString() {
    auto date = QDateTime::currentDateTimeUtc();
    int utcOffset = d->tz.offsetFromUtc(date);

    QStringList labelParts;

    int differenceFromLocal = utcOffset - QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTimeUtc());
    if (differenceFromLocal == 0) {
        labelParts.append(tr("Current Timezone"));
    } else {
        QTime offset = QTime(0, 0).addSecs(qAbs(differenceFromLocal));
        if (offset.minute() == 0) {
            if (differenceFromLocal < 0) {
                labelParts.append(tr("%n hours behind", nullptr, qAbs(differenceFromLocal / 3600)));
            } else {
                labelParts.append(tr("%n hours ahead", nullptr, qAbs(differenceFromLocal / 3600)));
            }
        } else {
            if (differenceFromLocal < 0) {
                labelParts.append(tr("%1 behind").arg(offset.toString("hh:mm")));
            } else {
                labelParts.append(tr("%1 ahead").arg(offset.toString("hh:mm")));
            }
        }
    }

    labelParts.append(d->tz.displayName(QDateTime::currentDateTime(), QTimeZone::LongName));
    return labelParts.join(libContemporaryCommon::humanReadablePartJoinString());
}

bool WorldClock::removable() {
    return d->removable;
}
