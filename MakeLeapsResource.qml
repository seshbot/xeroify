import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

StackView {
    id: root
    property MakeLeapsEndpoint endpoint
    property Component content

    signal newResourceButtonClicked()
    signal deleteResourceButtonClicked()
    signal showApiPropertyButtonClicked(ApiProperty prop)

    Component.onCompleted: {
        if ( endpoint && endpoint.state === MakeLeapsEndpoint.STATE_IDLE ) {
            endpoint.getResource()
        }
    }

    onEndpointChanged: {
        if ( endpoint && endpoint.state === MakeLeapsEndpoint.STATE_IDLE ) {
            endpoint.getResource()
        }
    }

    // resource details page
    initialItem: Page {
        header: Loader {
            Component {
                id: loadedToolbarComponent

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }
                    Button {
                        enabled: !!endpoint
                        text: 'Response'
                        onClicked: {
                            if ( root.endpoint.state === MakeLeapsEndpoint.STATE_IDLE ) {
                                root.endpoint.getResource()
                            }

                            showApiPropertyButtonClicked(root.endpoint.rootProperty)
                            resourceResponseComponent.open()
                        }
                    }
                    Button {
                        enabled: !!endpoint && false
                        text: 'Edit'
                    }
                    Button {
                        enabled: !!endpoint
                        text: 'Delete'
                        onClicked: deleteResourceButtonClicked()
                    }
                }
            } // loaded toolbar
            Component {
                id: loadingToolbarComponent
                Text {
                    horizontalAlignment: Text.AlignHCenter
                    text: if ( endpoint && endpoint.state !== MakeLeapsEndpoint.STATE_LOADING ) {
                              return endpoint.stateString
                          } else {
                              return ''
                          }
                }
            }

            sourceComponent:  {
                if ( endpoint && endpoint.state === MakeLeapsEndpoint.STATE_LOADED ) {
                    return loadedToolbarComponent
                }
                return loadingToolbarComponent
            }
        }

        Loader {
            anchors.fill: parent
            Component {
                id: noResourceComponent
                Rectangle {
                    anchors.fill: parent
                    color: 'lightgray'
                }
            }
            Component {
                id: loadingComponent
                Label {
                    anchors.centerIn: parent
                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter
                    text: qsTr('Loading...')
                }
            }
            Component {
                id: errorComponent
                Label {
                    anchors.centerIn: parent
                    color: 'red'
                    text: root.endpoint.lastErrorMessage
                }
            }

            sourceComponent: {
                if ( !root.endpoint )
                    return noResourceComponent;
                if ( root.endpoint.state === MakeLeapsEndpoint.STATE_LOADING )
                    return loadingComponent
                if ( root.endpoint.state === MakeLeapsEndpoint.STATE_ERROR || root.endpoint.state === MakeLeapsEndpoint.STATE_NEEDS_AUTHENTICATION )
                    return errorComponent
                return root.content
            }
        }
    } // initial page
}
