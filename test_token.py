"""
OneNet 标准 MQTT Token 生成验证脚本
在电脑上运行: python test_token.py
确认生成的 token 格式正确后，C++ 代码会自动生成相同格式的 token
"""
import base64
import hmac
import time
import json
import os
from urllib.parse import quote

# ── 从 secrets.json 读取凭证（不硬编码在源码里）──
_secrets_path = os.path.join(os.path.dirname(__file__), "secrets.json")
with open(_secrets_path, encoding="utf-8") as _f:
    _sec = json.load(_f)["onenet"]

PRODUCT_ID  = _sec["productId"]
DEVICE_NAME = _sec["deviceName"]
ACCESS_KEY  = _sec["accessKey"]

# ============= Token 生成 ===================
version = "2018-10-31"
res = f"products/{PRODUCT_ID}"  # ⚠️ 不含设备名
et = str(int(time.time()) + 86400 * 365)  # 365 天有效期
method = "sha1"

# 1. Base64 解码 access_key 作为 HMAC 密钥
key = base64.b64decode(ACCESS_KEY)
print(f"[1] key (hex) = {key.hex()}")

# 2. 构造待签名字符串
org = f"{et}\n{method}\n{res}\n{version}"
print(f"[2] org =\n{repr(org)}")

# 3. HMAC-SHA1 签名 → 标准 Base64 (保留 == 填充)
sign_b = hmac.new(key=key, msg=org.encode(), digestmod="sha1")
sign = base64.b64encode(sign_b.digest()).decode()
print(f"[3] sign (base64) = {sign}")

# 4. URL 编码 sign 和 res
sign_encoded = quote(sign, safe='')
res_encoded  = quote(res, safe='')
print(f"[4] sign_encoded = {sign_encoded}")
print(f"    res_encoded  = {res_encoded}")

# 5. 拼接完整 token
token = f"version={version}&res={res_encoded}&et={et}&method={method}&sign={sign_encoded}"
print()
print("=" * 60)
print("完整 Token (用作 MQTT Password):")
print(token)
print("=" * 60)
print()
print("连接参数:")
print(f"  Broker  : mqtts.heclouds.com:1883")
print(f"  ClientId: {DEVICE_NAME}")
print(f"  Username: {PRODUCT_ID}")
print(f"  Password: [上方 token]")
print()
print(f"  Publish Topic : $sys/{PRODUCT_ID}/{DEVICE_NAME}/thing/property/post")
print(f"  Subscribe     : $sys/{PRODUCT_ID}/{DEVICE_NAME}/thing/#")
