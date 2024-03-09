#include "timezonesmodel.h"

#include <QPainter>
#include <QTimeZone>

struct TimezonesModelPrivate {
        QList<QTimeZone> timezones;
        QList<QTimeZone> shownTimezones;
};

TimezonesModel::TimezonesModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new TimezonesModelPrivate();
    QDateTime now = QDateTime::currentDateTimeUtc();
    for (QByteArray timezone : QTimeZone::availableTimeZoneIds()) {
        QTimeZone tz(timezone);
        if (timezone == tz.displayName(now, QTimeZone::OffsetName)) continue; // Ignore
        d->timezones.append(tz);
    }

    // Sort timezones
    std::sort(d->timezones.begin(), d->timezones.end(), [=](const QTimeZone& first, const QTimeZone& second) {
        if (first.offsetFromUtc(now) < second.offsetFromUtc(now)) {
            return true;
        } else if (first.offsetFromUtc(now) > second.offsetFromUtc(now)) {
            return false;
        } else {
            return QString(first.id()).split("/").last().localeAwareCompare(QString(second.id()).split("/").last()) < 0;
        }
    });

    // Initialize the shown items
    search("");
}

TimezonesModel::~TimezonesModel() {
    delete d;
}

int TimezonesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->shownTimezones.count();
}

QVariant TimezonesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    QDateTime now = QDateTime::currentDateTimeUtc();
    QTimeZone tz = d->shownTimezones.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        case TimezoneLocation:
            return QString(tz.id()).split("/").last().replace("_", " ");
        case TimezoneId:
            return tz.id();
        case TimezoneOffset:
            return tz.displayName(now, QTimeZone::OffsetName);
        case TimezoneName:
            return tz.displayName(now, QTimeZone::LongName);
        case TimezoneMarksNewOffset:
            return index.row() == 0 || d->shownTimezones.at(index.row() - 1).offsetFromUtc(now) != tz.offsetFromUtc(now);
    }
    return QVariant();
}

QHash<int, QByteArray> TimezonesModel::roleNames() const {
    return {
        {TimezoneId,             "timezoneId"            },
        {TimezoneOffset,         "timezoneOffset"        },
        {TimezoneName,           "timezoneName"          },
        {TimezoneLocation,       "timezoneLocation"      },
        {TimezoneMarksNewOffset, "timezoneMarksNewOffset"}
    };
}

QModelIndex TimezonesModel::timezone(QTimeZone timezone) {
    for (int i = 0; i < d->timezones.count(); i++) {
        if (d->timezones.at(i) == timezone) return index(i);
    }
    return QModelIndex();
}

void TimezonesModel::search(QString query) {
    this->beginResetModel();
    d->shownTimezones.clear();
    if (query == "") {
        d->shownTimezones = d->timezones;
    } else {
        QDateTime now = QDateTime::currentDateTime();
        for (QTimeZone tz : d->timezones) {
            bool match = false;
            if (QString(tz.id()).contains(query, Qt::CaseInsensitive)) match = true;
            if (QString(tz.id()).replace("_", " ").contains(query, Qt::CaseInsensitive)) match = true;
            if (tz.displayName(now, QTimeZone::OffsetName).contains(query, Qt::CaseInsensitive)) match = true;
            if (tz.displayName(now, QTimeZone::LongName).contains(query, Qt::CaseInsensitive)) match = true;
            if (QLocale::countryToString(tz.country()).contains(query, Qt::CaseInsensitive)) match = true;

            if (match) d->shownTimezones.append(tz);
        }
    }

    this->endResetModel();
}
