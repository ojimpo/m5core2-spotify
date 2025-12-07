#include "DisplayManager.h"

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
  int barY = 180;
  int barHeight = 4;
  int screenW = M5.Display.width();

  float pct = (float)progress / (float)duration;
  if (pct > 1.0)
    pct = 1.0;

  int fillW = (int)(screenW * pct);

  // Draw background bar
  M5.Display.fillRect(0, barY, screenW, barHeight, TFT_DARKGREY);
  // Draw progress
  M5.Display.fillRect(0, barY, fillW, barHeight, TFT_GREEN);
}

void DisplayManager::drawAlbumArt(String url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    // Clear the artwork area
    M5.Display.fillRect(10, 30, 150, 150, TFT_BLACK);

    WiFiClient *stream = http.getStreamPtr();
    // Use jpeg_div_2 to scale 300x300 down to 150x150
    M5.Display.drawJpg(stream, 10, 30, 0, 0, 0, 0, jpeg_div_t::JPEG_DIV_2);
  }
  http.end();
}

void DisplayManager::drawTextInfo(String title, String artist) {
  M5.Display.fillRect(160, 30, 160, 150, TFT_BLACK);

  // Add padding: start at 170 instead of 160
  M5.Display.setCursor(170, 40);

  // Title
  M5.Display.setFont(&fonts::efontJA_16);
  M5.Display.println(title);

  // Artist
  M5.Display.setCursor(170, 80);
  M5.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5.Display.println(artist);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK); // Reset
}

void DisplayManager::drawControls(bool isPlaying) {
  // Draw Control Area background
  // y=190 to 240
  // Buttons: Prev, Play/Pause, Next

  int btnY = 200;
  int btnW = 60;
  int btnH = 40;

  // Prev
  drawButton(40, btnY, btnW, btnH, "|<", TFT_WHITE);

  // Play/Pause
  const char *label = isPlaying ? "||" : ">";
  drawButton(130, btnY, btnW, btnH, label, TFT_GREEN);

  // Next
  drawButton(220, btnY, btnW, btnH, ">|", TFT_WHITE);
}

void DisplayManager::drawButton(int x, int y, int w, int h, const char *label,
                                uint16_t color) {
  // Simple button drawing
  // We won't draw fancy stored buttons for now, just visuals
  // Interaction is handled by touch zones in main.cpp

  // Clear the button area first to prevent artifacts
  M5.Display.fillRect(x, y, w, h, TFT_BLACK);

  // Optional: Draw a border for better visibility?
  // M5.Display.drawRect(x, y, w, h, TFT_DARKGREY);

  M5.Display.setCursor(x + w / 2 - 10, y + h / 2 - 8);
  M5.Display.setTextColor(color);
  M5.Display.print(label);
  M5.Display.setTextColor(TFT_WHITE);
}

void DisplayManager::updateControlState(bool shuffle, const char *repeatMode,
                                        bool isLiked) {
  // Optional indicators
}
