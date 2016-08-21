import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import BusyBot 1.0

ItemDelegate {
    property var lineItem

    RowLayout {
        anchors.fill: parent
        spacing: 15
        ColumnLayout {
            Layout.fillWidth: true
            Row {
                spacing: 6
                Label {
                    background: Rectangle {
                        color: 'black'
                        radius: 3
                    }
                    color: 'white'
                    leftPadding: 3
                    rightPadding: 3
                    text: lineItem.quantity
                }
                Text {
                    font.bold: true
                    elide: Text.ElideRight
                    fontSizeMode: Text.HorizontalFit
                    text: lineItem.title
                }
            }
            Label {
                color: 'green'
                elide: Text.ElideRight
                text: lineItem.variantTitle
            }
            Row {
                spacing: 6
                Label {
                    background: Rectangle {
                        color: 'darkgray'
                        radius: 3
                    }
                    color: 'white'
                    leftPadding: 3
                    rightPadding: 3
                    text: lineItem.quantity - lineItem.fulfillableQuantity
                }
                Text {
                    elide: Text.ElideRight
                    fontSizeMode: Text.HorizontalFit
                    color: 'darkgray'
                    text: ' fulfilled'
                }
            }
        }
        Label {
            Layout.alignment: Qt.AlignRight
            font.bold: true
            text: lineItem.price
        }
    }
}
