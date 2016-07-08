import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

ColumnLayout {
    property OAuthZeroLeggedConnectionSettings settings: OAuthZeroLeggedConnectionSettings { }

    GridLayout {
        columns: 2
        columnSpacing: 20
        rowSpacing: 20
        Label {
            text: "Consumer Key"
        }
        TextField {
            id: consumerKeyField
            Layout.fillWidth: true
            text: settings ? settings.consumerKey : ''
        }
        Label {
            text: "Private Key"
        }
        Flickable {
            Layout.fillWidth: true
            height: 120
            TextArea.flickable: TextArea {
                id: rsaPrivateKeyField
                text: settings ? settings.rsaPrivateKey : ''
            }
            ScrollBar.vertical: ScrollBar { }
        }
    }

    function save() {
        console.log('saving consumer key ' + consumerKeyField.text)
        settings.consumerKey = consumerKeyField.text
        settings.rsaPrivateKey = rsaPrivateKeyField.text
    }

    function reset() {
        consumerKeyField.text = settings.consumerKey
        rsaPrivateKeyField.text = settings.rsaPrivateKey
    }
}
