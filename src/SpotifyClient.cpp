#include "SpotifyClient.h"

SpotifyClient::SpotifyClient(Spotify &spotify, const char *refreshToken) {
  _spotify = &spotify;
  _refreshToken = refreshToken;
}

bool SpotifyClient::refreshAccessToken() {
  Serial.println("Refreshing Access Token...");
  // The library handles this internally usually, but we can enable logic here
  // if needed. FinianLandes/SpotifyEsp32 handles refresh on API calls if token
  // is expired, provided we initialized it with refresh token.
  return true;
}

bool SpotifyClient::play() {
  return _spotify->start_resume_playback().status_code == 204;
}

bool SpotifyClient::pause() {
  return _spotify->pause_playback().status_code == 204;
}

bool SpotifyClient::next() { return _spotify->skip().status_code == 204; }

bool SpotifyClient::previous() {
  return _spotify->previous().status_code == 204;
}

bool SpotifyClient::toggleShuffle(bool state) {
  return _spotify->shuffle(state).status_code == 204;
}

bool SpotifyClient::setRepeatMode(const char *mode) {
  return _spotify->repeat_mode(mode).status_code == 204;
}

bool SpotifyClient::likeTrack(const char *trackId) {
  // Check API availability for 'save_tracks_for_current_user'
  const char *ids[] = {trackId};
  return _spotify->save_tracks_for_current_user(1, ids).status_code == 200;
}

bool SpotifyClient::unlikeTrack(const char *trackId) {
  const char *ids[] = {trackId};
  return _spotify->remove_user_saved_tracks(1, ids).status_code == 200;
}

bool SpotifyClient::getLikeState(const char *trackId, bool &isLiked) {
  const char *ids[] = {trackId};
  response resp = _spotify->check_user_saved_tracks(1, ids);

  // Debug output (commented out or use Serial if available)
  // Serial.printf("Like Check: %d\n", resp.status_code);

  if (resp.status_code == 200) {
    JsonDocument &doc = resp.reply;
    if (doc.size() > 0) {
      isLiked = doc[0].as<bool>();
      return true; // Success
    }
  }
  return false; // API Error or invalid response
}

int SpotifyClient::getNowPlaying(String &title, String &artist,
                                 String &albumName, String &albumArtUrl,
                                 String &trackId, bool &isPlaying,
                                 int &progressMs, int &durationMs) {
  // Use currently_playing() which returns a response object
  response resp = _spotify->currently_playing();

  if (resp.status_code == 200) {
    // Parse JSON
    JsonDocument &doc = resp.reply;

    if (doc["item"].is<JsonObject>()) {
      JsonObject item = doc["item"];
      title = item["name"].as<String>();
      trackId = item["id"].as<String>();

      // Artist
      JsonArray artists = item["artists"];
      if (artists.size() > 0) {
        artist = artists[0]["name"].as<String>();
      } else {
        artist = "Unknown";
      }

      // Album
      JsonObject album = item["album"];
      albumName = album["name"].as<String>();

      // Image
      JsonArray images = album["images"];
      // Get 300x300 or closest which is usually index 1
      if (images.size() > 0) {
        // Find best fit or just take first?
        // Spotify images: [0]=640px, [1]=300px, [2]=64px
        // Core2 screen is 320x240, art size ~140. Index 1 (300px) is best to
        // scale down or crop? Or index 2 (64px) is too small. Index 0 too big.
        // Let's pick appropriate size logic
        for (JsonObject img : images) {
          int w = img["width"];
          if (w <= 350 && w >= 100) {
            albumArtUrl = img["url"].as<String>();
            break;
          }
        }
        if (albumArtUrl.isEmpty()) {
          albumArtUrl = images[0]["url"].as<String>();
        }
      } else {
        albumArtUrl = "";
      }

      durationMs = item["duration_ms"];
    }

    isPlaying = doc["is_playing"];
    progressMs = doc["progress_ms"];

    return 200;
  }

  return resp.status_code;
}
