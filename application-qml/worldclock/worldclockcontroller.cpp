#include "worldclockcontroller.h"

#include "worldclock.h"

#include <ranges/trange.h>
#include <tsettings.h>

struct WorldClockControllerPrivate {
        tSettings settings;
        QStringList worldClocks;
};

WorldClockController::WorldClockController(QObject* parent) :
    QAbstractListModel{parent}, d{new WorldClockControllerPrivate()} {
    connect(&d->settings, &tSettings::settingChanged, this, [=](QString key) {
        if (key == "WorldClock/timezones") {
            updateClocks();
        }
    });

    d->worldClocks = d->settings.delimitedList("WorldClock/timezones");
    d->worldClocks.prepend(QTimeZone::systemTimeZone().id());
}

WorldClockController::~WorldClockController() {
    delete d;
}

void WorldClockController::updateClocks() {
    // Get the new list of world clocks
    QStringList newWorldClocks = d->settings.delimitedList("WorldClock/timezones");
    newWorldClocks.prepend(QTimeZone::systemTimeZone().id());

    // Prepare a list to keep track of removed indices
    QList<int> indicesToRemove;

    // Iterate over the current world clocks
    for (int i = 0; i < d->worldClocks.size(); ++i) {
        // If a clock is not found in the new list, mark its index for removal
        if (!newWorldClocks.contains(d->worldClocks.at(i))) {
            indicesToRemove.append(i);
        }
    }

    // Sort and reverse the list of indices so we can remove from the end
    std::ranges::sort(indicesToRemove, std::greater());

    for (int idx : indicesToRemove) {
        // Remove each index individually
        beginRemoveRows(QModelIndex(), idx, idx);
        d->worldClocks.removeAt(idx);
        endRemoveRows();
    }

    // Iterate over the new world clocks
    for (int i = 0; i < newWorldClocks.size(); ++i) {
        // If a clock is not found in the current list, mark its index for addition
        if (!d->worldClocks.contains(newWorldClocks.at(i))) {
            // Add each item individually
            beginInsertRows(QModelIndex(), i, i);
            d->worldClocks.insert(i, newWorldClocks.at(i));
            endInsertRows();
        }
    }
}

int WorldClockController::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return d->worldClocks.size();
}

QVariant WorldClockController::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount({})) {
        return {};
    }

    auto wc = d->worldClocks.at(index.row());

    switch (role) {
        case TimezoneRole:
            return wc;
        case RemovableRole:
            return index.row() != 0;
        default:
            return {};
    }
}

QHash<int, QByteArray> WorldClockController::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TimezoneRole] = "timezone";
    roles[RemovableRole] = "removable";
    return roles;
}

void WorldClockController::removeClock(QString timezone) {
    QStringList clocks = d->settings.delimitedList("WorldClock/timezones");
    clocks.removeAll(timezone);
    d->settings.setDelimitedList("WorldClock/timezones", clocks);
}
