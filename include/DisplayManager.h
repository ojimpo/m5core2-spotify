#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <HTTPClient.h>
#include <M5Unified.h>

class DisplayManager {
public:
  DisplayManager();
  void begin();
  void updateNowPlaying(String title, String artist, String albumName,
                        String albumArtUrl);
  void updatePlaybackState(bool isPlaying, int progress, int duration);
  void showLoading(const char *message);
  void showError(const char *message);

  // Call this if button state changes to redraw specific icons
  void updateControlState(bool shuffle, const char *repeatMode, bool isLiked);

private:
  String _lastArtUrl;
  String _lastTitle;
  String _lastArtist;
  bool _lastIsPlaying;

  void drawAlbumArt(String url);
  void drawTextInfo(String title, String artist);
  void drawControls(bool isPlaying);

  // Helpers
  void drawButton(int x, int y, int w, int h, const char *label,
                  uint16_t color);
};

#endif
