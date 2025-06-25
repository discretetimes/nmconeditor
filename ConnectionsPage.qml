import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    title: qsTr("Network Connections")

    // Component {
        // id: connectionsPage
        // ColumnLayout {
            // anchors.fill: parent
            // anchors.margins: 10

            // Label {
            //     text: "Available Wired Connections"
            //     font.bold: true
            // }

            // Button {
            //     text: "Manage Connections"
            //     Layout.alignment: Qt.AlignRight
            //     onClicked: stackView.push(DetailsPage)
            // }

            ListView {
                id: connectionsView
                // Layout.fillWidth: true
                // Layout.fillHeight: true
                // clip: true
                anchors.fill: parent
                model: networkModel
                delegate: ItemDelegate {
                    // height: 50
                    width: parent.width

                    Text {
                        text: "Name: " + model.name
                    }

                    onClicked: {
                        // make this delegate the current item
                        connectionsView.currentIndex = index
                        connectionsView.focus = true
                        stackView.push("DetailsPage.qml", { "connectionUuid": model.uuid })
                        // stackView.push("DetailsPage.qml")
                    }

                    // onRemove: {
                    //     // remove the current entry from the model
                    //     networkModel.remove(index)
                    // }
                }

            }

            Button {
                text: "Create Con"
                onClicked: stackView.push("DetailsPage.qml", { "connectionUuid": model.uuid })
                    // networkModel.createWiredConnection(connectionNameInput.text, interfaceNameInput.text)
            }
            // }
        // }
    // }
}
// }
    // Component.onCompleted: {
    //     networkModel.refresh()
    // }

    // Component {
    //     id: controlsPage
    //     ControlsPage {}
    // }

