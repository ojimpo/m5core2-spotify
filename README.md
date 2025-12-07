# M5Stack Core2 Spotify コントローラー

このプロジェクトは、M5Stack Core2を専用のSpotifyリモコンに変身させ、アルバムアートの表示や日本語の曲名表示を可能にします。

## 機能

- **再生コントロール**: 再生、一時停止、曲送り、曲戻し。
- **ビジュアル表示**: 高画質なアルバムアートワークの表示。
- **メタデータ**: 曲名、アーティスト名の表示（日本語/漢字対応）。
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
3.  **書き込み**:
    - PlatformIOを使用して、ファームウェアをビルドしM5Stack Core2に書き込みます。

## クレジット

このプロジェクトは、Google DeepMindの **Antigravity** (Powered by **Gemini**) によって作成されました。

---

# M5Stack Core2 Spotify Controller

This project transforms your M5Stack Core2 into a dedicated Spotify remote controller with cover art display and Japanese text support.

## Features

- **Playback Control**: Play, Pause, Next, Previous.
- **Visual Display**: Shows high-quality Album Artwork.
- **Metadata**: Displays Track Title and Artist Name (Supports Japanese/Kanji).
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
3.  **Build & Flash**:
    - Use PlatformIO to build and upload the firmware to your M5Stack Core2.

## Credits

This project was created with **Antigravity**, an agentic AI coding assistant by Google DeepMind, powered by **Gemini**.
