import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.vicr123.Contemporary
import Contemporary

Popup {
    id: root

    signal add(timezone: string)

    LayerCalculator {
        id: layer1
        layer: 1
    }

    Grandstand {
        id: grandstand
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        text: qsTr("Add Clock")
        color: layer1.color

        z: 20
    }

    TimezonesModel {
        id: timezonesModel
    }

    TextField {
        id: searchQuery
        placeholderText: qsTr("Search...")

        anchors.top: grandstand.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        onTextEdited: timezonesModel.search(searchQuery.text)
    }

    ListView {
        id: list
        anchors.top: searchQuery.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 6
        anchors.rightMargin: 6

        model: timezonesModel
        spacing: 3

        clip: true

        delegate: ItemDelegate {
            id: delegateRoot
            required property string timezoneId
            required property string timezoneOffset
            required property string timezoneName
            required property string timezoneLocation
            required property bool timezoneMarksNewOffset

            implicitWidth: list.width
            implicitHeight: offsetLabel.height

            contentItem: RowLayout {
                anchors.fill: parent

                Label {
                    id: offsetLabel
                    text: delegateRoot.timezoneOffset
                    color: delegateRoot.timezoneMarksNewOffset ? Contemporary.disabled(Contemporary.foreground) : "transparent"
                }

                Label {
                    id: locationLabel
                    text: timezoneLocation
                }

                Label {
                    id: nameLabel
                    text: delegateRoot.timezoneName
                    color: Contemporary.disabled(Contemporary.foreground)
                    Layout.fillWidth: true
                }
            }

            onClicked: () => {
                root.add(delegateRoot.timezoneId);
                root.close()
            }
        }
    }
}
