import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "NetworkManager Connection Editor"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        TextField {
            id: connectionNameInput
            placeholderText: "Enter Connection Name"
        }

        TextField {
            id: interfaceNameInput
            placeholderText: "Enter Interface Name"
        }

        Button {
            text: "Activate Wired Connection"
            onClicked: nmProxy.activateWiredConnection(connectionNameInput.text, interfaceNameInput.text)
        }

        Button {
            text: "Deactivate Wired Connection"
            onClicked: nmProxy.deactivateWiredConnection(connectionNameInput.text)
        }

        Button {
            text: "Create Manual Wired Connection"
            onClicked: nmProxy.createWiredConnection(connectionNameInput.text, interfaceNameInput.text)
        }

        Button {
            text: "Delete Connection"
            onClicked: nmProxy.deleteConnection(connectionNameInput.text)
        }

        Button {
            text: "Modify IPv4 to Auto"
            onClicked: nmProxy.modifyIpv4Setting(connectionNameInput.text)
        }
    }
}
