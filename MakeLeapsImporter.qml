import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 1.4 as OldControls
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import BusyBot 1.0

Page {
    id: root
    property Shopify shopify
    property MakeLeaps makeLeaps
    property date startDate: settings.lastImport
    property date endDate: new Date()

    Settings {
        id: settings
        property string currentPartner
        property date lastImport: new Date()
    }

    Popup {
        id: datePicker
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        modal: true

        property string title
        property date selectedDate
        property var selectionHandler

        ColumnLayout {
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Label.AlignHCenter
                font.bold: true
                text: datePicker.title
            }

            OldControls.Calendar {
                id: calendar
                selectedDate: datePicker.selectedDate
                maximumDate: new Date()
                onClicked: {
                    datePicker.selectionHandler( selectedDate )
                    datePicker.close()
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Button {
                    text: qsTr('Today')
                    onClicked: {
                        calendar.selectedDate = new Date()
                    }
                }
                Button {
                    text: qsTr('Cancel')
                    onClicked: {
                        datePicker.close()
                    }
                }
            }
        }
    }

//    updated_at_min
//    Show orders last updated after date (format: 2014-04-25T16:15:47-04:00)
//    updated_at_max
//    Show orders last updated before date (format: 2014-04-25T16:15:47-04:00)

    // main row layout
    RowLayout {
        // filter column
        GridLayout {
            Layout.fillHeight: true
            columns: 3
            columnSpacing: 6
            rowSpacing: 6

            Label {
                Layout.preferredWidth: 50
                Layout.alignment: Qt.AlignRight
                horizontalAlignment: Label.AlignRight
                color: 'darkgray'
                text: qsTr('Type')
            }
            ComboBox {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                model: [ qsTr('Clients'), qsTr('Invoices') ]
            }

            Label {
                Layout.alignment: Qt.AlignRight
                color: 'darkgray'
                text: qsTr('Start')
            }
            Label {
                text: startDate.toDateString()
            }
            Button {
                text: qsTr('change')
                onClicked: {
                    datePicker.title = qsTr('Start Date')
                    datePicker.selectedDate = root.startDate
                    datePicker.selectionHandler = function(d) { root.startDate = d }
                    datePicker.open()
                }
            }

            Label {
                Layout.alignment: Qt.AlignRight
                color: 'darkgray'
                text: qsTr('End')
            }
            Label {
                text: endDate.toDateString()
            }
            Button {
                text: qsTr('change')
                onClicked: {
                    datePicker.title = qsTr('End Date')
                    datePicker.selectedDate = root.endDate
                    datePicker.selectionHandler = function(d) { root.endDate = d }
                    datePicker.open()
                }
            }

            Button {
                Layout.columnSpan: 3
                Layout.alignment: Qt.AlignRight
                text: qsTr('Apply')
            }
        } // filter column layout

        Pane {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                anchors.centerIn: parent
                text: 'details'
            }
        }
    } // main row layout
}
