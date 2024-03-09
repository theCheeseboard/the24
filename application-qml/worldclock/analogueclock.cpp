#include "analogueclock.h"

#include <QPainter>

struct AnalogueClockPrivate {
        QTime time;
        QColor backgroundColor;
        QColor accentColor;
};

AnalogueClock::AnalogueClock(QQuickItem* parent) :
    QQuickPaintedItem(parent), d{new AnalogueClockPrivate()} {
}

AnalogueClock::~AnalogueClock() {
    delete d;
}

QTime AnalogueClock::time() {
    return d->time;
}

void AnalogueClock::setTime(QTime time) {
    d->time = time;
    emit timeChanged();

    this->update();
}

void AnalogueClock::paint(QPainter* painter) {
    QRect rect;
    rect.setSize(QSize(1, 1).scaled(this->width(), this->height(), Qt::KeepAspectRatio));
    rect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setWindow(-25, -25, 50, 50);
    painter->setPen(Qt::transparent);

    QFont font = painter->font();
    font.setPixelSize(8);
    QFontMetrics fontMetrics(font);

    // TODO: Fractional portions of the angles
    double hourHandAngle = static_cast<double>(d->time.msecsSinceStartOfDay()) / 43200000 * 360;
    double minuteHandAngle = static_cast<double>(d->time.msecsSinceStartOfDay()) / 3600000 * 360;
    double secondHandAngle = static_cast<double>(d->time.msecsSinceStartOfDay()) / 60000 * 360;
    bool isDarkClock = d->time.hour() < 6 || d->time.hour() >= 18;

    bool isDarkBackground = (d->backgroundColor.red() + d->backgroundColor.green() + d->backgroundColor.blue()) / 3 < 127;

    auto handCol = isDarkClock ? Qt::white : Qt::black;

    // Draw the clock circle
    if (isDarkClock) {
        if (isDarkBackground) {
            painter->setBrush(QColor(0x14, 0x14, 0x14));
        } else {
            painter->setBrush(QColor(0x28, 0x28, 0x28));
        }
    } else {
        if (isDarkBackground) {
            painter->setBrush(QColor(0xD2, 0xD2, 0xD2));
        } else {
            painter->setBrush(Qt::white);
        }
    }
    painter->drawEllipse(QRect(-25, -25, 50, 50));

    painter->setFont(font);
    for (QString number : QStringList({"12", "3", "6", "9"})) {
        int width = fontMetrics.horizontalAdvance(number);

        QRect textRect;
        textRect.setWidth(width);
        textRect.setHeight(fontMetrics.height());
        textRect.moveCenter(QPoint(0, 0));

        if (number == "12") {
            textRect.moveTop(-20);
        } else if (number == "3") {
            textRect.moveRight(20);
        } else if (number == "6") {
            textRect.moveBottom(20);
        } else { // number == 9
            textRect.moveLeft(-20);
        }

        painter->save();
        painter->setPen(handCol);
        painter->drawText(textRect, Qt::AlignCenter, number);
        painter->restore();
    }

    // Draw the second hand
    painter->save();
    painter->rotate(secondHandAngle);
    painter->setPen(d->accentColor);
    painter->drawLine(0, -15, 0, 0);
    painter->restore();

    painter->setPen(handCol);

    // Draw the minute hand
    painter->save();
    painter->rotate(minuteHandAngle);
    painter->drawLine(0, -19, 0, 0);
    painter->restore();

    // Draw the hour hand
    painter->save();
    painter->rotate(hourHandAngle);
    painter->drawLine(0, -10, 0, 0);
    painter->restore();
}

QColor AnalogueClock::accentColor() const {
    return d->accentColor;
}

void AnalogueClock::setAccentColor(const QColor& newAccentColor) {
    d->accentColor = newAccentColor;
    emit accentColorChanged();
    this->update();
}

QColor AnalogueClock::backgroundColor() const {
    return d->backgroundColor;
}

void AnalogueClock::setBackgroundColor(const QColor& newBackgroundColor) {
    d->backgroundColor = newBackgroundColor;
    emit backgroundColorChanged();
    this->update();
}
