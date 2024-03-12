#ifndef STOPWATCHPANECONTROLLER_H
#define STOPWATCHPANECONTROLLER_H

#include <QCoroTask>
#include <QObject>
#include <QQmlEngine>

struct StopwatchPaneControllerPrivate;
class StopwatchPaneController : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged);
        Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
        Q_PROPERTY(QString displayString READ displayString NOTIFY displayStringChanged)
        QML_ELEMENT
    public:
        explicit StopwatchPaneController(QObject* parent = nullptr);
        ~StopwatchPaneController();

        QString objectPath();
        void setObjectPath(QString objectPath);

        bool paused();
        void setPaused(bool paused);

        QString displayString();

        Q_SCRIPTABLE void reset();
        Q_SCRIPTABLE void remove();

    signals:
        void objectPathChanged();
        void pausedChanged();
        void displayStringChanged();

    private slots:
        QCoro::Task<> updateState();

    private:
        StopwatchPaneControllerPrivate* d;
};

#endif // STOPWATCHPANECONTROLLER_H
