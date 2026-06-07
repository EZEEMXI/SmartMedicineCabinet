#!/usr/bin/env python3
"""
验证 OneNet Token 生成是否与官方工具一致
"""
import hmac, hashlib, base64, urllib.parse, time, json, os

# ── 从 secrets.json 读取凭证（不硬编码在源码里）──
_secrets_path = os.path.join(os.path.dirname(__file__), "secrets.json")
with open(_secrets_path, encoding="utf-8") as _f:
    _sec = json.load(_f)["onenet"]

PRODUCT_ID  = _sec["productId"]
DEVICE_NAME = _sec["deviceName"]
ACCESS_KEY  = _sec["accessKey"]   # 产品 access_key（原始 base64）

# 用户 MQTT.fx 测试时用的 et 值（从 token 反推）
# et=1811921512 对应 ~2027-06-02，说明是 1 年有效期
ET = 1811921512
VERSION = "2018-10-31"
METHOD  = "md5"
RES     = f"products/{PRODUCT_ID}/devices/{DEVICE_NAME}"

def gen_token(et, key_b64, do_decode=True):
    """生成 OneNet Token"""
    key = base64.b64decode(key_b64) if do_decode else key_b64.encode()
    sign_str = f"{et}\n{METHOD}\n{RES}\n{VERSION}"
    sign = hmac.new(key, sign_str.encode(), hashlib.md5).digest()
    sign_b64 = base64.b64encode(sign).decode()   # 标准 Base64，保留 ==
    sign_enc = urllib.parse.quote(sign_b64, safe="")
    res_enc  = urllib.parse.quote(RES, safe="")
    token = f"version={VERSION}&res={res_enc}&et={et}&method={METHOD}&sign={sign_enc}"
    return token, sign_b64

print("=" * 60)
print("OneNet Token 验证")
print("=" * 60)
print(f"productId : {PRODUCT_ID}")
print(f"deviceName: {DEVICE_NAME}")
print(f"access_key: {ACCESS_KEY}")
print(f"res (raw) : {RES}")
print(f"et        : {ET}")
print()

# 方式1：先 Base64 解码 key，再 HMAC（SDK 标准方式，C++ 代码用的这个）
token1, sign1 = gen_token(ET, ACCESS_KEY, do_decode=True)
print("─" * 60)
print("方式1（推荐）: key 先 Base64 解码 → HMAC-MD5")
print(f"sign (B64): {sign1}")
print(f"token     : {token1}")
print()

# 方式2：key 不解码，直接用 base64 字符串的 UTF-8 字节作为 HMAC key
token2, sign2 = gen_token(ET, ACCESS_KEY, do_decode=False)
print("─" * 60)
print("方式2（错误）: key 不解码，直接用字符串字节")
print(f"sign (B64): {sign2}")
print(f"token     : {token2}")
print()

# 用户 MQTT.fx 连接成功的 token（从之前消息中提取）
user_token = "version=2018-10-31&res=products%2FHt0WBivOOO%2Fdevices%2Fcabinet_01&et=1811921512&method=md5&sign=WXHFF4IWQmLzmHuzvwbEEA%3D%3D"
print("─" * 60)
print("用户 MQTT.fx 成功连接的 Token:")
print(user_token)
print()

# 提取用户 token 中的 sign 进行对比
parsed = dict(urllib.parse.parse_qsl(user_token))
user_sign_enc = parsed["sign"]
user_sign_b64 = urllib.parse.unquote(user_sign_enc)
print(f"用户 token sign (B64): {user_sign_b64}")
print()

if sign1 == user_sign_b64:
    print("✅ 方式1 匹配！C++ 代码逻辑正确（key 先 Base64 解码）")
elif sign2 == user_sign_b64:
    print("❌ 方式2 匹配！需要修改 C++ 代码（key 不解码）")
else:
    print("⚠️  两种方式都不匹配，检查参数是否有误")
    print(f"  期望: {user_sign_b64}")
    print(f"  方式1: {sign1}")
    print(f"  方式2: {sign2}")
