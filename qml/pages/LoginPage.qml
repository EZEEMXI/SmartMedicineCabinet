import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    background: Rectangle { color: Theme.colorBg }

    property bool isRegister: false
    property string loginError: ""
    property bool configExpanded: false

    header: ToolBar {
        background: Rectangle { color: Theme.colorPrimary }
        contentHeight: 56
        Label {
            anchors.centerIn: parent
            text: "智能药柜"
            font.pixelSize: Theme.fsLG; font.bold: true
            color: "#ffffff"
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: mainCol.implicitHeight + 40
        clip: true

        ColumnLayout {
            id: mainCol
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 24
            spacing: 14

            // ── Logo ───────────────────────────────
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 80; height: 80; radius: 40
                color: Theme.colorPrimary
                Label { anchors.centerIn: parent; text: "💊"; font.pixelSize: 36 }
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: isRegister ? "创建新账号" : "智慧药柜管理系统"
                font.pixelSize: Theme.fsLG; font.bold: true
                color: Theme.colorText
            }

            // ── 登录/注册模式切换 ──────────────
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 0
                Rectangle {
                    width: 140; height: 36; radius: 8
                    color: isRegister ? "#e5e7eb" : Theme.colorPrimary
                    Label {
                        anchors.centerIn: parent
                        text: "登录"; font.pixelSize: Theme.fsSM; font.bold: !isRegister
                        color: isRegister ? Theme.colorText : "#ffffff"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: { isRegister = false; loginError = "" }
                    }
                }
                Item { width: 6 }
                Rectangle {
                    width: 140; height: 36; radius: 8
                    color: isRegister ? Theme.colorPrimary : "#e5e7eb"
                    Label {
                        anchors.centerIn: parent
                        text: "注册"; font.pixelSize: Theme.fsSM; font.bold: isRegister
                        color: isRegister ? "#ffffff" : Theme.colorText
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: { isRegister = true; loginError = "" }
                    }
                }
            }

            // ── 表单卡片 ────────────────────────
            Rectangle {
                Layout.fillWidth: true
                radius: 12; color: Theme.colorCard
                Layout.preferredHeight: loginForm.implicitHeight + 24

                ColumnLayout {
                    id: loginForm
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                    spacing: 12

                    RowLayout {
                        Label {
                            text: "用户名"
                            font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                            Layout.preferredWidth: 56
                        }
                        TextField {
                            id: tfUsername
                            Layout.fillWidth: true
                            font.pixelSize: Theme.fsMD
                            placeholderText: "请输入用户名"
                        }
                    }

                    RowLayout {
                        Label {
                            text: "密　码"
                            font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                            Layout.preferredWidth: 56
                        }
                        TextField {
                            id: tfPassword
                            Layout.fillWidth: true
                            font.pixelSize: Theme.fsMD
                            placeholderText: "请输入密码"
                            echoMode: showPwdBtn.checked ? TextInput.Normal : TextInput.Password
                        }
                        ToolButton {
                            id: showPwdBtn
                            checkable: true
                            text: checked ? "🙈" : "👁"
                            font.pixelSize: Theme.fsMD
                            background: Rectangle { color: "transparent" }
                        }
                    }

                    // ── 注册模式下显示确认密码 ──
                    RowLayout {
                        visible: isRegister
                        Label {
                            text: "确认密码"
                            font.pixelSize: Theme.fsSM; color: Theme.colorSubText
                            Layout.preferredWidth: 56
                        }
                        TextField {
                            id: tfConfirmPwd
                            Layout.fillWidth: true
                            font.pixelSize: Theme.fsMD
                            placeholderText: "再次输入密码"
                            echoMode: TextInput.Password
                        }
                    }
                }
            }

            // ── 错误提示 ────────────────────────
            Rectangle {
                visible: loginError !== ""
                Layout.fillWidth: true; Layout.preferredHeight: 36
                radius: 8; color: "#fef2f2"
                Label {
                    anchors.centerIn: parent
                    text: loginError
                    font.pixelSize: Theme.fsSM; color: Theme.colorDanger
                }
            }

            // ── 连接状态 ────────────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 36
                radius: 8
                color: App.mqttConnected ? "#dcfce7" : "#f3f4f6"
                Label {
                    anchors.centerIn: parent
                    text: App.mqttConnected ? "● 云平台已连接" : "○ " + App.mqttStatus
                    font.pixelSize: Theme.fsSM
                    color: App.mqttConnected ? "#16a34a" : (App.mqttStatus.indexOf("失败") >= 0 ? "#dc2626" : "#6b7280")
                }
            }

            // ── 登录/注册按钮 ──────────────────
            Button {
                Layout.fillWidth: true; Layout.preferredHeight: 48
                text: isRegister ? "注  册" : "登  录"
                font.pixelSize: Theme.fsMD; font.bold: true
                enabled: tfUsername.text.trim().length > 0 && tfPassword.text.length > 0

                background: Rectangle {
                    radius: 12
                    color: parent.enabled ? (parent.pressed ? "#1d4ed8" : Theme.colorPrimary) : "#9ca3af"
                }
                contentItem: Label {
                    text: parent.text
                    font: parent.font
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    loginError = ""
                    var uname = tfUsername.text.trim()
                    var pwd   = tfPassword.text

                    // 空值检查
                    if (uname === "") {
                        loginError = "用户名不能为空"
                        return
                    }
                    if (pwd === "") {
                        loginError = "密码不能为空"
                        return
                    }

                    if (isRegister) {
                        if (pwd !== tfConfirmPwd.text) {
                            loginError = "两次密码不一致"
                            return
                        }
                        var err = App.registerUser(uname, pwd)
                        if (err !== "") {
                            loginError = err
                            return
                        }
                        // 注册成功，直接进入主页
                        page.StackView.view.replace("qrc:/qml/pages/HomePage.qml")
                    } else {
                        if (App.login(uname, pwd)) {
                            page.StackView.view.replace("qrc:/qml/pages/HomePage.qml")
                        } else {
                            loginError = "用户名或密码错误"
                        }
                    }
                }
            }

            // ── 云平台配置入口 ──────────────────
            Item { Layout.preferredHeight: 8 }

            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 40
                radius: 8; color: "transparent"
                border { color: Theme.colorBorder; width: 1 }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12; anchors.rightMargin: 12
                    Label {
                        text: configExpanded ? "⚙ 云平台配置 ▲" : "⚙ 云平台配置 ▼"
                        font.pixelSize: Theme.fsSM; color: Theme.colorPrimary
                    }
                    Item { Layout.fillWidth: true }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: configExpanded = !configExpanded
                }
            }

            // ── 配置面板 ────────────────────────
            Rectangle {
                Layout.fillWidth: true
                radius: 12; color: Theme.colorCard
                Layout.preferredHeight: configExpanded ? adminPanel.implicitHeight + 20 : 0
                visible: configExpanded; clip: true

                Behavior on Layout.preferredHeight {
                    NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
                }

                ColumnLayout {
                    id: adminPanel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 10

                    Label {
                        text: "OneNet 云平台参数"
                        font.pixelSize: Theme.fsSM; font.bold: true; color: Theme.colorText
                        wrapMode: Text.WordWrap; Layout.fillWidth: true
                    }
                    RowLayout {
                        Label { text: "产品ID"; font.pixelSize: Theme.fsSM; color: Theme.colorSubText; Layout.preferredWidth: 56 }
                        TextField { id: tfProductId; Layout.fillWidth: true; font.pixelSize: Theme.fsSM; placeholderText: "默认: Ht0WBivOOO"; text: App.getSetting("productId", "") }
                    }
                    RowLayout {
                        Label { text: "设备名"; font.pixelSize: Theme.fsSM; color: Theme.colorSubText; Layout.preferredWidth: 56 }
                        TextField { id: tfDeviceName; Layout.fillWidth: true; font.pixelSize: Theme.fsSM; placeholderText: "默认: cabinet_01"; text: App.getSetting("deviceName", "") }
                    }
                    RowLayout {
                        Label { text: "密　钥"; font.pixelSize: Theme.fsSM; color: Theme.colorSubText; Layout.preferredWidth: 56 }
                        TextField { id: tfDeviceKey; Layout.fillWidth: true; font.pixelSize: Theme.fsSM; placeholderText: "留空使用内置密钥"; echoMode: TextInput.Password; text: App.getSetting("deviceKey", "") }
                    }
                    Button {
                        Layout.fillWidth: true; Layout.preferredHeight: 36
                        text: "保存配置并重连"; font.pixelSize: Theme.fsSM; font.bold: true
                        background: Rectangle { radius: 8; color: parent.enabled ? (parent.pressed ? "#059669" : Theme.colorAccent) : "#9ca3af" }
                        contentItem: Label { text: parent.text; font: parent.font; color: "#ffffff"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        onClicked: {
                            App.saveSetting("productId", tfProductId.text)
                            App.saveSetting("deviceName", tfDeviceName.text)
                            App.saveSetting("deviceKey", tfDeviceKey.text)
                            App.setOneNetConfig(tfProductId.text, tfDeviceName.text, tfDeviceKey.text, "")
                            App.connectMqtt()
                            configExpanded = false
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true; Layout.preferredHeight: 40 }
        }
    }
}
