# M5Stack Core2 Spotify Controller / M5Stack Core2 Spotify コントローラー

このプロジェクトは、M5Stack Core2を専用のSpotifyリモコンに変身させ、アルバムアートの表示や日本語の曲名表示を可能にします。

This project transforms your M5Stack Core2 into a dedicated Spotify remote controller with cover art display and Japanese text support.

<img src="https://github.com/user-attachments/assets/placeholder" width="300">

## 機能 / Features

- **再生コントロール**: 再生、一時停止、曲送り、曲戻し。
  - **Playback Control**: Play, Pause, Next, Previous.
- **ビジュアル表示**: 高画質なアルバムアートワークの表示。
  - **Visual Display**: Shows high-quality Album Artwork.
- **メタデータ**: 曲名、アーティスト名の表示（日本語/漢字対応）。
  - **Metadata**: Displays Track Title and Artist Name (Supports Japanese/Kanji).
- **操作**: M5Stack Core2の物理ボタンとタッチスクリーンを使用。
  - **Physical Integration**: Uses M5Stack Core2 physical buttons and touch screen.

## ハードウェア / Hardware

- M5Stack Core2

## セットアップ / Setup

1.  **リポジトリのクローン / Clone the repository**
2.  **認証情報の設定 / Configure Credentials**:
    - `include/secrets.h.template` を `include/secrets.h` にリネームします。
    - Rename `include/secrets.h.template` to `include/secrets.h`.
    - WiFiの認証情報を入力します。
    - Enter your WiFi credentials.
    - SpotifyのClient ID, Client Secret, Refresh Tokenを入力します。
    - Enter your Spotify Client ID, Client Secret, and Refresh Token.
    - *注意: `secrets.h` はキーを保護するためgitの管理対象外になっています。*
    - *Note: `secrets.h` is git-ignored to protect your keys.*
3.  **書き込み / Build & Flash**:
    - PlatformIOを使用して、ファームウェアをビルドしM5Stack Core2に書き込みます。
    - Use PlatformIO to build and upload the firmware to your M5Stack Core2.

## クレジット / Credits

このプロジェクトは、Google DeepMindの **Antigravity** (Powered by **Gemini**) によって作成されました。

This project was created with **Antigravity**, an agentic AI coding assistant by Google DeepMind, powered by **Gemini**.
