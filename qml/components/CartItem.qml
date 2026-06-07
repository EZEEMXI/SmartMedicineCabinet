import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: item
    radius: 0; color: Theme.colorCard
    height: 76

    property int cartId: 0
    property string cartName: ""
    property double cartPrice: 0
    property int cartNum: 1

    signal add()
    signal minus()
    signal remove()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // 药品图标
        Rectangle {
            width: 44; height: 44; radius: 10
            color: Theme.colorBg
            Label {
                anchors.centerIn: parent; text: "💊"; font.pixelSize: 20
            }
        }

        ColumnLayout {
            Layout.fillWidth: true; spacing: 2
            Label {
                text: cartName; font.pixelSize: Theme.fsSM; font.bold: true
                color: Theme.colorText; elide: Text.ElideRight
                Layout.fillWidth: true; maximumLineCount: 1
            }
            Label {
                text: "¥" + cartPrice.toFixed(2)
                font.pixelSize: Theme.fsMD; font.bold: true; color: Theme.colorDanger
            }
        }

        // +/- 按钮
        RowLayout { spacing: 4
            Rectangle {
                width: 30; height: 30; radius: 8; color: Theme.colorBg
                Label {
                    anchors.centerIn: parent; text: "-"; font.pixelSize: 16
                    font.bold: true; color: Theme.colorText
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: item.minus()
                }
            }

            Label {
                text: cartNum; font.pixelSize: Theme.fsMD; font.bold: true
                color: Theme.colorText
                Layout.preferredWidth: 24
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                width: 30; height: 30; radius: 8; color: Theme.colorAccent
                Label {
                    anchors.centerIn: parent; text: "+"; font.pixelSize: 16
                    font.bold: true; color: "#fff"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: item.add()
                }
            }
        }

        // 删除
        Label {
            text: "🗑️"; font.pixelSize: 16
            MouseArea {
                anchors.fill: parent
                onClicked: item.remove()
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1; color: Theme.colorBorder
    }
}
