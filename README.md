# M5Stack Core2 Spotify コントローラー

このプロジェクトは、M5Stack Core2を専用のSpotifyリモコンに変身させ、アルバムアートの表示や日本語の曲名表示を可能にします。

## 機能

- **再生コントロール**: 再生、一時停止、曲送り、曲戻し。
- **ビジュアル表示**: 高画質なアルバムアートワークの表示。
- **メタデータ**: 曲名、アーティスト名の表示（日本語/漢字対応）。
- **ライブラリ管理**: アルバムアート上のボタンで曲をライブラリに追加/削除。
- **操作**: M5Stack Core2の物理ボタンとタッチスクリーンを使用。

## ハードウェア

- M5Stack Core2

## セットアップ

1.  **リポジトリのクローン**:
    ```bash
    git clone https://github.com/ojimpo/m5core2-spotify.git
    ```
2.  **認証情報の設定**:
    - `include/secrets.h.template` を `include/secrets.h` にリネームします。
    - `include/secrets.h` に以下の情報を入力します：
        - WiFiの認証情報
        - SpotifyのClient ID, Client Secret, Refresh Token
    - *注意: `secrets.h` はキーを保護するためgitの管理対象外（.gitignore）になっています。*
    
    **Spotify Refresh Tokenの取得方法**:
    - 以下のスコープを含めてトークンを取得してください：
        - `user-read-playback-state` - 再生状態の読み取り
        - `user-modify-playback-state` - 再生制御
        - `user-read-currently-playing` - 現在の再生曲の取得
        - `user-library-read` - ライブラリの読み取り（Like状態の確認に必要）
        - `user-library-modify` - ライブラリへの追加/削除
    - 認証URL例（CLIENT_IDを置き換えてください）:
      ```
      https://accounts.spotify.com/authorize?client_id=YOUR_CLIENT_ID&response_type=code&redirect_uri=http://localhost:8888/callback&scope=user-read-playback-state%20user-modify-playback-state%20user-read-currently-playing%20user-library-read%20user-library-modify
      ```
3.  **書き込み**:
    - PlatformIOを使用して、ファームウェアをビルドしM5Stack Core2に書き込みます。

## トラブルシューティング

### Spotify Refresh Tokenの簡単な取得方法

手動でトークンを取得するのが難しい場合は、以下のWebツールを使用できます：

**https://spotify-refresh-token-generator.netlify.app/**

このツールを使用する場合：
1. Spotify Developer Dashboardで、リダイレクトURIに `https://spotify-refresh-token-generator.netlify.app/callback` を追加してください
2. 上記URLにアクセスし、必要なスコープを選択（すべて選択を推奨）
3. Client IDとClient Secretを入力
4. 生成されたRefresh Tokenを `secrets.h` にコピー

### PlatformIOで書き込みができない場合

PlatformIOが使用できない、またはエラーが発生する場合の代替方法：

**Arduino IDEを使用する方法:**

1. Arduino IDEをインストール
2. ボードマネージャーで「ESP32」を検索してインストール
3. ライブラリマネージャーで以下をインストール：
   - M5Unified
   - SpotifyEsp32
   - ArduinoJson
4. ボード設定：
   - ボード: "M5Stack-Core2"
   - Upload Speed: 115200
   - Flash Frequency: 80MHz
5. `src/main.cpp` を開いて書き込み

**esptool.pyを使用する方法（上級者向け）:**

```bash
# PlatformIOでビルドのみ実行
pio run

# 生成されたバイナリを手動で書き込み
esptool.py --chip esp32 --port /dev/cu.usbserial-XXXX write_flash 0x10000 .pio/build/m5stack-core2/firmware.bin
```

## クレジット

このプロジェクトは、Google DeepMindの **Antigravity** (Powered by **Gemini**) によって作成されました。

---

# M5Stack Core2 Spotify Controller

This project transforms your M5Stack Core2 into a dedicated Spotify remote controller with cover art display and Japanese text support.

## Features

- **Playback Control**: Play, Pause, Next, Previous.
- **Visual Display**: Shows high-quality Album Artwork.
- **Metadata**: Displays Track Title and Artist Name (Supports Japanese/Kanji).
- **Library Management**: Add/remove tracks to your library via button overlay on album art.
- **Physical Integration**: Uses M5Stack Core2 physical buttons and touch screen.

## Hardware

- M5Stack Core2

## Setup

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/ojimpo/m5core2-spotify.git
    ```
2.  **Configure Credentials**:
    - Rename `include/secrets.h.template` to `include/secrets.h`.
    - Fill in the following in `include/secrets.h`:
        - Your WiFi credentials.
        - Your Spotify Client ID, Client Secret, and Refresh Token.
    - *Note: `secrets.h` is git-ignored to protect your keys.*
    
    **Obtaining Spotify Refresh Token**:
    - Ensure your token includes the following scopes:
        - `user-read-playback-state` - Read playback state
        - `user-modify-playback-state` - Control playback
        - `user-read-currently-playing` - Get currently playing track
        - `user-library-read` - Read library (required for Like status)
        - `user-library-modify` - Add/remove tracks from library
    - Authorization URL example (replace YOUR_CLIENT_ID):
      ```
      https://accounts.spotify.com/authorize?client_id=YOUR_CLIENT_ID&response_type=code&redirect_uri=http://localhost:8888/callback&scope=user-read-playback-state%20user-modify-playback-state%20user-read-currently-playing%20user-library-read%20user-library-modify
      ```
3.  **Build & Flash**:
    - Use PlatformIO to build and upload the firmware to your M5Stack Core2.

## Troubleshooting

### Easy Way to Get Spotify Refresh Token

If manual token generation is difficult, you can use this web tool:

**https://spotify-refresh-token-generator.netlify.app/**

When using this tool:
1. Add `https://spotify-refresh-token-generator.netlify.app/callback` to your Redirect URIs in Spotify Developer Dashboard
2. Visit the URL above and select required scopes (recommend selecting all)
3. Enter your Client ID and Client Secret
4. Copy the generated Refresh Token to `secrets.h`

### Alternative Upload Methods if PlatformIO Fails

If PlatformIO doesn't work or encounters errors:

**Using Arduino IDE:**

1. Install Arduino IDE
2. Install "ESP32" from Board Manager
3. Install the following libraries from Library Manager:
   - M5Unified
   - SpotifyEsp32
   - ArduinoJson
4. Board settings:
   - Board: "M5Stack-Core2"
   - Upload Speed: 115200
   - Flash Frequency: 80MHz
5. Open `src/main.cpp` and upload

**Using esptool.py (Advanced):**

```bash
# Build only with PlatformIO
pio run

# Manually flash the generated binary
esptool.py --chip esp32 --port /dev/cu.usbserial-XXXX write_flash 0x10000 .pio/build/m5stack-core2/firmware.bin
```

## Credits

This project was created with **Antigravity**, an agentic AI coding assistant by Google DeepMind, powered by **Gemini**.
