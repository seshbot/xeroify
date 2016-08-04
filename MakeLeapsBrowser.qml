import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import BusyBot 1.0

Page {
    property MakeLeaps api

    Loader {
        anchors.fill: parent

        Settings {
            id: settings
            property string currentPartner
        }
    //    Component.onCompleted: {
    //        //api.c
    //    }
        Component.onDestruction: {
            settings.currentPartner = 'xxx'
        }

        Popup {
            id: changePartner
            modal: true
            width: 100
            height: 100
            ListView {
                Layout.fillHeight: true
                Layout.fillWidth: true
                header: Label {
                    width: parent.width
                    horizontalAlignment: Label.AlignHCenter
                    text: settings.currentPartner
                }
                model: api.partners
                delegate: ItemDelegate {
                    text: modelData.name
                }
            }
        }

        Component {
            id: loadingPage
            Label {
                anchors.centerIn: parent
                text: qsTr('Loading...')
            }
        }

        Component {
            id: selectPartnerPage
            ListView {
                anchors.fill: parent
                header: Label {
                    width: parent.width
                    horizontalAlignment: Label.AlignHCenter
                    text: qsTr('Select a partner')
                }
                model: api.partners
                delegate: ItemDelegate {
                    text: modelData.name
                    onClicked: {
                        settings.currentPartner = modelData.name
                    }
                }
            }

        }

        Component {
            id: partnerPage
            RowLayout {
                id: partnerRow
                anchors.fill: parent

                property MakeLeapsPartner partner: settings.currentPartner ? api.partner( settings.currentPartner ) : null
                property MakeLeapsEndpoint section

                onPartnerChanged: {
                    console.log('partner changed')
                }

                onSectionChanged: {
                    console.log('section changed:', section ? section.url : '-')
                }

                MakeLeapsNavBar {
                    Layout.fillHeight: true
                    Layout.minimumWidth: 150

                    partner: partnerRow.partner

                    onResourceSelected: {
                        console.log( qsTr('%1 resource selected: %2').arg(type).arg(resource.name) )
                        if ( type === 'contacts' ) {}
                        else if ( type === 'clients' ) {}
                        else if ( type === 'documents' ) {}
                    }
                }

                Page {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    header: Label {
                        width: parent.width
                        horizontalAlignment: Label.AlignHCenter
                        text: settings.currentPartner
                    }
                    StackView {
                        id: detailsView
                        initialItem: Label {
                            //anchors.centerIn: parent
                            text: 'select something'
                        }

                        Component {
                            id: documentsDetails
                            Pane {
                                Label {
                                    anchors.centerIn: parent
                                    text: 'documents selected'
                                }
                            }
                        }
                        Component {
                            id: clientsDetails
                            Pane {
                                Label {
                                    anchors.centerIn: parent
                                    text: 'clients selected'
                                }
                            }
                        }
                        Component {
                            id: contactsDetails
                            Pane {
                                Label {
                                    anchors.centerIn: parent
                                    text: 'contacts selected'
                                }
                            }
                        }
                    }
                }
            }
        } // partner page

        sourceComponent: {
            if ( !api || api.state !== MakeLeaps.STATE_IDLE ) return loadingPage
            if ( !settings.currentPartner || !api.hasPartner(settings.currentPartner) ) return selectPartnerPage
            return partnerPage
        }
    }
}
