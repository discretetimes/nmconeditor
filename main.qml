import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "NetworkManager Connection Editor"

    // StackView {
    //     id: stackView
    //     initialItem: connectionsPage
    // }

    // Component {
        id: connectionsPage
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            Label {
                text: "Available Wired Connections"
                font.bold: true
            }

            Button {
                text: "Manage Connections"
                Layout.alignment: Qt.AlignRight
                onClicked: stackView.push(controlsPage)
            }

            ListView {
                id: connectionsView
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: networkModel
                delegate: ItemDelegate {
                    height: 50
                    Text {
                        text: "Name: " + model.name
                    }
                }
            }
        }
    // }

    // Component.onCompleted: {
    //     networkModel.refresh()
    // }

    // Component {
    //     id: controlsPage
    //     ControlsPage {}
    // }
}
