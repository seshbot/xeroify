import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import BusyBot 1.0

// TODO:
//   MakeLeapsResource {
//      endpoint: xxx
//      onCreateNewResource: { }
//      onDeleteResource: { }
//      contentItem: ContactPanel { ... }
//   }
// TODO: provide +add button in header
// TODO: check for delete/patch capabilities and show buttons

// CLIENT
//
// "url": "https://app.makeleaps.com/api/partner/1725857643916378083/client/1734479364211706503/",
// "mid": "1734479364211706503",
// "display_name": "TEST CECHNERCOMPANY",
// "organization_type": "client",
// "is_organization": false,
// "date_archived": null,
// "client_external_id": "",
// "contacts": [
//     "https://app.makeleaps.com/api/partner/1725857643916378083/contact/1734479364020640438/"
// ],
// "default_contact": "https://app.makeleaps.com/api/partner/1725857643916378083/contact/1734479364020640438/",
// "managing_teams": [],
// "default_presets": []

// CONTACT
//
//"url": "https://app.makeleaps.com/api/partner/1725857643916378083/contact/1734479364020640438/",
//"mid": "1734479364020640438",
//"owner": "https://app.makeleaps.com/api/partner/1725857643916378083/client/1734479364211706503/",
//"owner_type": "client",
//"contact_type": "person",
//"name": "",
//"family_name": "Cechner",
//"given_name": "Shaun",
//"display_name": "Shaun Cechner",
//"title": "COO",
//"department": "Accounts Receivable",
//"format": "default",
//"phone_number": "029-852-0244",
//"is_default": true,
//"email": {
// "address": "storkyd@gmail.com"
//},
//"addresses": [],
//"default_address": null

// Summary:
//  - managing groups
//  - outstanding total
//  - invoices recently sent
//  - notes
// Documents
// Notes
//
// Contact:
//Name: Shaun Cechner
//Title: COO
//Department: Accounts Receivable
//Phone Number: 029-852-0244
//Email: storkyd@gmail.com
//Address —

Page {
    property ApiObject client

    Loader {
        Component {
            id: loadingComponent
            Label {
                anchors.centerIn: parent
                text: 'loading...'
            }
        }

        Component {
            id: loadedComponent
            GridLayout {
                anchors.fill: parent
                columns: 2
                Label {
                    text: ''
                }
            }
        }

        sourceComponent: loadingComponent
    }
}
