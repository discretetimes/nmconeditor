import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    property string connectionUuid
    property var connectionDetails: ({})

    title: "Edit Addresses for " + (connectionDetails.name || "")

    ListModel {
        id: addressModel
    }

    Component.onCompleted: {
        connectionDetails = networkModel.getConnectionDetails(connectionUuid)
        if (connectionDetails.ipv4Method === 0) {
            methodComboBox.currentIndex = 0
        } else {
            methodComboBox.currentIndex = 1
        }
        for (var i = 0; i < connectionDetails.addresses.length; ++i) {
            addressModel.append({ "address": connectionDetails.addresses[i] })
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        Label { text: "IPv4 Method:" }
        ComboBox {
            id: methodComboBox
            Layout.fillWidth: true
            model: ["Automatic (DHCP)", "Manual"]
        }

        ListView {
            id: addressListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: addressModel
            currentIndex: -1
            enabled: methodComboBox.currentIndex === 1

            delegate: ItemDelegate {
                width: parent.width
                highlighted: ListView.isCurrentItem

                // --- FIX START: Explicit MouseArea and Focus Handling ---
                TextField {
                    id: addressField
                    anchors.fill: parent
                    text: model.address
                    placeholderText: parent.enabled ? "e.g., 192.168.1.100/24" : "Not applicable"
                    background: null
                    readOnly: !parent.enabled
                    // When the user is done typing, update the model
                    onEditingFinished: {
                        model.address = text
                    }
                }

                // This MouseArea overlays the whole delegate
                MouseArea {
                    anchors.fill: parent
                    // This ensures clicks are not propagated further up,
                    // which is good practice here.
                    preventStealing: true

                    onClicked: {
                        // 1. Set the ListView's current index. This is the key fix.
                        addressListView.currentIndex = index

                        // 2. Give the TextField focus so the user can type.
                        addressField.forceActiveFocus()
                    }
                }
                // --- FIX END ---
            }
        }

        RowLayout {
            enabled: methodComboBox.currentIndex === 1
            Button {
                text: qsTr("Add Address")
                onClicked: {
                    addressModel.append({ "address": "" })
                }
            }
            Button {
                text: qsTr("Remove Selected")
                enabled: addressListView.currentIndex !== -1
                onClicked: {
                    if (addressListView.currentIndex !== -1) {
                        addressModel.remove(addressListView.currentIndex)
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel
        onAccepted: {
            var newAddresses = []
            if(methodComboBox.currentIndex === 1) {
                for (var i = 0; i < addressModel.count; i++) {
                    var item = addressModel.get(i)
                    if (item.address !== "") {
                        newAddresses.push(item.address)
                    }
                }
            }
            var newMethod;
            if (methodComboBox.currentIndex === 0) {
                newMethod = 0;
            } else {
                newMethod = 2;
            }
            var newSettings = {
                "ipv4Method": newMethod,
                "addresses": newAddresses
            }
            networkModel.updateConnection(connectionUuid, newSettings)
            stackView.pop()
        }
        onRejected: {
            stackView.pop()
        }
    }
}
