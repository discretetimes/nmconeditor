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
                delegate: ListDelegate {
                    height: 50

                    Text {
                        text: "Name: " + model.name
                    }
                    // onClicked: {
                    //     // make this delegate the current item
                    //     // connectionsView.currentIndex = index
                    //     connectionsView.focus = true
                    // }

                    onRemove: {
                        // remove the current entry from the model
                        networkModel.remove(index)
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
