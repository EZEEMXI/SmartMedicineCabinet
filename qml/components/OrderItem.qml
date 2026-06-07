import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: item
    radius: 12; color: Theme.colorCard
    height: col.implicitHeight + 24

    property int orderId: 0
    property string orderTime: ""
    property double orderPrice: 0
    property string orderContent: ""

    signal removeRequested()

    ColumnLayout {
        id: col
        anchors.fill: parent
        anchors.margins: 14
        spacing: 6

        // 订单号和时间
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "订单 #" + orderId
                font.pixelSize: Theme.fsSM; font.bold: true; color: Theme.colorText
            }
            Item { Layout.fillWidth: true }
            Label {
                text: orderTime; font.pixelSize: Theme.fsXS; color: Theme.colorSubText
            }
        }

        // 订单内容
        Label {
            Layout.fillWidth: true
            text: orderContent; font.pixelSize: Theme.fsSM; color: Theme.colorText
            wrapMode: Text.WordWrap; lineHeight: 1.4
        }

        // 价格
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "合计"; font.pixelSize: Theme.fsSM; color: Theme.colorSubText
            }
            Label {
                text: "¥" + orderPrice.toFixed(2)
                font.pixelSize: Theme.fsLG; font.bold: true; color: Theme.colorDanger
            }
            Item { Layout.fillWidth: true }
            Label {
                text: "删除"; font.pixelSize: Theme.fsXS; color: Theme.colorSubText
                MouseArea {
                    anchors.fill: parent
                    onClicked: item.removeRequested()
                }
            }
        }
    }
}
