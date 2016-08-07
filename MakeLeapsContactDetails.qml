import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Flickable {
    property ApiObject contact

    function createEmailLink(email) {
        return '<a href="mailto:' + email + '">' + email + '</a>'
    }

    GridLayout {
        columns: 2

        Label {
            text: qsTr('Name')
        }
        Label {
            text: contact.name
        }

        Label {
            text: qsTr('Email')
        }
        Text {
            text: {
                var object = contact.property('email').asObject
                var email = object ? object.property('address').asScalar : ''
                return email && createEmailLink( email )
            }
            onLinkActivated: Qt.openUrlExternally(link)
        }

        Label {
            text: qsTr('Title')
        }
        Label {
            text: contact.property('title').asScalar
        }

        Label {
            text: qsTr('Department')
        }
        Label {
            text: contact.property('department').asScalar
        }

        Label {
            text: qsTr('Phone Number')
        }
        Label {
            text: contact.property('phone_number').asScalar
        }

        Label {
            text: qsTr('Default Address')
        }
        Text {
            property ApiObject defaultAddress: contact.property('default_address').asObject
            text: defaultAddress ? defaultAddress.property('formatted_html').asScalar : '-'
        }

        Label {
            text: qsTr('Addresses')
        }
        Pane {
            ListView {
                anchors.fill: parent
                model: contact.property('addresses').asArray
                delegate: ItemDelegate {
                    text: modelData.asObject.property('display').asScalar
                }
            }
        }
    }
}
