import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

StackView {
    id: navBar

    property MakeLeapsPartner partner

    signal resourceSelected(string type, ApiObject resource)

    Component {
        id: endpointNavComponent
        Pane {
            id: endpointLoader

            property string type
            property MakeLeapsEndpoint endpoint

            onEndpointChanged: {
                if ( endpoint && endpoint.state === MakeLeapsEndpoint.STATE_IDLE )
                    endpoint.getResource()
            }

            Loader {
                anchors.fill: parent
                Component {
                    id: endpointLoadingComponent
                    Label {
                        anchors.centerIn: parent
                        text: 'loading...'
                    }
                }
                Component {
                    id: endpointChildrenComponent
                    ListView {
                        id: endpointChildren
                        model: endpoint.rootProperty.asArray
                        delegate: ItemDelegate {
                            width: endpointChildren.width
                            text: modelData.name
                            onClicked: {
                                resourceSelected( type, modelData.asObject )
                            }
                        }

                        ScrollIndicator.vertical: ScrollIndicator { }
                    }
                }
                sourceComponent: {
                    if (!endpoint || endpoint.state !== MakeLeapsEndpoint.STATE_LOADED)
                        return endpointLoadingComponent
                    return endpointChildrenComponent
                }
            }
        }
    }
    Component {
        id: contactsNav
        Label {
            text: 'contacts'
        }
    }
    Component {
        id: clientsNav
        Label {
            text: 'clients'
        }
    }
    Component {
        id: documentsNav
        Label {
            text: 'documents'
        }
    }

    initialItem: Loader {
        Component {
            id: loadedPage
            ListView {
                id: partnerSections
                anchors.fill: parent
                model: ListModel {
                    ListElement { name: 'Contacts'; path: 'contacts' }
                    ListElement { name: 'Clients'; path: 'clients' }
                    ListElement { name: 'Documents'; path: 'documents' }
                }
                delegate: ItemDelegate {
                    width: partnerSections.width
                    text: model.name
                    onClicked: {
                        console.log('click:', model.path, partner)
                        if ( model.path === 'contacts' ) navBar.push( endpointNavComponent, { type: 'contacts', endpoint: partner.contacts } )
                        else if ( model.path === 'clients' ) navBar.push( endpointNavComponent, { type: 'clients', endpoint: partner.clients } )
                        else if ( model.path === 'documents' ) navBar.push( endpointNavComponent, { type: 'documents', endpoint: partner.documents } )
                    }
                }

                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
        Component {
            id: loadingPage
            Label {
                anchors.centerIn: parent
                text: 'loading'
            }
        }
        Component {
            id: nullPage
            Label {
                text: 'null'
            }
        }
        Component {
            id: errorPage
            Label {
                text: 'error'
            }
        }
        sourceComponent: {
            if ( !partner ) return nullPage
            if ( partner.state === MakeLeapsPartner.STATE_LOADING ) return loadingPage
            if ( partner.state === MakeLeapsPartner.STATE_LOADED ) return loadedPage
            return errorPage
        }
    }
}
