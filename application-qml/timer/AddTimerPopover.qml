import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Drawer {
    id: root
    interactive: false

    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        text: qsTr("New Timer")
        backButtonVisible: true
        onBackButtonClicked: root.visible = false;
    }

    TimerPaneController {
        id: itemController
        objectPath: root.objectPath
    }

    function addTimer(msecs) {
        itemController.addTimer(msecs);
        root.visible = false
    }

    ColumnLayout {
        anchors.top: grandstand.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 9

        spacing: 6

        GroupBox {
            Layout.alignment: Qt.AlignHCenter
            title: qsTr("Presets")
            implicitWidth: 600

            Rectangle {
                color: Contemporary.accent
                radius: 4
                implicitHeight: childrenRect.height
                anchors.left: parent.left
                anchors.right: parent.right

                GridLayout {
                    columns: 4
                    columnSpacing: 2
                    rowSpacing: 2
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("1 min");
                        onClicked: addTimer(60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("2 min");
                        onClicked: addTimer(2 * 60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("5 min");
                        onClicked: addTimer(5 * 60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("10 min");
                        onClicked: addTimer(10 * 60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("15 min");
                        onClicked: addTimer(15 * 60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("30 min");
                        onClicked: addTimer(30 * 60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("1 hr");
                        onClicked: addTimer(60 * 60 * 1000)
                    }
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("2 hr");
                        onClicked: addTimer(2 * 60 * 60 * 1000)
                    }
                }
            }
        }

        GroupBox {
            id: offsetGroup
            Layout.alignment: Qt.AlignHCenter
            title: qsTr("Custom Timer Duration")
            implicitWidth: 600

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 6

                Tumbler {
                    id: offsetHour
                    model: 100
                    font.pointSize: 15
                    delegate: Label {
                        text: Qt.locale().toString(index).padStart(2, Qt.locale().zeroDigit)
                        opacity: 1.0 - Math.abs(Tumbler.displacement) / (offsetHour.visibleItemCount / 2)
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        required property int index
                    }
                    currentIndex: 0
                }
                Tumbler {
                    id: offsetMinute
                    model: 60
                    font.pointSize: 15
                    delegate: Label {
                        text: Qt.locale().toString(index).padStart(2, Qt.locale().zeroDigit)
                        opacity: 1.0 - Math.abs(Tumbler.displacement) / (offsetMinute.visibleItemCount / 2)
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        required property int index
                    }
                    currentIndex: 5
                }
                Tumbler {
                    id: offsetSecond
                    model: 60
                    font.pointSize: 15
                    delegate: Label {
                        text: Qt.locale().toString(index).padStart(2, Qt.locale().zeroDigit)
                        opacity: 1.0 - Math.abs(Tumbler.displacement) / (offsetMinute.visibleItemCount / 2)
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        required property int index
                    }
                    currentIndex: 0
                }
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 600
            text: qsTr("Add Timer")
            icon.name: "list-add"

            onClicked: () => {
                const minutes = offsetHour.currentIndex * 60 + offsetMinute.currentIndex;
                const seconds = minutes * 60 + offsetSecond.currentIndex;
                itemController.addTimer(seconds * 1000);
                root.visible = false
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
