import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Drawer {
    id: root
    interactive: false

    property string objectPath
    readonly property bool is24hour: !Qt.locale().timeFormat(Locale.ShortFormat).includes("AP")

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
            id: offsetGroup
            Layout.alignment: Qt.AlignHCenter
            title: qsTr("Alarm Time")
            implicitWidth: 600

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 6

                Row {
                    LayoutMirroring.enabled: false
                    spacing: 6

                    Tumbler {
                        id: offsetHour
                        model: root.is24hour ? 24 : 12
                        font.pointSize: 15
                        delegate: Label {
                            text: Qt.locale().toString(index == 0 && !root.is24hour ? 12 : index).padStart(2, Qt.locale().zeroDigit)
                            opacity: 1.0 - Math.abs(Tumbler.displacement) / (offsetHour.visibleItemCount / 2)
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            required property int index
                        }
                        currentIndex: itemController.offsetHour % (root.is24hour ? 24 : 12)
                        onMovingChanged: offsetGroup.updateOffset()
                        onCurrentIndexChanged: offsetGroup.updateOffset()
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
                        currentIndex: itemController.offsetMinute
                        onMovingChanged: offsetGroup.updateOffset()
                        onCurrentIndexChanged: offsetGroup.updateOffset()
                    }
                }
                Tumbler {
                    id: offsetAmPm
                    model: [Qt.locale().amText, Qt.locale().pmText]
                    font.pointSize: 15
                    visible: !root.is24hour
                    currentIndex: itemController.offsetHour >= 12 ? 1 : 0
                    onMovingChanged: offsetGroup.updateOffset()
                    onCurrentIndexChanged: offsetGroup.updateOffset()
                }
            }

            function updateOffset() {
                if (offsetAmPm.moving || offsetHour.moving || offsetMinute.moving) return;

                itemController.offsetMinute = offsetMinute.currentIndex
                if (root.is24hour) {
                    itemController.offsetHour = offsetHour.currentIndex;
                } else {
                    itemController.offsetHour = offsetHour.currentIndex + offsetAmPm.currentIndex * 12;
                }
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

                            // firstDayOfWeek returns 0 for Sunday, but is represented by 0x40 (2^6) in itemController
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

            onClicked: () => {
                itemController.addAsNewAlarm()
                root.visible = false
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
