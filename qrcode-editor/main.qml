import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import App 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Button {
        text: qsTr("Click")

        onClicked: {
            console.log(Utils.genQRCode())
        }
    }
}
