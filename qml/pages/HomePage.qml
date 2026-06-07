import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    // 供 DashboardPage 快捷入口调用
    function switchToTab(idx) {
        contentView.currentIndex = idx
    }

    // 返回键：内部栈 pop → 否则切到首页 → 否则交上层
    Keys.onBackPressed: function(event) {
        if (contentView.currentIndex > 0) {
            contentView.currentIndex = 0
            event.accepted = true
        }
    }

    // ── 底部导航栏 ──────────────────────────
    footer: Rectangle {
        width: parent.width; height: 64
        color: Theme.colorCard
        border.color: Theme.colorBorder
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.topMargin: 4
            anchors.bottomMargin: 6
            spacing: 0

            Repeater {
                model: [
                    { text: "首页",   icon: "🏠", badge: "" },
                    { text: "药品",   icon: "💊", badge: "" },
                    { text: "健康",   icon: "❤️", badge: "" },
                    { text: "购物车", icon: "🛒", badge: "cart" },
                    { text: "我的",   icon: "👤", badge: "" }
                ]

                Item {
                    Layout.fillWidth: true; Layout.fillHeight: true

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 2

                        // Icon + badge
                        Item {
                            Layout.alignment: Qt.AlignHCenter
                            width: 28; height: 28
                            Label {
                                anchors.centerIn: parent
                                text: modelData.icon
                                font.pixelSize: 22
                                opacity: contentView.currentIndex === index ? 1.0 : 0.5
                            }
                            Rectangle {
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.rightMargin: -4; anchors.topMargin: -4
                                visible: modelData.badge === "cart" && App.cartCount > 0
                                width: 18; height: 18; radius: 9; color: Theme.colorDanger
                                Label {
                                    anchors.centerIn: parent
                                    text: App.cartCount > 99 ? "99+" : App.cartCount
                                    font.pixelSize: 9; color: "#fff"; font.bold: true
                                }
                            }
                        }

                        Item { Layout.preferredHeight: 6 }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: modelData.text
                            font.pixelSize: 10
                            color: contentView.currentIndex === index ? Theme.colorPrimary : Theme.colorSubText
                            font.bold: contentView.currentIndex === index
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: contentView.currentIndex = index
                    }
                }
            }
        }
    }

    // ── 内容区（SwipeView）───────────────────
    SwipeView {
        id: contentView
        anchors.fill: parent
        interactive: false
        currentIndex: 0

        onCurrentIndexChanged: {
            // 切换到"我的"(tab 4) 时，重置 profileStack 到根页面
            if (currentIndex === 4 && tabProfile.item)
                tabProfile.item.popToRoot()
        }

        Loader { id: tabHome;    source: "qrc:/qml/pages/DashboardPage.qml";     asynchronous: false }
        Loader { id: tabDrugs;   source: "qrc:/qml/pages/DrugShelfPage.qml";     asynchronous: false }
        Loader { id: tabHealth;  source: "qrc:/qml/pages/HealthMonitorPage.qml";  asynchronous: false }
        Loader { id: tabCart;    source: "qrc:/qml/pages/CartPage.qml";           asynchronous: false }
        Loader { id: tabProfile; source: "qrc:/qml/pages/ProfilePage.qml";        asynchronous: false }
    }
}
