/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "analogueclock.h"
#include "ui_analogueclock.h"

#include <QPainter>
#include <QTime>
#include <libcontemporary_global.h>

struct AnalogueClockPrivate {
        QTime time;
};

AnalogueClock::AnalogueClock(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AnalogueClock) {
    ui->setupUi(this);
    d = new AnalogueClockPrivate();

    d->time = QTime(0, 0, 0);
}

AnalogueClock::~AnalogueClock() {
    delete ui;
}

QSize AnalogueClock::sizeHint() const {
    return QSize(10, 10);
}

void AnalogueClock::setTime(QTime time) {
    d->time = time;
    this->update();
}

void AnalogueClock::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QRect rect;
    rect.setSize(QSize(1, 1).scaled(this->width(), this->height(), Qt::KeepAspectRatio));
    rect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setWindow(-25, -25, 50, 50);
    p.setPen(Qt::transparent);

    QFont font = this->font();
    font.setPixelSize(8);
    QFontMetrics fontMetrics(font);

    // TODO: Fractional portions of the angles
    double hourHandAngle = static_cast<double>(d->time.msecsSinceStartOfDay()) / 43200000 * 360;
    double minuteHandAngle = static_cast<double>(d->time.msecsSinceStartOfDay()) / 3600000 * 360;
    double secondHandAngle = static_cast<double>(d->time.msecsSinceStartOfDay()) / 60000 * 360;
    bool isDarkClock = d->time.hour() < 6 || d->time.hour() >= 18;

    QColor backgroundCol = this->palette().color(QPalette::Window);
    bool isDarkBackground = (backgroundCol.red() + backgroundCol.green() + backgroundCol.blue()) / 3 < 127;

    QColor accentCol = this->palette().color(QPalette::Highlight);
    QColor handCol = isDarkClock ? Qt::white : Qt::black;

    // Draw the clock circle
    if (isDarkClock) {
        if (isDarkBackground) {
            p.setBrush(QColor(0x14, 0x14, 0x14));
        } else {
            p.setBrush(QColor(0x28, 0x28, 0x28));
        }
    } else {
        if (isDarkBackground) {
            p.setBrush(QColor(0xD2, 0xD2, 0xD2));
        } else {
            p.setBrush(Qt::white);
        }
    }
    p.drawEllipse(QRect(-25, -25, 50, 50));

    p.setFont(font);
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

        p.save();
        p.setPen(handCol);
        p.drawText(textRect, Qt::AlignCenter, number);
        p.restore();
    }

    // Draw the second hand
    p.save();
    p.rotate(secondHandAngle);
    p.setPen(accentCol);
    p.drawLine(0, -15, 0, 0);
    p.restore();

    p.setPen(handCol);

    // Draw the minute hand
    p.save();
    p.rotate(minuteHandAngle);
    p.drawLine(0, -19, 0, 0);
    p.restore();

    // Draw the hour hand
    p.save();
    p.rotate(hourHandAngle);
    p.drawLine(0, -10, 0, 0);
    p.restore();
}

void AnalogueClock::resizeEvent(QResizeEvent* event) {
    this->setFixedWidth(this->height());
}
