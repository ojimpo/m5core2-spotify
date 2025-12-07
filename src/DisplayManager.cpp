#include "DisplayManager.h"

// Approximate Spotify Green (RGB 29, 185, 84) -> RGB565 conversion
// 29>>3 = 3 (00011)
// 185>>2 = 46 (101110)
// 84>>3 = 10 (01010)
// 0001 1101 1100 1010 -> 0x1DCA
#define SPOTIFY_GREEN 0x1DCA

DisplayManager::DisplayManager() { _lastIsPlaying = false; }

void DisplayManager::begin() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextSize(1);
  // Use a font that supports Japanese. efont is usually available in
  // M5Unified/LovyanGFX If not, we might need to verify availability.
  // fonts::efontJA_16 is a good candidate for standard Japanese text.
  M5.Display.setFont(&fonts::efontJA_16);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
}

void DisplayManager::showLoading(const char *message) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(10, 100);
  M5.Display.println(message);
}

void DisplayManager::showError(const char *message) {
  M5.Display.fillScreen(TFT_RED);
  M5.Display.setTextColor(TFT_WHITE, TFT_RED);
  M5.Display.setCursor(10, 100);
  M5.Display.println(message);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK); // Reset
}

void DisplayManager::updateNowPlaying(String title, String artist,
                                      String albumName, String albumArtUrl) {
  bool artChanged = (albumArtUrl != _lastArtUrl);
  bool textChanged = (title != _lastTitle || artist != _lastArtist);

  if (artChanged && !albumArtUrl.isEmpty()) {
    drawAlbumArt(albumArtUrl);
    _lastArtUrl = albumArtUrl;
  }

  if (textChanged) {
    drawTextInfo(title, artist);
    _lastTitle = title;
    _lastArtist = artist;
  }
}

void DisplayManager::updatePlaybackState(bool isPlaying, int progress,
                                         int duration) {
  // Redraw play/pause button if changed
  if (isPlaying != _lastIsPlaying) {
    drawControls(isPlaying);
    _lastIsPlaying = isPlaying;
  }

  // Draw Progress Bar
  int barY = 186; // Below 180px artwork + padding
  int barHeight = 4;
  int screenW = M5.Display.width();

  float pct = (float)progress / (float)duration;
  if (pct > 1.0)
    pct = 1.0;

  int fillW = (int)(screenW * pct);

  // Draw background bar
  M5.Display.fillRect(0, barY, screenW, barHeight, TFT_DARKGREY);
  // Draw progress
  M5.Display.fillRect(0, barY, fillW, barHeight, SPOTIFY_GREEN);
}

void DisplayManager::drawAlbumArt(String url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    // Clear the artwork area
    M5.Display.fillRect(0, 0, 180, 180, TFT_BLACK);

    WiFiClient *stream = http.getStreamPtr();
    // Scale 300x300 -> 180x180 (scale 0.6)
    M5.Display.drawJpg(stream, 0, 0, 0, 0, 0, 0, 0.6f);
  }
  http.end();
}

// Helper to draw icons
void drawIcon(int x, int y, int type, uint16_t color) {
  // type: 0=Prev, 1=Play, 2=Pause, 3=Next
  M5.Display.setColor(color);
  int size = 9; // Reduced size (3/4 of 12)

  if (type == 0) { // Prev (|<< like, but simplified to |<)
    // Bar
    M5.Display.fillRect(x - size, y - size, 4, size * 2, color);
    // Triangle pointing left
    M5.Display.fillTriangle(x - size + 4, y, x + size, y - size, x + size,
                            y + size, color);
  } else if (type == 1) { // Play (Triangle)
    // Adjust center for visual balance
    int off = 2;
    M5.Display.fillTriangle(x - size + off, y - size, x - size + off, y + size,
                            x + size + off, y, color);
  } else if (type == 2) { // Pause (||)
    M5.Display.fillRect(x - 8, y - size, 6, size * 2, color);
    M5.Display.fillRect(x + 2, y - size, 6, size * 2, color);
  } else if (type == 3) { // Next (>|)
    // Triangle pointing right
    M5.Display.fillTriangle(x - size, y - size, x - size, y + size,
                            x + size - 4, y, color);
    // Bar
    M5.Display.fillRect(x + size - 4, y - size, 4, size * 2, color);
  }
}

void DisplayManager::drawTextInfo(String title, String artist) {
  // Clear text area (X=180 to 320, Y=0 to 180)
  M5.Display.fillRect(180, 0, 140, 180, TFT_BLACK);
  M5.Display.setClipRect(180, 0, 140, 180);

  // Layout calculations
  // Artwork 180px. Center Y = 90.
  // Block Height = 52px.
  // Start Y = 90 - 26 = 64.

  int startX = 186; // +6px margin
  int startY = 64;

  M5.Display.setCursor(startX, startY);

  // Title: 20px, White, Prominent
  M5.Display.setFont(&fonts::lgfxJapanGothicP_20);
  M5.Display.setTextSize(1.0);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);

  // Manual Wrapping for Title
  int maxWidth = 135; // Increased slightly due to reduced margin
  M5.Display.setTextWrap(false);

  if (M5.Display.textWidth(title) > maxWidth) {
    String line1 = "";
    String line2 = "";
    int len = title.length();

    for (int i = 0; i < len;) {
      int charLen = 1;
      unsigned char c = title[i];
      if (c >= 0xF0)
        charLen = 4;
      else if (c >= 0xE0)
        charLen = 3;
      else if (c >= 0xC0)
        charLen = 2;

      String temp = title.substring(0, i + charLen);
      if (M5.Display.textWidth(temp) > maxWidth) {
        line1 = title.substring(0, i);
        line2 = title.substring(i);
        break;
      }
      i += charLen;
      if (i >= len) {
        line1 = title;
        line2 = "";
      }
    }

    int adjustedY = startY - 12;
    M5.Display.setCursor(startX, adjustedY);

    M5.Display.println(line1);
    M5.Display.setCursor(startX, M5.Display.getCursorY());
    M5.Display.println(line2);
  } else {
    M5.Display.setCursor(startX, startY);
    M5.Display.println(title);
  }

  // Gap
  int cursorY = M5.Display.getCursorY() + 8;
  M5.Display.setCursor(startX, cursorY);

  // Artist: 16px, Grey
  M5.Display.setFont(&fonts::lgfxJapanGothicP_16);
  M5.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

  // Manual Wrapping for Artist
  if (M5.Display.textWidth(artist) > maxWidth) {
    String line1 = "";
    String line2 = "";
    int len = artist.length();

    for (int i = 0; i < len;) {
      int charLen = 1;
      unsigned char c = artist[i];
      if (c >= 0xF0)
        charLen = 4;
      else if (c >= 0xE0)
        charLen = 3;
      else if (c >= 0xC0)
        charLen = 2;

      String temp = artist.substring(0, i + charLen);
      if (M5.Display.textWidth(temp) > maxWidth) {
        line1 = artist.substring(0, i);
        line2 = artist.substring(i);
        break;
      }
      i += charLen;
      if (i >= len) {
        line1 = artist;
        line2 = "";
      }
    }
    M5.Display.println(line1);
    M5.Display.setCursor(startX, M5.Display.getCursorY());
    M5.Display.println(line2);
  } else {
    M5.Display.println(artist);
  }

  M5.Display.setTextWrap(true);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.clearClipRect();
}

void DisplayManager::drawControls(bool isPlaying) {
  int centerY = 216; // Lower position (Bottom 240, R=19, Pad=5 -> 240-24=216)

  // Prev Button (Center ~ 60)
  // Clear area (Reduced)
  M5.Display.fillRect(45, centerY - 15, 30, 30, TFT_BLACK);
  drawIcon(60, centerY, 0, TFT_WHITE);

  // Play/Pause - Special Circular Button (Center 160)
  int ppX = 160;
  int ppR = 19; // Reduced from 25 (approx 3/4)

  // Clear area (Reduced)
  M5.Display.fillRect(135, centerY - 25, 50, 50, TFT_BLACK);

  // Draw White Circle
  M5.Display.fillCircle(ppX, centerY, ppR, TFT_WHITE);

  // Draw Icon
  if (isPlaying) {
    drawIcon(ppX, centerY, 2, TFT_BLACK);
  } else {
    drawIcon(ppX, centerY, 1, TFT_BLACK);
  }

  // Next Button (Center ~ 260)
  // Clear area (Reduced)
  M5.Display.fillRect(245, centerY - 15, 30, 30, TFT_BLACK);
  drawIcon(260, centerY, 3, TFT_WHITE);
}

void DisplayManager::drawButton(int x, int y, int w, int h, const char *label,
                                uint16_t color, bool filled) {}

void DisplayManager::updateControlState(bool shuffle, const char *repeatMode,
                                        bool isLiked) {}
