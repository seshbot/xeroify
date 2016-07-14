import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

ColumnLayout {
    property OAuth2Settings settings: OAuth2Settings { }

    GridLayout {
        columns: 2
        columnSpacing: 20
        rowSpacing: 20
        Label {
            text: "Client ID"
        }
        TextField {
            id: clientIdField
            Layout.fillWidth: true
            text: settings ? settings.clientId : ''
        }
        Label {
            text: "Client Secret"
        }
        TextField {
            id: clientSecretField
            Layout.fillWidth: true
            text: settings ? settings.clientSecret : ''
        }
        Button {
            text: "Clear Access Token"
            enabled: settings && settings.accessToken !== ''
            onClicked: settings.accessToken = ''
        }
    }

    function save() {
        settings.clientId = clientIdField.text
        settings.clientSecret = clientSecretField.text
    }

    function reset() {
        clientIdField.text = settings.clientId
        clientSecretField.text = settings.clientSecret
    }
}
