#ifndef SPOTIFY_CLIENT_H
#define SPOTIFY_CLIENT_H

#include <ArduinoJson.h>
#include <SpotifyEsp32.h>

class SpotifyClient {
public:
  SpotifyClient(Spotify &spotify, const char *refreshToken);

  // Auth
  bool refreshAccessToken();

  // Player Control
  bool play();
  bool pause();
  bool next();
  bool previous();
  bool toggleShuffle(bool state);
  bool setRepeatMode(const char *mode); // "track", "context", "off"
  bool likeTrack(const char *trackId);

  // Data Code
  // Returns true if data was successfully fetched and parsed
  int getNowPlaying(String &title, String &artist, String &albumName,
                    String &albumArtUrl, bool &isPlaying, int &progressMs,
                    int &durationMs);

private:
  Spotify *_spotify;
  const char *_refreshToken;

  // parsing helper
  String getLargestImage(JsonArray images, int minWidth);
};

#endif
