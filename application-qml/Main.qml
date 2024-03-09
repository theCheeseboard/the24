import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary
import Qt.labs.platform as Labs
import worldclock as WorldClock
import alarm as Alarm

ContemporaryWindow {
    id: window

    height: 480
    overlayActionBar: true
    title: qsTr("the24")
    visible: true
    width: 640

    actionBar: ActionBar {
        control: window

        menuItems: [
            Menu {
                title: qsTr("Theme")

                Action {
                    text: qsTr("Light")
                }
                Action {
                    text: qsTr("Dark")
                }
            },
            MenuSeparator {
            },
            Action {
                shortcut: "Ctrl+Q"
                text: qsTr("Exit")

                onTriggered: window.close()
            }
        ]

        ActionBarTabber {
            ActionBarTabber.Button {
                text: qsTr("World Clock")
                icon.name: "clock-world"
                checked: stack.currentIndex === 0
                onActivated: stack.currentIndex = 0
            }
            ActionBarTabber.Button {
                text: qsTr("Alarms")
                icon.name: "clock-alarm"
                checked: stack.currentIndex === 1
                onActivated: stack.currentIndex = 1
            }
            ActionBarTabber.Button {
                text: qsTr("Stopwatch")
                icon.name: "clock-stopwatch"
                checked: stack.currentIndex === 2
                onActivated: stack.currentIndex = 2
            }
            ActionBarTabber.Button {
                text: qsTr("Timer")
                icon.name: "clock-timer"
                checked: stack.currentIndex === 3
                onActivated: stack.currentIndex = 3
            }
        }
    }

    Pager {
        id: stack
        currentAnimation: Pager.Animation.SlideHorizontal
        anchors.fill: parent

        WorldClock.WorldClocksPane { }
        Alarm.AlarmsPane { }
    }
}
