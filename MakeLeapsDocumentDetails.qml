import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

Pane {
    property ApiObject document

    GridLayout {
        anchors.fill: parent
        columns: 2
        Label {
            anchors.centerIn: parent
            text: 'Document: ' + document.name
        }
    }
}
