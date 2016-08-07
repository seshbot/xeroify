import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import BusyBot 1.0

Page {
    id: root
    property MakeLeaps api

    Settings {
        id: settings
        property string currentPartner
    }

    // property popup
    Popup {
        id: resourceResponseComponent

        property ApiProperty propertyToShow

        modal: true
        width: ( parent.width / 5 ) * 4
        height: ( parent.height / 6 ) * 5
        x: ( parent.width - width ) / 2

        Page {
            anchors.fill: parent
            header: Label {
                horizontalAlignment: Label.AlignHCenter
                font.bold: true
                text: resourceResponseComponent.propertyToShow.name
            }

            ColumnLayout {
                anchors.fill: parent

                ApiPropertyPage {
                    id: propertyPage

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    currentProperty: resourceResponseComponent.propertyToShow
                }
            }
        }
    } // property popup

    Loader {
        anchors.fill: parent

        // loading component
        Component {
            id: loadingComponent
            Label {
                anchors.centerIn: parent
                text: qsTr('Loading...')
            }
        }

        // 'select partner' component
        Component {
            id: selectPartnerComponent
            ListView {
                anchors.fill: parent
                header: Label {
                    width: parent.width
                    horizontalAlignment: Label.AlignHCenter
                    text: ( api && api.partners.length ) ? qsTr('Select a partner') : qsTr('Loading partner information...')
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

        // partner loaded page
        Component {
            id: partnerLoadedComponent
            RowLayout {
                id: partnerRow
                anchors.fill: parent

                property MakeLeapsPartner partner: settings.currentPartner ? api.partner( settings.currentPartner ) : null
                property MakeLeapsEndpoint section

                // nav bar
                MakeLeapsNavBar {
                    Layout.fillHeight: true
                    Layout.minimumWidth: root.width / 3

                    partner: partnerRow.partner

                    onResourceSelected: {
                        if (!resource.url) {
                            console.log( qsTr('selected resource %1 (%2) does not contain url').arg(resource.name).arg(type) )
                            return
                        }
                        console.log( qsTr('%1 resource selected: %2').arg(type).arg(resource.name) )

                        resourceDetailsPage.resource = resource
                        if ( type === 'contacts' ) { resourceDetailsPage.content = contactComponent }
                        else if ( type === 'clients' ) { resourceDetailsPage.content = clientComponent }
                        else if ( type === 'documents' ) { resourceDetailsPage.content = documentComponent }
                    }
                }

                // details page
                MakeLeapsResource {
                    id: resourceDetailsPage
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    property ApiObject resource
                    endpoint: resource && resource.property('url').asEndpoint

                    onShowApiPropertyButtonClicked: {
                        resourceResponseComponent.propertyToShow = prop
                        resourceResponseComponent.open()
                    }

                    onNewResourceButtonClicked: {
                        //endpoint.deleteResource()
                    }

                    onDeleteResourceButtonClicked: {
                        endpoint.deleteResource()
                    }

                    // contact page
                    Component {
                        id: contactComponent
                        MakeLeapsContactDetails {
                            //anchors.fill: parent
                            contact: resourceDetailsPage.resource
                        }
                    }

                    // client page
                    Component {
                        id: clientComponent
                        MakeLeapsClientDetails {
                            anchors.fill: parent
                            client: resourceDetailsPage.resource
                        }
                    }

                    // document page
                    Component {
                        id: documentComponent
                        MakeLeapsDocumentDetails {
                            anchors.fill: parent
                            document: resourceDetailsPage.resource
                        }
                    }

                    content: Label {
                        //anchors.fill: parent
                        background: Rectangle {
                            color: 'darkgray'
                        }
                        text: ''
                    }
                }
            }
        } // partner page

        sourceComponent: {
            if ( !api || api.state !== MakeLeaps.STATE_IDLE ) return loadingComponent
            if ( !settings.currentPartner || !api.hasPartner(settings.currentPartner) ) return selectPartnerComponent
            return partnerLoadedComponent
        }
    }
}
