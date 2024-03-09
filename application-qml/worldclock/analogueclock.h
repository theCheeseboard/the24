#ifndef ANALOGUECLOCK_H
#define ANALOGUECLOCK_H

#include <QColor>
#include <QQuickPaintedItem>
#include <QTime>

struct AnalogueClockPrivate;
class AnalogueClock : public QQuickPaintedItem {
        Q_OBJECT
        Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged FINAL)
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged FINAL)
        Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged FINAL)
        QML_ELEMENT
    public:
        explicit AnalogueClock(QQuickItem* parent = nullptr);
        ~AnalogueClock();

        QTime time();
        void setTime(QTime time);

        QColor accentColor() const;
        void setAccentColor(const QColor& newAccentColor);

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor& newBackgroundColor);

    signals:
        void timeChanged();

        void accentColorChanged();

        void backgroundColorChanged();

    private:
        AnalogueClockPrivate* d;

    public:
        void paint(QPainter* painter);
};

#endif // ANALOGUECLOCK_H
