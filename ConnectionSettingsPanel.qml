import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

ColumnLayout {
    property ConnectionSettings settings: ConnectionSettings { }

    GridLayout {
        columns: 2
        columnSpacing: 20
        rowSpacing: 20
        Label {
            text: "URL"
        }
        TextField {
            id: urlField
            Layout.fillWidth: true
            text: settings ? settings.url : ''
        }
        Label {
            text: "API Key"
        }
        TextField {
            id: apiKeyField
            Layout.fillWidth: true
            text: settings ? settings.apiKey : ''
        }
        Label {
            text: "Password"
        }
        TextField {
            id: passwordField
            Layout.fillWidth: true
            text: settings ? settings.password : ''
        }
    }

    function save() {
        settings.url = urlField.text
        settings.apiKey = apiKeyField.text
        settings.password = passwordField.text
    }

    function reset() {
        urlField.text = settings.url
        apiKeyField.text = settings.apiKey
        passwordField.text = settings.password
    }
}
