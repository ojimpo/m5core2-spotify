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
  M5.Display.fillRect(0, barY, fillW, barHeight, SPOTIFY_GREEN);
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

// Helper to draw icons
void drawIcon(int x, int y, int type, uint16_t color) {
  // type: 0=Prev, 1=Play, 2=Pause, 3=Next
  M5.Display.setColor(color);
  int size = 12; // Base size for icons

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
  // Clear text area
  M5.Display.fillRect(160, 30, 160, 150, TFT_BLACK);
  M5.Display.setClipRect(160, 30, 160, 150);

  int startY = 40;
  int startX = 170;

  M5.Display.setCursor(startX, startY);

  // Title: Large, White
  // Scale usually helps but lgfx fonts are bitmaps mostly.
  // Let's try 16px scaled 1.2 or just 16px. User wanted Title > Artist.
  // We'll keep Title 16px standard, and make Artist smaller (0.8).
  M5.Display.setFont(&fonts::lgfxJapanGothicP_16);
  M5.Display.setTextSize(1.2);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.println(title);

  // Artist: Smaller, Grey
  // Reduce gap: Get current Y.
  int artistY = M5.Display.getCursorY();
  // Ensure minimal gap but not huge
  if (artistY < startY + 24)
    artistY = startY + 24;

  M5.Display.setCursor(startX, artistY);
  M5.Display.setTextSize(0.9); // Slightly smaller
  M5.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5.Display.println(artist);

  // Reset
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1.0);
  M5.Display.clearClipRect();
}

void DisplayManager::drawControls(bool isPlaying) {
  // Draw Control Area background
  int btnY = 190; // Moved up from 200 to correct circle cut-off (200+25+offset
                  // > 240) 190 Center of buttons? No, buttons are height 40?
                  // Let's decide a center Y relative to bottom.
                  // Screen 240. Bottom section height ~60. Center ~210.
                  // If we use Y=190 as top-left of button rect, center is 210.
                  // Circle radius 25 -> 210+25=235. Fits.

  int centerY = 210;

  // Prev Button (Center ~ 60)
  // Clear area
  M5.Display.fillRect(40, centerY - 20, 40, 40, TFT_BLACK);
  drawIcon(60, centerY, 0, TFT_WHITE); // Type 0 = Prev

  // Play/Pause - Special Circular Button (Center 160)
  int ppX = 160;
  int ppR = 25;

  // Clear area (larger for circle)
  M5.Display.fillRect(130, centerY - 30, 60, 60, TFT_BLACK);

  // Draw White Circle
  M5.Display.fillCircle(ppX, centerY, ppR, TFT_WHITE);

  // Draw Icon
  if (isPlaying) {
    drawIcon(ppX, centerY, 2, TFT_BLACK); // Pause
  } else {
    drawIcon(ppX, centerY, 1, TFT_BLACK); // Play
  }

  // Next Button (Center ~ 260)
  // Clear area
  M5.Display.fillRect(240, centerY - 20, 40, 40, TFT_BLACK);
  drawIcon(260, centerY, 3, TFT_WHITE); // Type 3 = Next
}

void DisplayManager::drawButton(int x, int y, int w, int h, const char *label,
                                uint16_t color, bool filled) {
  // Legacy drawButton (still used by logic? No, we replaced calls in
  // drawControls) Leaving empty or simple just in case, or removing if
  // possible. Code might call it elsewhere? No, only drawControls used it. But
  // updating header signatures usually unsafe if we don't update
  // implementation. We'll keep it but it might be unused by drawControls now.
  // Or we can assume this function is dead code for controls but maybe useful
  // later.
}

void DisplayManager::updateControlState(bool shuffle, const char *repeatMode,
                                        bool isLiked) {
  // Optional indicators
}
