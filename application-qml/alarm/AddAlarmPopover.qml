import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Drawer {
    id: root
    interactive: false

    property string objectPath

    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        text: root.objectPath === "" ? qsTr("New Alarm") : qsTr("Edit Alarm")
        backButtonVisible: true
        onBackButtonClicked: root.visible = false;
    }

    AlarmPaneController {
        id: itemController
        objectPath: root.objectPath
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
            title: qsTr("Alarm Time")
            implicitWidth: 600

            Label {
                text: "Alarm Time"
            }
        }

        GroupBox {
            Layout.alignment: Qt.AlignHCenter
            title: qsTr("Repeat on")
            implicitWidth: 600

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                color: Contemporary.accent
                radius: 4

                implicitHeight: childrenRect.height

                RowLayout {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: childrenRect.height
                    spacing: 2

                    Repeater {
                        model: 7

                        Button {
                            id: dayButton
                            required property int index

                            Layout.fillWidth: true

                            text: Qt.locale().dayName((Qt.locale().firstDayOfWeek + index) % 7, Locale.NarrowFormat)
                            checkable: true

                            // firstDayOfWeek returns 0 for Sunday, but is represented by 0x40 in itemController
                            // Add 6 to displace every day by 1 so that Sunday becomes day #6
                            checked: itemController.repeat & Math.pow(2, (Qt.locale().firstDayOfWeek + index + 6) % 7)
                            onCheckedChanged: itemController.setRepeatDay((Qt.locale().firstDayOfWeek + index + 6) % 7, dayButton.checked)
                            implicitWidth: 2
                        }
                    }
                }
            }

        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 600
            text: qsTr("Add Alarm")
            icon.name: "list-add"
            visible: root.objectPath === ""
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
